/******************************************************************************
 * Copyright 2021-2022, Barcelona Supercomputing Center (BSC), Spain
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

int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fprintf(stderr, "ERROR: no server address provided\n");
        fprintf(stderr, "Usage: ADM_remove_pfs_storage <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_return_t ret = ADM_SUCCESS;
    ADM_server_t server = NULL;

    // pfs information
    const char* pfs_name = "gpfs_scratch";
    const char* pfs_mount = "/gpfs/scratch";
    ADM_pfs_context_t pfs_ctx = NULL;
    ADM_pfs_storage_t pfs_storage = NULL;

    // Let's prepare all the information required by the API calls.
    // ADM_remove_pfs_storage() obviously requires a PFS storage to have
    // been registered onto the system, so let's prepare first the data required
    // to call ADM_register_pfs_storage():

    // 1. define the PFS execution context
    pfs_ctx = ADM_pfs_context_create(pfs_mount);

    if(pfs_ctx == NULL) {
        fprintf(stderr, "Fatal error preparing PFS context\n");
        goto cleanup;
    }

    // All the information required by the ADM_register_pfs_storage() API is
    // now ready. Let's actually contact the server:

    // 1. Find the server endpoint
    if((server = ADM_server_create("tcp", argv[1])) == NULL) {
        fprintf(stderr, "Fatal error creating server\n");
        goto cleanup;
    }

    // 2. Register the adhoc storage
    if(ADM_register_pfs_storage(server, pfs_name, ADM_PFS_STORAGE_GPFS, pfs_ctx,
                                &pfs_storage) != ADM_SUCCESS) {
        fprintf(stderr, "ADM_register_pfs_storage() failed: %s\n",
                ADM_strerror(ret));
        goto cleanup;
    }

    // Now that we have an existing PFS storage registered into the system
    // we can try to remove it...

    if((ret = ADM_remove_pfs_storage(server, pfs_storage)) != ADM_SUCCESS) {
        fprintf(stderr, "ADM_remove_pfs_storage() failed: %s\n",
                ADM_strerror(ret));
        goto cleanup;
    }

    // Everything is fine now...
    exit_status = EXIT_SUCCESS;

cleanup:
    ADM_server_destroy(server);
    ADM_pfs_context_destroy(pfs_ctx);

    exit(exit_status);
}
