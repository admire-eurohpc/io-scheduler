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

#define NJOB_NODES   50
#define NADHOC_NODES 25
#define NINPUTS      10
#define NOUTPUTS     5

int
main(int argc, char* argv[]) {

    test_info test_info{
            .name = TESTNAME,
            .requires_server = true,
            .requires_controller = true,
            .requires_data_stager = true,
    };

    const auto cli_args = process_args(argc, argv, test_info);

    scord::server server{"tcp", cli_args.server_address};

    const auto adhoc_nodes = prepare_nodes(NADHOC_NODES);
    const auto inputs = prepare_datasets("input-dataset-{}", NINPUTS);
    const auto outputs = prepare_datasets("output-dataset-{}", NOUTPUTS);

    std::string name = "adhoc_storage_42";
     

    const auto adhoc_storage_ctx = scord::adhoc_storage::ctx{
            cli_args.controller_address,
            cli_args.data_stager_address,
            scord::adhoc_storage::execution_mode::separate_new,
            scord::adhoc_storage::access_type::read_write,
            100,
            false};
    const auto adhoc_resources = scord::adhoc_storage::resources{adhoc_nodes};

    try {
        const auto adhoc_storage = scord::register_adhoc_storage(
                server, name, scord::adhoc_storage::type::gekkofs,
                adhoc_storage_ctx, adhoc_resources);

        fmt::print(stdout,
                   "ADM_register_adhoc_storage() remote procedure completed "
                   "successfully\n");

        [[maybe_unused]] const auto adhoc_storage_path =
                scord::deploy_adhoc_storage(server, adhoc_storage);

    } catch(const std::exception& e) {
        fmt::print(stderr,
                   "FATAL: ADM_register_adhoc_storage() or "
                   "ADM_deploy_adhoc_storage() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_deploy_adhoc_storage() remote procedure completed "
                       "successfully\n");
}
