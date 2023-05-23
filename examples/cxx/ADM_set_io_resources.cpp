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

#include <fmt/format.h>
#include <scord/scord.hpp>
#include "common.hpp"


int
main(int argc, char* argv[]) {

    test_info test_info{
            .name = TESTNAME,
            .requires_server = true,
            .requires_controller = true,
    };

    const auto cli_args = process_args(argc, argv, test_info);

    scord::server server{"tcp", cli_args.server_address};

    ADM_job_t job{};
    ADM_adhoc_storage_t tier{};
    ADM_adhoc_resources_t resources{};
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = scord::set_io_resources(server, job, tier, resources);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_set_io_resources() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_set_io_resources() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_set_io_resources() remote procedure completed "
                       "successfully\n");
}
