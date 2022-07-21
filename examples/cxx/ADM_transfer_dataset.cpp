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
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_transfer_dataset <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_dataset_t** sources = nullptr;
    ADM_dataset_t** targets = nullptr;
    ADM_qos_limit_t** limits = nullptr;
    ADM_transfer_mapping_t mapping = ADM_MAPPING_ONE_TO_ONE;
    ADM_transfer_t tx{};
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::transfer_dataset(server, job, sources, targets, limits,
                                       mapping, &tx);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_cancel_transfer() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_transfer_dataset() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(stdout, "ADM_transfer_dataset() remote procedure completed "
                           "successfully\n");
    }
}
