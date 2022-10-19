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

#define NJOB_NODES   50
#define NADHOC_NODES 25
#define NINPUTS      10
#define NOUTPUTS     5

int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no server address provided\n");
        fmt::print(stderr, "Usage: ADM_update_job <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    const auto job_nodes = prepare_nodes(NJOB_NODES);
    const auto new_job_nodes = prepare_nodes(NJOB_NODES * 2);
    const auto adhoc_nodes = prepare_nodes(NADHOC_NODES);
    const auto inputs = prepare_datasets("input-dataset-{}", NINPUTS);
    const auto outputs = prepare_datasets("output-dataset-{}", NOUTPUTS);

    const auto gkfs_storage = admire::register_adhoc_storage(
            server, "foobar", admire::storage::type::gekkofs,
            admire::adhoc_storage::ctx{
                    admire::adhoc_storage::execution_mode::separate_new,
                    admire::adhoc_storage::access_type::read_write,
                    admire::adhoc_storage::resources{adhoc_nodes}, 100, false});

    admire::job_requirements reqs{inputs, outputs, gkfs_storage};

    const auto new_inputs = prepare_datasets("input-new-dataset-{}", NINPUTS);
    const auto new_outputs =
            prepare_datasets("output-new-dataset-{}", NOUTPUTS);

    try {
        [[maybe_unused]] const auto job = admire::register_job(
                server, admire::job::resources{job_nodes}, reqs, 0);

        [[maybe_unused]] ADM_return_t ret = admire::update_job(
                server, job, admire::job::resources{new_job_nodes});

        fmt::print(
                stdout,
                "ADM_register_job() and ADM_update_job() remote procedure completed "
                "successfully\n");
        exit(EXIT_SUCCESS);
    } catch(const std::exception& e) {
        fmt::print(stderr,
                   "FATAL: ADM_register_job() or ADM_update_job() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }
}
