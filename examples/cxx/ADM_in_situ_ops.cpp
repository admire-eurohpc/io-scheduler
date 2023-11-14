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
#include "common.hpp"

int
main(int argc, char* argv[]) {

    test_info test_info{
            .name = TESTNAME,
            .requires_server = true,
            .requires_controller = true,
    };

    const auto cli_args = process_args(argc, argv, test_info);

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(cli_args.server_address);

    fmt::print(
            stdout,
            "Calling ADM_in_situ_ops remote procedure on {} -> access method: {} ...\n",
            cli_args.controller_address, argv[2]);
    ADM_in_situ_ops_in_t in;
    in.in_situ = argv[2];
    ADM_in_situ_ops_out_t out;

    endp.call("ADM_in_situ_ops", &in, &out);

    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_in_situ_ops remote procedure not completed successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(stdout,
                   "ADM_in_situ_ops remote procedure completed successfully\n");
    }
}
