/******************************************************************************
 * Copyright 2022-2023, Inria, France.
 * Copyright 2023, Barcelona Supercomputing Center (BSC), Spain.
 * All rights reserved.
 *
 * This software was partially supported by the EuroHPC-funded project ADMIRE
 *   (Project ID: 956748, https://www.admire-eurohpc.eu).
 *
 * This file is part of scord.
 *
 * scord is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * scord is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with scord.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *****************************************************************************/

#include <string.h>
#include <slurm/slurm.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "utils.h"

extern const char plugin_name[];

hostlist_t *
get_slurm_hostlist(spank_t sp) {

    /* get list of nodes. /!\ at this point env SLURM_NODELIST is
       set, but not SLURM_JOB_NODELIST! */

    char* nodelist = NULL;

    spank_context_t sctx = spank_context();

    if(sctx != S_CTX_LOCAL && sctx != S_CTX_ALLOCATOR && sctx != S_CTX_REMOTE) {
        return NULL;
    }

    if(sctx == S_CTX_LOCAL || sctx == S_CTX_ALLOCATOR) {
        nodelist = getenv("SLURM_NODELIST");

        if(!nodelist) {
            slurm_error("%s: failed to get SLURM_NODELIST", plugin_name);
            return NULL;
        }
    } else {

        spank_err_t ec = ESPANK_SUCCESS;
        int size = 256;
        char* buffer = malloc(sizeof(char) * size);

        ec = spank_getenv(sp, "SLURM_NODELIST", buffer, size);

        if(ec != ESPANK_SUCCESS) {
            slurm_error("%s: failed to get SLURM_NODELIST: %s", plugin_name,
                        spank_strerror(ec));
            return NULL;
        }

        nodelist = buffer;
    }

    slurm_debug("%s: SLURM_NODELIST=%s", plugin_name, nodelist);

    hostlist_t * hl = NULL;
    hl = slurm_hostlist_create(nodelist);

    if(!hl) {
        slurm_error("%s: slurm_hostlist_create() failed", plugin_name);
        return NULL;
    }

    return hl;
}

scord_nodelist_t
scord_nodelist_create(hostlist_t* hostlist) {

    ADM_node_t* nodes = NULL;
    char* host = NULL;

    /* get number of nodes */
    int n = slurm_hostlist_count(hostlist);
    if(n <= 0) {
        slurm_error("%s: slurm_hostlist_count() failed", plugin_name);
        goto error;
    }

    /* allocate array of ADM_node_t */
    nodes = calloc(n, sizeof(ADM_node_t));
    if(!nodes) {
        slurm_error("%s: calloc() failed", plugin_name);
        goto error;
    }

    /* fill array of ADM_node_t */
    for(int i = 0; i < n; i++) {
        host = slurm_hostlist_shift(hostlist);
        if(!host) {
            slurm_error("%s: slurm_hostlist_shift() failed", plugin_name);
            goto error;
        }

        nodes[i] = ADM_node_create(host, ADM_NODE_REGULAR);

        if(!nodes[i]) {
            slurm_error("%s: ADM_node_create() failed", plugin_name);
            goto error;
        }
    }

    scord_nodelist_t nodelist = calloc(1, sizeof(struct scord_nodelist));

    if(!nodelist) {
        slurm_error("%s: calloc() failed", plugin_name);
        goto error;
    }

    nodelist->nodes = nodes;
    nodelist->nnodes = n;

    return nodelist;

error:
    if(nodes) {
        for(int i = 0; i < n; i++) {
            if(nodes[i]) {
                ADM_node_destroy(nodes[i]);
            }
        }
        free(nodes);
    }

    return NULL;
}

int
scord_nodelist_get_nodecount(scord_nodelist_t nodelist) {
    return nodelist ? (int) nodelist->nnodes : -1;
}

ADM_node_t*
scord_nodelist_get_nodes(scord_nodelist_t nodelist) {
    if(!nodelist) {
        return NULL;
    }
    return nodelist->nodes;
}

ADM_node_t
scord_nodelist_get_node(scord_nodelist_t nodelist, int index) {
    if(!nodelist || index < 0 || index >= nodelist->nnodes) {
        return NULL;
    }
    return nodelist->nodes[index];
}

void
scord_nodelist_destroy(scord_nodelist_t nodelist) {
    if(nodelist) {
        if(nodelist->nodes) {
            for(ssize_t i = 0; i < nodelist->nnodes; i++) {
                if(nodelist->nodes[i]) {
                    ADM_node_destroy(nodelist->nodes[i]);
                }
            }
            free(nodelist->nodes);
        }
        free(nodelist);
    }
}

int
resolve_host(const char* hostname, char* buffer) {

    struct addrinfo hints, *result;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if((rv = getaddrinfo(hostname, NULL, &hints, &result)) != 0) {
        return rv;
    }

    // we only return the first AF_INET address
    for(struct addrinfo* rp = result; rp != NULL; rp = rp->ai_next) {
        switch(rp->ai_family) {
            case AF_INET:
                inet_ntop(AF_INET,
                          &((struct sockaddr_in*) rp->ai_addr)->sin_addr,
                          buffer, INET_ADDRSTRLEN);
                freeaddrinfo(result);
                return 0;

            default:
                continue;
        }
    }

    freeaddrinfo(result);
    return EAI_NONAME;
}

const char*
margo_address_create(const char* protocol, const char* hostname, int port) {

    const char sep[] = "://";

    if(!protocol) {
        return strndup(hostname, strlen(hostname));
    }

    if(!hostname) {
        return NULL;
    }

    int rv;
    char buffer[INET_ADDRSTRLEN];
    if((rv = resolve_host(hostname, buffer)) != 0) {
        slurm_error("%s: resolve_host() failed: %s", plugin_name,
                    gai_strerror(rv));
        return NULL;
    }

    size_t n = snprintf(NULL, 0, "%s%s%s:%d", protocol, sep, buffer, port);
    char* addr = malloc(n + 1);

    if(!addr) {
        return NULL;
    }

    snprintf(addr, n + 1, "%s%s%s:%d", protocol, sep, buffer, port);
    return addr;
}
