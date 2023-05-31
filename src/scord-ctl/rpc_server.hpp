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


#ifndef SCORD_CTL_RPC_SERVER_HPP
#define SCORD_CTL_RPC_SERVER_HPP

#include <net/server.hpp>
#include <scord/types.hpp>
#include "config_file.hpp"

namespace scord_ctl {

class rpc_server : public network::server,
                   public network::provider<rpc_server> {

public:
    rpc_server(std::string name, std::string address, bool daemonize,
               std::filesystem::path rundir);

    void
    set_config(std::optional<config::config_file> config);

    void
    print_configuration() const final;

private:
    void
    ping(const network::request& req);

    void
    deploy_adhoc_storage(
            const network::request& req, const std::string& adhoc_uuid,
            enum scord::adhoc_storage::type adhoc_type,
            const scord::adhoc_storage::resources& adhoc_resources);

    std::optional<config::config_file> m_config;
};

} // namespace scord_ctl

#endif // SCORD_SCORD_CTL_RPC_SERVER_HPP
