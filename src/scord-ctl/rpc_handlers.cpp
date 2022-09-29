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

#include <logger/logger.hpp>
#include <net/proto/rpc_types.h>
#include <net/engine.hpp>
#include "rpc_handlers.hpp"

struct remote_procedure {
    static std::uint64_t
    new_id() {
        static std::atomic_uint64_t current_id;
        return current_id++;
    }
};

static void
ADM_ping(hg_handle_t h) {

    using scord::network::utils::get_address;

    [[maybe_unused]] hg_return_t ret;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    const auto id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{}}",
                id, std::quoted(__FUNCTION__), std::quoted(get_address(h)));

    ADM_ping_out_t out;
    out.op_id = id;
    out.retval = ADM_SUCCESS;

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                id, std::quoted(__FUNCTION__), std::quoted(get_address(h)),
                ADM_SUCCESS);

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_ping);
