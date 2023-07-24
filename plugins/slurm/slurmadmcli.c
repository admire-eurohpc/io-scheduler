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
#include <scord/utils.h>
#include "defaults.h"
#include "utils.h"

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

#define TAG_NNODES                         0
#define TAG_ADHOC_TYPE                     1
#define TAG_ADHOC_OVERLAP                  2
#define TAG_ADHOC_EXCLUSIVE                3
#define TAG_ADHOC_DEDICATED                4
#define TAG_ADHOC_REMOTE                   5
#define TAG_DATASET_INPUT                  6
#define TAG_DATASET_OUTPUT                 7
#define TAG_DATASET_EXPECTED_OUTPUT        8
#define TAG_DATASET_EXPECTED_INOUT_DATASET 9

// clang-format off
SPANK_PLUGIN (admire-cli, 1)
// clang-format on

static int scord_flag = 0;

/* scord adhoc options */
static long adhoc_nnodes = 0;
static long adhoc_walltime = 0;
static ADM_adhoc_mode_t adhoc_mode = ADM_ADHOC_MODE_IN_JOB_SHARED;
static ADM_adhoc_storage_type_t adhoc_type = 0;
static char adhoc_id[ADHOCID_LEN] = {0};
ADM_dataset_route_t* input_datasets = NULL;
size_t input_datasets_count = 0;
ADM_dataset_route_t* output_datasets = NULL;
size_t output_datasets_count = 0;
ADM_dataset_route_t* expected_output_datasets = NULL;
size_t expected_output_datasets_count = 0;
ADM_dataset_route_t* expected_inout_datasets = NULL;
size_t expected_inout_datasets_count = 0;

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
    scord_server_info_t cargo_info;
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
                          .tmpdir = SCORDCTL_TMPDIR_DEFAULT},
        .cargo_info = {.addr = NULL,
                       .proto = CARGO_PROTO_DEFAULT,
                       .port = CARGO_PORT_DEFAULT,
                       .prog = CARGO_PROG_DEFAULT,
                       .tmpdir = NULL}};

static int
process_opts(int tag, const char* optarg, int remote);

struct spank_option spank_opts[] = {
        {
                "adm-adhoc", "type",
                "Deploy an ad-hoc storage of type `type` for this job. "
                "Supported ad-hoc storages are: gekkofs, expand, hercules, and "
                "dataclay. By default, it implies `--adm-adhoc-overlap`, but "
                "this behavior can be modified with the  "
                "`--adm-adhoc-exclusive` or `--adm-adhoc-dedicated flags`.",
                1,                            /* option takes an argument */
                TAG_ADHOC_TYPE,               /* option tag */
                (spank_opt_cb_f) process_opts /* callback  */
        },
        {
                "adm-adhoc-overlap", NULL,
                "Deploy the requested ad-hoc storage on the same nodes as the "
                "compute nodes, but request ad-hoc nodes to BE SHARED "
                "with the application. The number of nodes assigned to the "
                "ad-hoc storage CAN be specified with the "
                "`--adm-adhoc-nodes` option.",
                0,                            /* option takes an argument */
                TAG_ADHOC_OVERLAP,            /* option tag */
                (spank_opt_cb_f) process_opts /* callback  */
        },
        {
                "adm-adhoc-exclusive", NULL,
                "Deploy the requested ad-hoc storage on the same nodes as the "
                "compute nodes, but request ad-hoc nodes to NOT BE SHARED "
                "with the application. The number of nodes assigned to the "
                "ad-hoc storage MUST be specified with the "
                "`--adm-adhoc-nodes` option.",
                0,                            /* option takes an argument */
                TAG_ADHOC_EXCLUSIVE,          /* option tag */
                (spank_opt_cb_f) process_opts /* callback  */
        },
        {
                "adm-adhoc-dedicated", NULL,
                "Deploy the requested ad-hoc storage service will be deployed "
                "in an independent job allocation and all the nodes in this "
                "allocation will be available for it. A specific `adhoc-id` "
                "will be generated for it and will be returned to the user "
                "so that other jobs can refer to this deployed ad-hoc storage "
                "service. In this mode, the resources assigned to the ad-hoc "
                "storage service can be controlled with the normal Slurm "
                "options.",
                0,                            /* option takes an argument */
                TAG_ADHOC_DEDICATED,          /* option tag */
                (spank_opt_cb_f) process_opts /* callback  */
        },
        {
                "adm-adhoc-remote", "adhoc-id",
                "Use an independent ad-hoc storage already running in its own"
                "allocation. The service must have been previously deployed "
                "with the `--adm-adhoc-exclusive` option.",
                1,                            /* option takes an argument */
                TAG_ADHOC_REMOTE,             /* option tag */
                (spank_opt_cb_f) process_opts /* callback  */
        },
        {
                "adm-adhoc-nodes", "nnodes",
                "Dedicate `nnodes` to the ad-hoc storage service. Only "
                "valid if paired with `--adm-adhoc-overlap` and "
                "`--adm-adhoc-exclusive`. Ignored otherwise.",
                1,                            /* option takes an argument */
                TAG_NNODES,                   /* option tag */
                (spank_opt_cb_f) process_opts /* callback  */
        },
        {
                "adm-input-datasets", "dataset-route[,dataset-route...]",
                "Define datasets that should be transferred between the PFS "
                "and the ad-hoc storage service. The `dataset-route` is "
                "defined as `ORIGIN-TIER:PATH TARGET-TIER:PATH`. For example,"
                "to transfer the file `input000.dat` from the Lustre PFS to "
                "the an on-demand GekkoFS ad-hoc storage service, the option "
                "could be specified in the following manner: "
                "  \"lustre:/input.dat gekkofs:/input.dat\"",
                1,                            /* option takes an argument */
                TAG_DATASET_INPUT,            /* option tag */
                (spank_opt_cb_f) process_opts /* callback  */
        },
        {
                "adm-output-datasets", "dataset-route[,dataset-route...]",
                "Define datasets that should be automatically transferred "
                "between the ad-hoc storage system and the PFS. The ad-hoc "
                "storage will guarantee that the dataset is not transferred "
                "while there are processes accessing the file. The datasets "
                "will be transferred before the job allocation finishes if at "
                "all possible, but no hard guarantees are made.",
                1,                            /* option takes an argument */
                TAG_DATASET_OUTPUT,           /* option tag */
                (spank_opt_cb_f) process_opts /* callback  */
        },
        {
                "adm-expected-output-datasets",
                "dataset-route[,dataset-route...]",
                "Define datasets that are expected to be generated by the "
                "application. When using this option, the application itself "
                "MUST use the programmatic APIs defined in `scord-user.h`to "
                "explicitly request the transfer of the datasets.",
                1,                            /* option takes an argument */
                TAG_DATASET_EXPECTED_OUTPUT,  /* option tag */
                (spank_opt_cb_f) process_opts /* callback  */
        },
        {
                "adm-expected-inout-datasets",
                "dataset-route[,dataset-route...]",
                "Define the datasets that should be transferred INTO "
                "the ad-hoc storage AND BACK when finished.",
                1, /* option takes an argument */
                TAG_DATASET_EXPECTED_INOUT_DATASET, /* option tag */
                (spank_opt_cb_f) process_opts       /* callback  */
        },
        SPANK_OPTIONS_TABLE_END};

int
process_opts(int tag, const char* optarg, int remote) {
    (void) remote;

    slurm_debug("%s: %s(tag: %d, optarg: %s, remote: %d) called", plugin_name,
                __func__, tag, optarg, remote);

    /* srun & sbatch/salloc */
    spank_context_t sctx = spank_context();
    if(sctx != S_CTX_LOCAL && sctx != S_CTX_ALLOCATOR && sctx != S_CTX_REMOTE)
        return 0;

    /* if we're here some scord options were passed to the Slurm CLI */
    scord_flag = 1;

    switch(tag) {
        case TAG_NNODES: {
            char* endptr;
            errno = 0;

            adhoc_nnodes = strtol(optarg, &endptr, 0);
            if(errno != 0 || endptr == optarg || *endptr != '\0' ||
               adhoc_nnodes <= 0) {
                return -1;
            }

            return 0;
        }

        case TAG_ADHOC_TYPE:
            if(!strncmp(optarg, "gekkofs", strlen("gekkofs"))) {
                adhoc_type = ADM_ADHOC_STORAGE_GEKKOFS;
                return 0;
            }

            if(!strncmp(optarg, "expand", strlen("expand"))) {
                adhoc_type = ADM_ADHOC_STORAGE_EXPAND;
                return 0;
            }

            if(!strncmp(optarg, "hercules", strlen("hercules"))) {
                adhoc_type = ADM_ADHOC_STORAGE_HERCULES;
                return 0;
            }

            if(!strncmp(optarg, "dataclay", strlen("dataclay"))) {
                adhoc_type = ADM_ADHOC_STORAGE_DATACLAY;
                return 0;
            }

            return -1;

        case TAG_ADHOC_EXCLUSIVE:
            adhoc_mode = ADM_ADHOC_MODE_IN_JOB_DEDICATED;
            return 0;

        case TAG_ADHOC_DEDICATED:
            adhoc_mode = ADM_ADHOC_MODE_SEPARATE_NEW;
            return 0;

        case TAG_ADHOC_REMOTE:
            adhoc_mode = ADM_ADHOC_MODE_SEPARATE_EXISTING;
            strncpy(adhoc_id, optarg, ADHOCID_LEN - 1);
            adhoc_id[ADHOCID_LEN - 1] = '\0';
            return 0;

        case TAG_DATASET_INPUT:
            if(input_datasets) {
                free(input_datasets);
            }

            if(scord_utils_parse_dataset_routes(optarg, &input_datasets,
                                                &input_datasets_count) !=
               ADM_SUCCESS) {
                slurm_error("%s: %s: failed to parse dataset route: %s",
                            plugin_name, __func__, optarg);
                return -1;
            }
            return 0;

        case TAG_DATASET_OUTPUT:
            if(output_datasets) {
                free(output_datasets);
            }

            if(scord_utils_parse_dataset_routes(optarg, &output_datasets,
                                                &output_datasets_count) !=
               ADM_SUCCESS) {
                slurm_error("%s: %s: failed to parse dataset route: %s",
                            plugin_name, __func__, optarg);
                return -1;
            }
            return 0;

        case TAG_DATASET_EXPECTED_OUTPUT:
            if(expected_output_datasets) {
                free(expected_output_datasets);
            }

            if(scord_utils_parse_dataset_routes(
                       optarg, &expected_output_datasets,
                       &expected_output_datasets_count) != ADM_SUCCESS) {
                slurm_error("%s: %s: failed to parse dataset route: %s",
                            plugin_name, __func__, optarg);
                return -1;
            }
            return 0;

        case TAG_DATASET_EXPECTED_INOUT_DATASET:
            if(expected_inout_datasets) {
                free(expected_inout_datasets);
            }

            if(scord_utils_parse_dataset_routes(
                       optarg, &expected_inout_datasets,
                       &expected_inout_datasets_count) != ADM_SUCCESS) {
                slurm_error("%s: %s: failed to parse dataset route: %s",
                            plugin_name, __func__, optarg);
                return -1;
            }
            return 0;

        default:
            return -1;
    }
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
            EXPAND_SCORD_OPT("cargo_prog", TYPE_STR, &cfg->cargo_info.prog),
            EXPAND_SCORD_OPT("cargo_port", TYPE_INT, &cfg->cargo_info.port),
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
scord_register_job(scord_plugin_config_t cfg, scord_nodelist_t nodelist,
                   uint32_t jobid) {

    int rc = 0;
    int nnodes = 0;

    ADM_server_t scord_server = NULL;
    ADM_node_t* nodes = NULL;
    ADM_job_resources_t job_resources = NULL;
    ADM_adhoc_resources_t adhoc_resources = NULL;
    ADM_adhoc_context_t adhoc_ctx = NULL;
    ADM_adhoc_storage_t adhoc_storage = NULL;
    ADM_job_requirements_t scord_reqs = NULL;
    ADM_job_t scord_job = NULL;
    char* adhoc_path = NULL;

    /* First determine the node on which to launch scord-ctl (typically the
     * first node of the allocation)  */
    ADM_node_t ctl_node = scord_nodelist_get_node(nodelist, 0);
    cfg.scordctl_info.addr = margo_address_create(
            cfg.scordctl_info.proto, ADM_node_get_hostname(ctl_node),
            cfg.scordctl_info.port);

    cfg.cargo_info.addr = margo_address_create(cfg.cargo_info.proto,
                                               ADM_node_get_hostname(ctl_node),
                                               cfg.cargo_info.port);

    if(!cfg.scordctl_info.addr) {
        slurm_error("%s: failed to compute address scordctl server",
                    plugin_name);
        return -1;
    }

    /* The Cargo master will also typically reside on the first node of the
     * allocation */
    cfg.cargo_info.addr = margo_address_create(cfg.cargo_info.proto,
                                               ADM_node_get_hostname(ctl_node),
                                               cfg.cargo_info.port);

    slurm_debug("%s: %s: scord_info:", plugin_name, __func__);
    slurm_debug("%s: %s:   addr: \"%s\",", plugin_name, __func__,
                cfg.scord_info.addr);
    slurm_debug("%s: %s:   proto: \"%s\",", plugin_name, __func__,
                cfg.scord_info.proto);
    slurm_debug("%s: %s:   port: %d,", plugin_name, __func__,
                cfg.scord_info.port);

    slurm_debug("%s: %s: scordctl_info:", plugin_name, __func__);
    slurm_debug("%s: %s:   addr: \"%s\",", plugin_name, __func__,
                cfg.cargo_info.addr);
    slurm_debug("%s: %s:   proto: \"%s\",", plugin_name, __func__,
                cfg.cargo_info.proto);
    slurm_debug("%s: %s:   port: %d,", plugin_name, __func__,
                cfg.cargo_info.port);


    slurm_debug("%s: %s: cargo_info:", plugin_name, __func__);
    slurm_debug("%s: %s:   addr: \"%s\",", plugin_name, __func__,
                cfg.scordctl_info.addr);
    slurm_debug("%s: %s:   proto: \"%s\",", plugin_name, __func__,
                cfg.scordctl_info.proto);
    slurm_debug("%s: %s:   port: %d,", plugin_name, __func__,
                cfg.scordctl_info.port);

    slurm_debug("%s: %s: cargo_info:", plugin_name, __func__);
    slurm_debug("%s: %s:   addr: \"%s\",", plugin_name, __func__,
                cfg.cargo_info.addr);
    slurm_debug("%s: %s:   proto: \"%s\",", plugin_name, __func__,
                cfg.cargo_info.proto);
    slurm_debug("%s: %s:   port: %d,", plugin_name, __func__,
                cfg.cargo_info.port);

    /* Register the job with the scord server */
    scord_server = ADM_server_create(cfg.scord_info.proto, cfg.scord_info.addr);
    if(!scord_server) {
        slurm_error("%s: scord server creation failed", plugin_name);
        rc = -1;
        goto end;
    }

    nnodes = scord_nodelist_get_nodecount(nodelist);
    if(nnodes <= 0) {
        slurm_error("%s: wrong scord_nodelist count", plugin_name);
        rc = -1;
        goto end;
    }

    nodes = scord_nodelist_get_nodes(nodelist);
    if(!nodes) {
        slurm_error("%s: wrong scord_nodelist_nodes", plugin_name);
        rc = -1;
        goto end;
    }

    job_resources = ADM_job_resources_create(nodes, nnodes);
    if(!job_resources) {
        slurm_error("%s: job_resources creation failed", plugin_name);
        rc = -1;
        goto end;
    }

    /* take the ADHOC_NNODES first nodes for the adhoc */
    adhoc_resources = ADM_adhoc_resources_create(
            nodes, adhoc_nnodes < nnodes ? adhoc_nnodes : nnodes);
    if(!adhoc_resources) {
        slurm_error("%s: adhoc_resources creation failed", plugin_name);
        rc = -1;
        goto end;
    }

    adhoc_ctx = ADM_adhoc_context_create(
            cfg.scordctl_info.addr, cfg.cargo_info.addr, adhoc_mode,
            ADM_ADHOC_ACCESS_RDWR, adhoc_walltime, false);
    if(!adhoc_ctx) {
        slurm_error("%s: adhoc_context creation failed", plugin_name);
        rc = -1;
        goto end;
    }

    if(ADM_register_adhoc_storage(scord_server, "mystorage", adhoc_type,
                                  adhoc_ctx, adhoc_resources,
                                  &adhoc_storage) != ADM_SUCCESS) {
        slurm_error("%s: adhoc_storage registration failed", plugin_name);
        rc = -1;
        goto end;
    }

    slurm_debug("Creating job requirements: %zu inputs, %zu outputs",
                input_datasets_count, output_datasets_count);

    /* no inputs or outputs */
    scord_reqs = ADM_job_requirements_create(
            input_datasets, input_datasets_count, output_datasets,
            output_datasets_count, expected_output_datasets,
            expected_output_datasets_count, adhoc_storage);
    if(!scord_reqs) {
        slurm_error("%s: scord job_requirements creation", plugin_name);
        rc = -1;
        goto end;
    }

    if(ADM_register_job(scord_server, job_resources, scord_reqs, jobid,
                        &scord_job) != ADM_SUCCESS) {
        slurm_error("%s: scord job registration failed", plugin_name);
        rc = -1;
        goto end;
    }

    if(ADM_deploy_adhoc_storage(scord_server, adhoc_storage, &adhoc_path) !=
       ADM_SUCCESS) {
        slurm_error("%s: adhoc storage deployment failed", plugin_name);
        rc = -1;
        goto end;
    }

end:
    if(adhoc_path) {
        free(adhoc_path);
    }

    if(scord_job) {
        ADM_job_destroy(scord_job);
    }

    if(scord_reqs) {
        ADM_job_requirements_destroy(scord_reqs);
    }

    if(adhoc_storage) {
        ADM_adhoc_storage_destroy(adhoc_storage);
    }

    if(adhoc_ctx) {
        ADM_adhoc_context_destroy(adhoc_ctx);
    }

    if(adhoc_resources) {
        ADM_adhoc_resources_destroy(adhoc_resources);
    }

    if(job_resources) {
        ADM_job_resources_destroy(job_resources);
    }

    if(scord_server) {
        ADM_server_destroy(scord_server);
    }

    return rc;
}

/**
 * Called just after plugins are loaded. In remote context, this is just after
 * job step is initialized. This function is called before any plugin option
 * processing.
 *
 * ┌-----------------------┐
 * | Command | Context     |
 * ├---------|-------------┤
 * | srun    | S_CTX_LOCAL |
 * | salloc  | S_CTX_ALLOC |
 * | sbatch  | S_CTX_ALLOC |
 * └-----------------------┘
 *
 * Available in the following contexts:
 *   S_CTX_LOCAL (srun)
 *   S_CTX_ALLOCATOR (sbatch/salloc)
 *   S_CTX_REMOTE (slurmstepd)
 *   S_CTX_SLURMD (slurmd)
 */
int
slurm_spank_init(spank_t sp, int ac, char** av) {
    (void) ac;
    (void) av;

    spank_err_t rc = ESPANK_SUCCESS;

    spank_context_t sctx = spank_context();
    if(sctx == S_CTX_LOCAL || sctx == S_CTX_ALLOCATOR || sctx == S_CTX_REMOTE) {

        slurm_debug("%s: %s() registering options", plugin_name, __func__);

        /* register adm/scord options */
        struct spank_option* opt = &spank_opts[0];
        while(opt->name) {
            rc = spank_option_register(sp, opt++);
        }
    }

    return rc == ESPANK_SUCCESS ? 0 : -1;
}

/**
 * Called in local context only after all options have been processed.
 * This is called after the job ID and step IDs are available. This happens in
 * `srun` after the allocation is made, but before tasks are launched.
 *
 * ┌-----------------------┐
 * | Command | Context     |
 * ├---------|-------------┤
 * | srun    | S_CTX_LOCAL |
 * └-----------------------┘
 *
 * Available in the following contexts:
 *  S_CTX_LOCAL (srun)
 */
int
slurm_spank_local_user_init(spank_t sp, int ac, char** av) {

    (void) sp;
    (void) ac;
    (void) av;

    return 0;
}

/**
 * Called after privileges are temporarily dropped. (remote context only)
 *
 * ┌------------------------┐
 * | Command | Context      |
 * ├---------|--------------┤
 * | srun    | S_CTX_REMOTE |
 * | salloc  | S_CTX_REMOTE |
 * | sbatch  | S_CTX_REMOTE |
 * └------------------------┘
 *
 * Available in the following contexts:
 *  S_CTX_REMOTE (slurmstepd)
 */
int
slurm_spank_user_init(spank_t sp, int ac, char** av) {

    (void) sp;
    (void) ac;
    (void) av;

    /* No ADMIRE options were passed to the job, nothing to do here */
    if(!scord_flag) {
        return 0;
    }

    /* Get relative for the node executing id. Job registration is only done
     * by the node with ID 0 */
    spank_err_t rc;
    uint32_t nodeid;

    if((rc = spank_get_item(sp, S_JOB_NODEID, &nodeid)) != ESPANK_SUCCESS) {
        slurm_error("%s: failed to get node id: %s", plugin_name,
                    spank_strerror(rc));
        return -1;
    }

    slurm_debug("%s: %s: node id: %d", plugin_name, __func__, nodeid);

    if(nodeid != 0) {
        return 0;
    }

    scord_plugin_config_t cfg = default_cfg;

    if(process_config(ac, av, &cfg) != 0) {
        return -1;
    }

    /* get job id */
    uint32_t jobid;

    if((rc = spank_get_item(sp, S_JOB_ID, &jobid)) != ESPANK_SUCCESS) {
        slurm_error("%s: failed to get jobid: %s", plugin_name,
                    spank_strerror(rc));
        return -1;
    }

    slurm_debug("%s: %s: job id: %d", plugin_name, __func__, jobid);

    /* list of job nodes */
    hostlist_t hostlist = get_slurm_hostlist(sp);
    if(!hostlist) {
        slurm_error("%s: failed to retrieve hostlist", plugin_name);
        return -1;
    }

    char buf[256];
    slurm_hostlist_ranged_string(hostlist, sizeof(buf), buf);
    slurm_debug("%s: %s: hostlist: %s", plugin_name, __func__, buf);

    scord_nodelist_t nodelist = scord_nodelist_create(hostlist);

    int ec;

    if(!nodelist) {
        slurm_error("%s: failed to create nodelist", plugin_name);
        ec = -1;
        goto cleanup;
    }

    if((ec = scord_register_job(cfg, nodelist, jobid)) != 0) {
        slurm_error("%s: failed to register job with scord", plugin_name);
        ec = -1;
        goto cleanup;
    }

cleanup:
    if(cfg.scordctl_info.addr) {
        free((void*) cfg.scordctl_info.addr);
    }

    scord_nodelist_destroy(nodelist);
    slurm_hostlist_destroy(hostlist);

    return ec;
}
