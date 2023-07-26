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

#include <fmt/format.h>
#include <scord/scord.hpp>
#include "common.hpp"

#define NJOB_NODES   50
#define NADHOC_NODES 25
#define NINPUTS      10
#define NOUTPUTS     5
#define NSOURCES     5
#define NTARGETS     5
#define NLIMITS      4

int
main(int argc, char* argv[]) {

    test_info test_info{
            .name = TESTNAME,
            .requires_server = true,
            .requires_controller = true,
    };

    const auto cli_args = process_args(argc, argv, test_info);

    scord::server server{"tcp", cli_args.server_address};

    const auto job_nodes = prepare_nodes(NJOB_NODES);
    const auto adhoc_nodes = prepare_nodes(NADHOC_NODES);
    const auto inputs = prepare_datasets("input-dataset-{}", NINPUTS);
    const auto outputs = prepare_datasets("output-dataset-{}", NOUTPUTS);

    const auto sources = prepare_datasets("source-dataset-{}", NSOURCES);
    const auto targets = prepare_datasets("target-dataset-{}", NTARGETS);
    const auto qos_limits = prepare_qos_limits(NLIMITS);
    const auto mapping = scord::transfer::mapping::n_to_n;

    std::string name = "adhoc_storage_42";
    const auto adhoc_storage_ctx = scord::adhoc_storage::ctx{
            cli_args.controller_address,
            scord::adhoc_storage::execution_mode::separate_new,
            scord::adhoc_storage::access_type::read_write, 100, false};
    const auto adhoc_resources = scord::adhoc_storage::resources{adhoc_nodes};

    try {
        const auto adhoc_storage = scord::register_adhoc_storage(
                server, name, scord::adhoc_storage::type::gekkofs,
                adhoc_storage_ctx, adhoc_resources);

        scord::job::requirements reqs(inputs, outputs, adhoc_storage);

        const auto job = scord::register_job(
                server, scord::job::resources{job_nodes}, reqs, 0);
        const auto transfer = scord::transfer_datasets(
                server, job, sources, targets, qos_limits, mapping);


        scord::transfer_update(server, transfer.id(), 10.0f);
        fmt::print(stdout, "ADM_transfer_update remote procedure completed "
                           "successfully\n");
        exit(EXIT_SUCCESS);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_transfer_update() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }
}
