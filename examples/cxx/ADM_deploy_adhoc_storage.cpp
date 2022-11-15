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
        fmt::print(stderr,
                   "Usage: ADM_deploy_adhoc_storage <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};
    std::vector<admire::node> nodes{admire::node{"node1"},admire::node{"node2"}};
    admire::adhoc_storage::resources res(nodes);

    admire::adhoc_storage adhoc_storage(
        admire::storage::type::dataclay, "foobar", 1,
        admire::adhoc_storage::execution_mode::separate_new,
        admire::adhoc_storage::access_type::read_write, 
        res, 100, false);

    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::deploy_adhoc_storage(server, adhoc_storage);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_deploy_adhoc_storage() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_deploy_adhoc_storage() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_deploy_adhoc_storage() remote procedure completed "
                       "successfully\n");
}
