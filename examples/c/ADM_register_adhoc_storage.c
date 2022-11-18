/******************************************************************************
 * Copyright 2021, Barcelona Supercomputing Center (BSC), Spain
 *
 * This software was partially supported by the EuroHPC-funded project ADMIRE
 *   (Project ID: 956748, https://www.admire-eurohpc.eu).
 *
 * This file is part of scord.
 *
 * scord is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * scord is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with scord.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <admire.h>
#include "common.h"

#define NADHOC_NODES 25
#define NINPUTS      10
#define NOUTPUTS     5

int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr, "Usage: ADM_register_adhoc_storage <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_FAILURE;
    ADM_return_t ret = ADM_SUCCESS;
    ADM_server_t server = NULL;

    // adhoc information
    const char* adhoc_name = "adhoc_storage_42";
    ADM_node_t* adhoc_nodes = NULL;
    ADM_adhoc_resources_t adhoc_resources = NULL;
    ADM_adhoc_context_t adhoc_ctx = NULL;
    ADM_adhoc_storage_t adhoc_storage = NULL;

    // Let's prepare all the information required by the API calls.
    // ADM_register_adhoc_storage() requires a set of nodes for the adhoc
    // storage to use and an appropriate execution context defining how it
    // should behave:

    // 1. prepare the nodes
    adhoc_nodes = prepare_nodes(NADHOC_NODES);

    if(adhoc_nodes == NULL) {
        fprintf(stderr, "Fatal error preparing adhoc nodes\n");
        goto cleanup;
    }

    // 2. define the adhoc storage resources
    adhoc_resources = ADM_adhoc_resources_create(adhoc_nodes, NADHOC_NODES);

    if(adhoc_resources == NULL) {
        fprintf(stderr, "Fatal error preparing adhoc resources\n");
        goto cleanup;
    }

    // 3. define the adhoc execution context
    adhoc_ctx = ADM_adhoc_context_create(ADM_ADHOC_MODE_SEPARATE_NEW,
                                         ADM_ADHOC_ACCESS_RDWR, adhoc_resources,
                                         100, false);

    if(adhoc_ctx == NULL) {
        fprintf(stderr, "Fatal error preparing adhoc context\n");
        goto cleanup;
    }

    // All the information required by the ADM_register_adhoc_storage() API is
    // now ready. Let's actually contact the server:

    // 1. Find the server endpoint
    if((server = ADM_server_create("tcp", argv[1])) == NULL) {
        fprintf(stderr, "Fatal error creating server\n");
        goto cleanup;
    }

    // 2. Register the adhoc storage
    if(ADM_register_adhoc_storage(server, adhoc_name, ADM_ADHOC_STORAGE_GEKKOFS,
                                  adhoc_ctx, &adhoc_storage) != ADM_SUCCESS) {
        fprintf(stderr, "ADM_register_adhoc_storage() failed: %s\n",
                ADM_strerror(ret));
        goto cleanup;
    }

    // The adhoc storage is now registered into the system :)
    exit_status = EXIT_SUCCESS;

    // Once the adhoc storage is no longer required we need to notify the server
    if((ret = ADM_remove_adhoc_storage(server, adhoc_storage)) != ADM_SUCCESS) {
        fprintf(stderr, "ADM_remove_adhoc_storage() failed: %s\n",
                ADM_strerror(ret));
        adhoc_storage = NULL;
        exit_status = EXIT_FAILURE;
        // intentionally fall through...
    }

cleanup:
    ADM_server_destroy(server);

    ADM_adhoc_context_destroy(adhoc_ctx);
    ADM_adhoc_resources_destroy(adhoc_resources);
    destroy_nodes(adhoc_nodes, NADHOC_NODES);

    exit(exit_status);
}
