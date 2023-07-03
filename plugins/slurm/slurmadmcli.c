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

#include <errno.h>
#include <stdint.h> /* SIZE_MAX, uint32_t, etc. */
#include <stdlib.h> /* strtoul, getenv, reallocarray */
#include <string.h> /* strchr, strncmp, strncpy */
#include <slurm/slurm.h>
#include <slurm/spank.h>

#include <scord/scord.h>
#include "defaults.h"

/**
 * Slurm SPANK plugin to handle the ADMIRE adhoc storage CLI. Options are
 * forwarded to scord on srun, salloc and sbatch. See the struct spank_option
 * for the list of options.
 *
 * Notes:
 * - --adm-adhoc-context-id will be silently truncated to ADHOCID_LEN
 *   characters, including NULL byte
 **/

#define ADHOCID_LEN   64
#define INT32_STR_LEN 16 /* 16 chars are enough to fit an int32 in decimal */

#define TAG_NNODES     1
#define TAG_WALLTIME   2
#define TAG_MODE       3
#define TAG_CONTEXT_ID 4

// clang-format off
SPANK_PLUGIN (admire-cli, 1)
// clang-format on

static int scord_flag = 0;

/* scord adhoc options */
static long adhoc_nnodes = 0;
static long adhoc_walltime = 0;
static ADM_adhoc_mode_t adhoc_mode = 0;
static char adhoc_id[ADHOCID_LEN] = {0};

/* server-related options */
typedef struct {
    const char* addr;
    const char* proto;
    int port;
    const char* prog;
    const char* tmpdir;
} scord_server_info_t;

typedef struct {
    scord_server_info_t scord_info;
    scord_server_info_t scordctl_info;
} scord_plugin_config_t;


static scord_plugin_config_t default_cfg = {
        .scord_info = {.addr = SCORD_SERVER_DEFAULT,
                       .proto = SCORD_PROTO_DEFAULT,
                       .port = SCORD_PORT_DEFAULT,
                       .prog = NULL,
                       .tmpdir = NULL},
        .scordctl_info = {.addr = NULL,
                          .proto = SCORDCTL_PROTO_DEFAULT,
                          .port = SCORDCTL_PORT_DEFAULT,
                          .prog = SCORDCTL_PROG_DEFAULT,
                          .tmpdir = SCORDCTL_TMPDIR_DEFAULT}};

static int
process_opts(int tag, const char* optarg, int remote);

struct spank_option spank_opts[] = {
        {
                "adm-adhoc-nodes", "[nnodes]",
                "Dedicate [nnodes] to the ad-hoc storage",
                1,                            /* option takes an argument */
                TAG_NNODES,                   /* option tag */
                (spank_opt_cb_f) process_opts /* callback  */
        },
        {"adm-adhoc-walltime", "[walltime]",
         "Reserve the ad-hoc storage for [walltime] seconds", 1, TAG_WALLTIME,
         (spank_opt_cb_f) process_opts},
        {"adm-adhoc-context", "[context]",
         "Mode of operation for the ad-hoc storage: in_job:shared|dedicated | separate:new|existing",
         1, TAG_MODE, (spank_opt_cb_f) process_opts},
        {"adm-adhoc-context-id", "[context_id]",
         "Context ID of the ad-hoc storage", 1, TAG_CONTEXT_ID,
         (spank_opt_cb_f) process_opts},
        SPANK_OPTIONS_TABLE_END};

int
process_opts(int tag, const char* optarg, int remote) {
    (void) remote;

    slurm_debug("%s: %s() called", plugin_name, __func__);

    /* srun & sbatch/salloc */
    spank_context_t sctx = spank_context();
    if(sctx != S_CTX_LOCAL && sctx != S_CTX_ALLOCATOR && sctx != S_CTX_REMOTE)
        return 0;

    /* if we're here some scord options were passed to the Slurm CLI */
    scord_flag = 1;

    if(tag == TAG_NNODES || tag == TAG_WALLTIME) {
        long tmp;
        char* endptr;
        errno = 0;

        tmp = strtol(optarg, &endptr, 0);
        if(errno != 0 || endptr == optarg || *endptr != '\0' || tmp <= 0) {
            return -1;
        }

        if(tag == TAG_NNODES) {
            adhoc_nnodes = tmp;
        }
        if(tag == TAG_WALLTIME) {
            adhoc_walltime = tmp;
        }

        return 0;
    }

    if(tag == TAG_MODE) {
        char* col = strchr(optarg, ':');
        int parsed = 0;

        if(col) {
            if(!strncmp(optarg, "in_job", 6)) {
                if(!strncmp(col + 1, "shared", 6)) {
                    adhoc_mode = ADM_ADHOC_MODE_IN_JOB_SHARED;
                    parsed = 1;
                }
                if(!strncmp(col + 1, "dedicated", 9)) {
                    adhoc_mode = ADM_ADHOC_MODE_IN_JOB_DEDICATED;
                    parsed = 1;
                }
            } else if(!strncmp(optarg, "separate", 8)) {
                if(!strncmp(col + 1, "new", 3)) {
                    adhoc_mode = ADM_ADHOC_MODE_SEPARATE_NEW;
                    parsed = 1;
                }
                if(!strncmp(col + 1, "existing", 8)) {
                    adhoc_mode = ADM_ADHOC_MODE_SEPARATE_EXISTING;
                    parsed = 1;
                }
            }
        }

        if(!parsed) {
            return -1;
        }
    }

    if(tag == TAG_CONTEXT_ID) {
        strncpy(adhoc_id, optarg, ADHOCID_LEN - 1);
        adhoc_id[ADHOCID_LEN - 1] = '\0';
    }

    return 0;
}

static int
process_config(int ac, char** av, scord_plugin_config_t* cfg) {

    typedef struct {
        const char* name;
        size_t len;
        enum { TYPE_INT, TYPE_STR } type;
        void* value;
    } scord_option_t;

#define EXPAND_SCORD_OPT(opt_name, type, ptr)                                  \
    { opt_name, strlen(opt_name), type, ptr }

    const scord_option_t scord_options[] = {
            EXPAND_SCORD_OPT("scord_addr", TYPE_STR, &cfg->scord_info.addr),
            EXPAND_SCORD_OPT("scord_proto", TYPE_STR, &cfg->scord_info.proto),
            EXPAND_SCORD_OPT("scordctl_prog", TYPE_STR,
                             &cfg->scordctl_info.prog),
            EXPAND_SCORD_OPT("scordctl_port", TYPE_INT,
                             &cfg->scordctl_info.port),
            EXPAND_SCORD_OPT("scordctl_tmpdir", TYPE_STR,
                             &cfg->scordctl_info.tmpdir),
    };

#undef EXPAND_SCORD_OPT

    for(int i = 0; i < ac; i++) {

        bool invalid_opt = true;

        for(uint j = 0; j < sizeof(scord_options) / sizeof(scord_option_t);
            j++) {

            scord_option_t opt_desc = scord_options[j];

            if(!strncmp(av[i], opt_desc.name, opt_desc.len)) {

                switch(opt_desc.type) {
                    case TYPE_INT: {
                        char* endptr;
                        int val = (int) strtol(av[i] + opt_desc.len + 1,
                                               &endptr, 10);
                        if(*endptr != '\0') {
                            slurm_error("%s: invalid option value: %s",
                                        plugin_name, av[i]);
                            return -1;
                        }

                        *(int*) opt_desc.value = val;
                        invalid_opt = false;
                        break;
                    }

                    case TYPE_STR:
                        *(char**) opt_desc.value = av[i] + opt_desc.len + 1;
                        invalid_opt = false;
                        break;

                    default:
                        slurm_error("%s: invalid option type: %d", plugin_name,
                                    opt_desc.type);
                        return -1;
                }
                break;
            }
        }

        if(invalid_opt) {
            slurm_error("%s: invalid option: %s", plugin_name, av[i]);
            return -1;
        }
    }

    return 0;
}

static int
scord_register_job(const char* scord_proto, const char* scord_addr,
                   const char* nodelist, uint32_t jobid) {
    int rc = 0;

    ADM_server_t scord_server;
    scord_server = ADM_server_create(scord_proto, scord_addr);
    if(!scord_server) {
        slurm_error("slurmadmcli: scord server creation failed");
        rc = -1;
        goto end;
    }

    /* list of job nodes */
    hostlist_t hl = slurm_hostlist_create(nodelist);
    if(!hl) {
        slurm_error("slurmadmcli: slurm_hostlist creation failed");
        rc = -1;
        goto end;
    }

    int nnodes = slurm_hostlist_count(hl);
    if(nnodes <= 0) {
        slurm_error("slurmadmcli: wrong slurm_hostlist count");
        rc = -1;
        goto end;
    }

    ADM_node_t* nodes = reallocarray(NULL, nnodes, sizeof(ADM_node_t));
    if(!nodes) {
        slurm_error("slurmadmcli: out of memory");
        rc = -1;
        goto end;
    }

    size_t i = 0;
    char* nodename;
    while((nodename = slurm_hostlist_shift(hl))) {
        nodes[i] = ADM_node_create(nodename, ADM_NODE_REGULAR);
        if(!nodes[i]) {
            slurm_error("slurmadmcli: scord node creation failed");
            rc = -1;
            goto end;
        }
        i++;
    }

    ADM_job_resources_t job_resources;
    job_resources = ADM_job_resources_create(nodes, nnodes);
    if(!job_resources) {
        slurm_error("slurmadmcli: job_resources creation failed");
        rc = -1;
        goto end;
    }

    /* take the ADHOC_NNODES first nodes for the adhoc */
    ADM_adhoc_resources_t adhoc_resources;
    adhoc_resources = ADM_adhoc_resources_create(
            nodes, adhoc_nnodes < nnodes ? adhoc_nnodes : nnodes);
    if(!adhoc_resources) {
        slurm_error("slurmadmcli: adhoc_resources creation failed");
        rc = -1;
        goto end;
    }

    ADM_adhoc_context_t adhoc_ctx;
    adhoc_ctx = ADM_adhoc_context_create(
            NULL, adhoc_mode, ADM_ADHOC_ACCESS_RDWR, adhoc_walltime, false);
    if(!adhoc_ctx) {
        slurm_error("slurmadmcli: adhoc_context creation failed");
        rc = -1;
        goto end;
    }

    ADM_adhoc_storage_t adhoc_storage;
    if(ADM_register_adhoc_storage(
               scord_server, "mystorage", ADM_ADHOC_STORAGE_GEKKOFS, adhoc_ctx,
               adhoc_resources, &adhoc_storage) != ADM_SUCCESS) {
        slurm_error("slurmadmcli: adhoc_storage registration failed");
        rc = -1;
        goto end;
    }

    /* no inputs or outputs */
    ADM_job_requirements_t scord_reqs;
    scord_reqs = ADM_job_requirements_create(NULL, 0, NULL, 0, adhoc_storage);
    if(!scord_reqs) {
        slurm_error("slurmadmcli: scord job_requirements creation");
        rc = -1;
        goto end;
    }

    ADM_job_t scord_job;
    if(ADM_register_job(scord_server, job_resources, scord_reqs, jobid,
                        &scord_job) != ADM_SUCCESS) {
        slurm_error("slurmadmcli: scord job registration failed");
        rc = -1;
        goto end;
    }

    if(ADM_deploy_adhoc_storage(scord_server, adhoc_storage, NULL) !=
       ADM_SUCCESS) {
        slurm_error("slurmadmcli: adhoc storage deployment failed");
        rc = -1;
        goto end;
    }

end:
    slurm_hostlist_destroy(hl);
    ADM_adhoc_resources_destroy(adhoc_resources);
    ADM_remove_job(scord_server, scord_job);
    ADM_job_requirements_destroy(scord_reqs);
    ADM_adhoc_storage_destroy(adhoc_storage);
    ADM_server_destroy(scord_server);
    return rc;
}

int
slurm_spank_init(spank_t sp, int ac, char** av) {
    (void) ac;
    (void) av;

    spank_err_t rc = ESPANK_SUCCESS;

    spank_context_t sctx = spank_context();
    if(sctx == S_CTX_LOCAL || sctx == S_CTX_ALLOCATOR) {
        /* register adm/scord options */
        struct spank_option* opt = &spank_opts[0];
        while(opt->name) {
            rc = spank_option_register(sp, opt++);
        }
    }

    return rc == ESPANK_SUCCESS ? 0 : -1;
}


int
slurm_spank_local_user_init(spank_t sp, int ac, char** av) {
    (void) sp;

    if(!scord_flag)
        return 0;

    const char* scord_addr = SCORD_SERVER_DEFAULT;
    const char* scord_proto = SCORD_PROTO_DEFAULT;
    const char* scordctl_bin = SCORDCTL_PROG_DEFAULT;
    scord_plugin_config_t cfg = default_cfg;

    if(process_config(ac, av, &cfg) != 0) {
        return -1;
    }

    /* get job id */
    spank_err_t rc;
    uint32_t jobid;
    char sjobid[INT32_STR_LEN];
    if((rc = spank_get_item(sp, S_JOB_ID, &jobid)) != ESPANK_SUCCESS) {
        slurm_error("slurmadmcli: failed to get jobid: %s", spank_strerror(rc));
        return -1;
    }
    if(snprintf(sjobid, INT32_STR_LEN, "%" PRIu32, jobid) < 0) {
        slurm_error("slurmadmcli: failed to convert jobid");
        return -1;
    }

    /* get list of nodes. /!\ at this point env SLURM_NODELIST is
       set, but not SLURM_JOB_NODELIST! */
    const char* nodelist = getenv("SLURM_NODELIST");
    if(!nodelist) {
        slurm_error("slurmadmcli: failed to get node list");
        return -1;
    }


    /* launch one scord-ctl task on one node in the allocation, let Slurm decide
     * which */
    pid_t pid;
    if((pid = fork()) < 0) {
        slurm_error("slurmadmcli: failed to start scord-ctl: %s",
                    strerror(errno));
        return -1;
    } else if(pid == 0) {
        char* argv[] = {"srun",
                        "-N1",
                        "-n1",
                        "--overlap",
                        "--cpu-bind=none",
                        "--jobid",
                        sjobid,
                        (char* const) scordctl_bin,
                        NULL};
        execvp(argv[0], argv);
        slurm_error("slurmadmcli: failed to srun scord-ctl: %s",
                    strerror(errno));
        return 0;
    }

    return scord_register_job(scord_proto, scord_addr, nodelist, jobid);
}
