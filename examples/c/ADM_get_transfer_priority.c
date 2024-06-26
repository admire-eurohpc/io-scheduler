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
#include <scord/scord.h>
#include <assert.h>
#include "common.h"

int
main(int argc, char* argv[]) {

    test_info_t test_info = {
            .name = TESTNAME,
            .requires_server = true,
            .requires_controller = true,
            .requires_data_stager = true,
    };

    cli_args_t cli_args;
    if(process_args(argc, argv, test_info, &cli_args)) {
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", cli_args.server_address);

    ADM_job_t job = NULL;
    ADM_node_t* job_nodes = prepare_nodes(NJOB_NODES);
    assert(job_nodes);
    ADM_node_t* adhoc_nodes = prepare_nodes(NADHOC_NODES);
    assert(adhoc_nodes);
    ADM_dataset_route_t* inputs =
            prepare_routes("%s-input-dataset-%d", NINPUTS);
    assert(inputs);
    ADM_dataset_route_t* outputs =
            prepare_routes("%s-output-dataset-%d", NOUTPUTS);
    assert(outputs);
    ADM_dataset_route_t* expected_outputs =
            prepare_routes("%s-exp-output-dataset-%d", NEXPOUTPUTS);
    assert(expected_outputs);

    ADM_job_resources_t job_resources =
            ADM_job_resources_create(job_nodes, NJOB_NODES);
    assert(job_resources);

    ADM_adhoc_resources_t adhoc_resources =
            ADM_adhoc_resources_create(adhoc_nodes, NADHOC_NODES);
    assert(adhoc_resources);

    ADM_adhoc_context_t ctx = ADM_adhoc_context_create(
            cli_args.controller_address, cli_args.data_stager_address,
            ADM_ADHOC_MODE_SEPARATE_NEW, ADM_ADHOC_ACCESS_RDWR, 100, false);
    assert(ctx);

    const char* name = "adhoc_storage_42";

    ADM_adhoc_storage_t adhoc_storage;
    ADM_return_t ret =
            ADM_register_adhoc_storage(server, name, ADM_ADHOC_STORAGE_GEKKOFS,
                                       ctx, adhoc_resources, &adhoc_storage);

    if(ret != ADM_SUCCESS) {
        fprintf(stderr,
                "ADM_register_adhoc_storage() remote procedure not completed "
                "successfully: %s\n",
                ADM_strerror(ret));
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    ADM_job_requirements_t reqs = ADM_job_requirements_create(
            inputs, NINPUTS, outputs, NOUTPUTS, expected_outputs, NEXPOUTPUTS,
            adhoc_storage);
    assert(reqs);

    uint64_t slurm_job_id = 42;
    ret = ADM_register_job(server, job_resources, reqs, slurm_job_id, &job);

    if(ret != ADM_SUCCESS) {
        fprintf(stderr,
                "ADM_register_job() remote procedure not completed "
                "successfully: %s\n",
                ADM_strerror(ret));
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    ADM_dataset_t* sources = NULL;
    size_t sources_len = 0;
    ADM_dataset_t* targets = NULL;
    size_t targets_len = 0;
    ADM_qos_limit_t* limits = NULL;
    size_t limits_len = 0;
    ADM_transfer_mapping_t mapping = ADM_MAPPING_ONE_TO_ONE;
    ADM_transfer_t tx;

    ret = ADM_transfer_datasets(server, job, sources, sources_len, targets,
                                targets_len, limits, limits_len, mapping, &tx, false);

    if(ret != ADM_SUCCESS) {
        fprintf(stderr,
                "ADM_transfer_datasets() remote procedure not "
                "completed successfully: %s\n",
                ADM_strerror(ret));
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }
    ADM_transfer_priority_t priority;

    ret = ADM_get_transfer_priority(server, job, tx, &priority);

    if(ret != ADM_SUCCESS) {
        fprintf(stderr,
                "ADM_get_transfer_priority() remote procedure not completed "
                "successfully: %s\n",
                ADM_strerror(ret));
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_get_transfer_priority() remote procedure completed "
                    "successfully\n");

cleanup:
    ADM_remove_job(server, job);
    ADM_server_destroy(server);
    exit(exit_status);
}
