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
#include <admire.hpp>
#include "common.hpp"

#define NADHOC_NODES 25
#define NINPUTS      10
#define NOUTPUTS     5
#define NSOURCES     5
#define NTARGETS     5
#define NLIMITS      4

int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no server address provided\n");
        fmt::print(stderr, "Usage: ADM_transfer_datasets <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    const auto adhoc_nodes = prepare_nodes(NADHOC_NODES);
    const auto inputs = prepare_datasets("input-dataset-{}", NINPUTS);
    const auto outputs = prepare_datasets("output-dataset-{}", NOUTPUTS);

    const auto sources = prepare_datasets("source-dataset-{}", NSOURCES);
    const auto targets = prepare_datasets("target-dataset-{}", NTARGETS);
    const auto qos_limits = prepare_qos_limits(NLIMITS);
    const auto mapping = admire::transfer::mapping::n_to_n;

    auto p = std::make_unique<admire::adhoc_storage>(
            admire::storage::type::gekkofs, "foobar",
            admire::adhoc_storage::execution_mode::separate_new,
            admire::adhoc_storage::access_type::read_write,
            admire::adhoc_storage::resources{adhoc_nodes}, 100, false);

    admire::job_requirements reqs(inputs, outputs, std::move(p));

    try {
        const auto job = admire::register_job(server, reqs);
        const auto transfer = admire::transfer_datasets(
                server, job, sources, targets, qos_limits, mapping);

        fmt::print(stdout, "ADM_transfer_datasets() remote procedure completed "
                           "successfully\n");
        exit(EXIT_SUCCESS);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_transfer_datasets() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }
}
