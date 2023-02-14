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


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no server address provided\n");
        fmt::print(stderr, "Usage: ADM_update_pfs_storage <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    scord::server server{"tcp", argv[1]};

    std::string pfs_name = "gpfs_scratch";
    std::string pfs_mount = "/gpfs/scratch";
    std::string new_pfs_mount = "/gpfs/scratch2";

    try {

        const auto pfs_storage = scord::register_pfs_storage(
                server, pfs_name, scord::pfs_storage::type::gpfs,
                scord::pfs_storage::ctx{pfs_mount});

        scord::update_pfs_storage(server, pfs_storage,
                                  scord::pfs_storage::ctx{new_pfs_mount});
    } catch(const std::exception& e) {
        fmt::print(stderr,
                   "FATAL: ADM_update_pfs_storage() or "
                   "ADM_update_pfs_storage() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_update_pfs_storage() remote procedure completed "
                       "successfully\n");
}
