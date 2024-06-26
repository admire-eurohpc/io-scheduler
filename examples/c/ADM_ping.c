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
#include <scord/scord.h>
#include <assert.h>
#include "common.h"

int
main(int argc, char* argv[]) {

    test_info_t test_info = {
            .name = TESTNAME,
            .requires_server = true,
            .requires_controller = false,
    };

    cli_args_t cli_args;
    if(process_args(argc, argv, test_info, &cli_args)) {
        exit(EXIT_FAILURE);
    }

    ADM_server_t server = ADM_server_create("tcp", cli_args.server_address);

    ADM_return_t ret = ADM_ping(server);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout, "ADM_ping() remote procedure not completed "
                        "successfully\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
