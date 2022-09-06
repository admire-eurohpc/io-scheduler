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

#define NINPUTS  10
#define NOUTPUTS 5

int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr, "Usage: ADM_deploy_adhoc_storage <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_adhoc_context_t ctx = ADM_adhoc_context_create(
            ADM_ADHOC_MODE_SEPARATE_NEW, ADM_ADHOC_ACCESS_RDWR, 42, 100, false);
    assert(ctx);

    ADM_storage_t st = ADM_storage_create("foobar", ADM_STORAGE_GEKKOFS, ctx);
    assert(st);

    ADM_storage_t adhoc_storage;

    ADM_return_t ret = ADM_register_adhoc_storage(server, ctx, &adhoc_storage);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout,
                "ADM_register_adhoc_storage() remote procedure not completed "
                "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_register_adhoc_storage() remote procedure completed "
                    "successfully\n");

    ret = ADM_deploy_adhoc_storage(server, adhoc_storage);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout,
                "ADM_deploy_adhoc_storage() remote procedure not completed "
                "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_deploy_adhoc_storage() remote procedure completed "
                    "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}
