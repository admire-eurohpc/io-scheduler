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
#include <net/utilities.hpp>
#include "rpc_server.hpp"

// required for ::waitpid()
#include <sys/types.h>
#include <sys/wait.h>

using namespace std::literals;

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
rpc_server::set_config(std::optional<config::config_file> config) {
    m_config = std::move(config);
}

void
rpc_server::print_configuration() const {

    server::print_configuration();

    if(!m_config || m_config->adhoc_storage_configs().empty()) {
        return;
    }

    const auto print_command = [](const auto& command) {
        LOGGER_INFO("        - environment:");

        if(const auto& env = command.env(); env.has_value()) {
            for(const auto& [k, v] : *env) {
                LOGGER_INFO("          - {} = {}", k, std::quoted(v));
            }
        }

        LOGGER_INFO("        - command:");
        LOGGER_INFO("            {}", std::quoted(command.cmdline()));
    };

    LOGGER_INFO("  - adhoc storage configurations:");

    for(const auto& [type, adhoc_cfg] : m_config->adhoc_storage_configs()) {
        LOGGER_INFO("    * {:e}:", type);
        LOGGER_INFO("      - workdir: {}", adhoc_cfg.working_directory());
        LOGGER_INFO("      - startup:");
        print_command(adhoc_cfg.startup_command());
        LOGGER_INFO("      - shutdown:");
        print_command(adhoc_cfg.shutdown_command());
    }
    LOGGER_INFO("");
}


#define RPC_NAME() ("ADM_"s + __FUNCTION__)

void
rpc_server::ping(const network::request& req) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{}}", rpc);

    const auto resp = generic_response{rpc.id(), scord::error_code::success};

    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc,
                scord::error_code::success);

    req.respond(resp);
}

void
rpc_server::deploy_adhoc_storage(
        const network::request& req,
        const enum scord::adhoc_storage::type adhoc_type,
        const scord::adhoc_storage::ctx& adhoc_ctx,
        const scord::adhoc_storage::resources& adhoc_resources) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{type: {}, ctx: {}, resources: {}}}", rpc,
                adhoc_type, adhoc_ctx, adhoc_resources);

    auto ec = scord::error_code::success;

    if(adhoc_type == scord::adhoc_storage::type::gekkofs) {
        /* Number of nodes */
        const std::string nodes =
                std::to_string(adhoc_resources.nodes().size());

        /* Walltime */
        const std::string walltime = std::to_string(adhoc_ctx.walltime());

        /* Launch script */
        switch(const auto pid = fork()) {
            case 0: {
                std::vector<const char*> args;
                args.push_back("gkfs");
                // args.push_back("-c");
                // args.push_back("gkfs.conf");
                args.push_back("-n");
                args.push_back(nodes.c_str());
                // args.push_back("-w");
                // args.push_back(walltime.c_str());
                args.push_back("--srun");
                args.push_back("start");
                args.push_back(NULL);
                std::vector<const char*> env;
                env.push_back(NULL);

                execvpe("gkfs", const_cast<char* const*>(args.data()),
                        const_cast<char* const*>(env.data()));
                LOGGER_INFO("ADM_deploy_adhoc_storage() script didn't execute");
                exit(EXIT_FAILURE);
                break;
            }
            case -1: {
                ec = scord::error_code::other;
                LOGGER_ERROR("rpc {:<} body: {{retval: {}}}", rpc, ec);
                break;
            }
            default: {
                int wstatus = 0;
                pid_t retwait = ::waitpid(pid, &wstatus, 0);
                if(retwait == -1) {
                    LOGGER_ERROR(
                            "rpc id: {} error_msg: \"Error waitpid code: {}\"",
                            rpc.id(), retwait);
                    ec = scord::error_code::other;
                } else {
                    if(WEXITSTATUS(wstatus) != 0) {
                        ec = scord::error_code::other;
                    } else {
                        ec = scord::error_code::success;
                    }
                }
                break;
            }
        }
    }

    const auto resp = generic_response{rpc.id(), ec};

    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc, ec);

    req.respond(resp);
}

} // namespace scord_ctl
