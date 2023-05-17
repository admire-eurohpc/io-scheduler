/******************************************************************************
 * Copyright 2021-2023, Barcelona Supercomputing Center (BSC), Spain
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

#include <net/request.hpp>
#include <net/serialization.hpp>
#include "rpc_server.hpp"

using namespace std::literals;

struct remote_procedure {
    static std::uint64_t
    new_id() {
        static std::atomic_uint64_t current_id;
        return current_id++;
    }
};

namespace scord_ctl {

rpc_server::rpc_server(std::string name, std::string address, bool daemonize,
                       std::filesystem::path rundir)
    : server::server(std::move(name), std::move(address), std::move(daemonize),
                     std::move(rundir)),
      provider::provider(m_network_engine, 0) {

#define EXPAND(rpc_name) "ADM_" #rpc_name##s, &rpc_server::rpc_name

    provider::define(EXPAND(ping));
    provider::define(EXPAND(deploy_adhoc_storage));

#undef EXPAND
}

void
rpc_server::ping(const network::request& req) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)));

    const auto resp = generic_response{rpc_id, scord::error_code::success};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                scord::error_code::success);

    req.respond(resp);
}

void
rpc_server::deploy_adhoc_storage(const network::request& req) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{}}",
                rpc_id, std::quoted(__FUNCTION__),
                std::quoted(get_address(req)));

    const auto resp = generic_response{rpc_id, scord::error_code::success};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(__FUNCTION__),
                std::quoted(get_address(req)), scord::error_code::success);

    req.respond(resp);
}

} // namespace scord_ctl
