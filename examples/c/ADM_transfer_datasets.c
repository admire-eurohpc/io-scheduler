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
#include <assert.h>
#include "common.h"

#define NINPUTS  10
#define NOUTPUTS 5
#define NSOURCES 5
#define NTARGETS 5
#define NLIMITS  3

int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr, "Usage: ADM_transfer_datasets <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_job_t job;
    ADM_dataset_t* inputs = prepare_datasets("input-dataset-%d", NINPUTS);
    assert(inputs);
    ADM_dataset_t* outputs = prepare_datasets("output-dataset-%d", NOUTPUTS);
    assert(outputs);

    ADM_adhoc_context_t ctx = ADM_adhoc_context_create(
            ADM_ADHOC_MODE_SEPARATE_NEW, ADM_ADHOC_ACCESS_RDWR, 42, 100, false);
    assert(ctx);

    ADM_storage_t st = ADM_storage_create("foobar", ADM_STORAGE_GEKKOFS, ctx);
    assert(st);

    ADM_job_requirements_t reqs =
            ADM_job_requirements_create(inputs, NINPUTS, outputs, NOUTPUTS, st);
    assert(reqs);

    ADM_return_t ret = ADM_register_job(server, reqs, &job);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout, "ADM_register_job() remote procedure not completed "
                        "successfully\n");
        exit_status = EXIT_FAILURE;
    }

    ADM_dataset_t* sources = prepare_datasets("source-dataset-%d", NSOURCES);
    assert(sources);
    ADM_dataset_t* targets = prepare_datasets("target-dataset-%d", NTARGETS);
    assert(targets);
    ADM_qos_limit_t* limits = prepare_qos_limits(NLIMITS);
    assert(limits);
    ADM_transfer_mapping_t mapping = ADM_MAPPING_ONE_TO_ONE;
    ADM_transfer_t tx;

    ret = ADM_transfer_datasets(server, job, sources, NSOURCES, targets,
                                NTARGETS, limits, NLIMITS, mapping, &tx);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout, "ADM_transfer_datasets() remote procedure not "
                        "completed successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_transfer_datasets() remote procedure completed "
                    "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}