/******************************************************************************
 * Copyright 2021-2023, Barcelona Supercomputing Center (BSC), Spain
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

#include <scord/scord-user.h>
#include <stdio.h>
#include "common.h"

int
main(int argc, char** argv) {

    enum { N = 5 };

    test_info_t test_info = {
            .name = TESTNAME,
            .requires_server = true,
            .requires_controller = true,
    };

    cli_args_t cli_args;
    if(process_args(argc, argv, test_info, &cli_args)) {
        exit(EXIT_FAILURE);
    }

    const char* paths[N] = {"input00.dat", "inpu01.dat", "input02.dat",
                            "input03.dat", "input04.dat"};

    ADM_dataset_t sources[N];
    ADM_dataset_t targets[N];

    for(size_t i = 0; i < N; ++i) {
        sources[i] = ADM_dataset_create(/*"lustre", */ paths[i]);
        targets[i] = ADM_dataset_create(/*"gekkofs",*/ paths[i]);

        if(!sources[i] || !targets[i]) {
            abort();
        }
    }

    ADM_return_t ret = ADM_SUCCESS;
    ADM_transfer_t tx;

    // the library will automatically route the request to the `scord`
    // server configured in the cluster
    if((ret = ADM_transfer_datasets(sources, N, targets, N, &tx)) !=
       ADM_SUCCESS) {
        fprintf(stderr, "ADM_transfer_datasets() failed: %s\n",
                ADM_strerror(ret));
        abort();
    }

    ADM_transfer_status_t status;
    do {
        struct timespec timeout = {.tv_sec = 1, .tv_nsec = 0};
        // Wait for the transfer to complete
        ret = ADM_transfer_wait(tx, &status, &timeout);

        if(ret != ADM_SUCCESS && ret != ADM_ETIMEOUT) {
            fprintf(stderr, "ADM_transfer_wait() failed: %s\n",
                    ADM_strerror(ret));
            abort();
        }

        if(ADM_TRANSFER_SUCCEEDED(status)) {
            fprintf(stdout, "Transfer completed successfully\n");
        } else if(ADM_TRANSFER_FAILED(status)) {
            fprintf(stderr, "Transfer failed: %s\n",
                    ADM_strerror(ADM_TRANSFER_ERROR(status)));
        } else if(ADM_TRANSFER_PENDING(status)) {
            fprintf(stdout, "Transfer pending\n");
            continue;
        } else if(ADM_TRANSFER_IN_PROGRESS(status)) {
            fprintf(stdout, "Transfer in progress\n");
            continue;
        } else {
            fprintf(stderr, "Transfer status unknown\n");
            abort();
        }
    } while(!ADM_TRANSFER_SUCCEEDED(status));

    free(status);

    return 0;
}
