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
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_in_transit_ops <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_in_transit_ops remote procedure on {} -> access method: {} ...\n",
            argv[1], argv[2]);
    ADM_in_transit_ops_in_t in;
    in.in_transit = argv[2];
    ADM_in_transit_ops_out_t out;

    endp.call("ADM_in_transit_ops", &in, &out);

    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_in_transit_ops remote procedure not completed successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(
                stdout,
                "ADM_in_transit_ops remote procedure completed successfully\n");
    }
}
