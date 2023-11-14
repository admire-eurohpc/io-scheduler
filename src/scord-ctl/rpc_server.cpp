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

#include <ranges>
#include <net/request.hpp>
#include <net/serialization.hpp>
#include <net/utilities.hpp>
#include "rpc_server.hpp"


using namespace std::literals;

namespace scord_ctl {

rpc_server::rpc_server(std::string name, std::string address, bool daemonize,
                       std::filesystem::path rundir,
                       std::optional<std::filesystem::path> pidfile)
    : server::server(std::move(name), std::move(address), std::move(daemonize),
                     std::move(rundir), std::move(pidfile)),
      provider::provider(m_network_engine, 0) {

#define EXPAND(rpc_name) "ADM_" #rpc_name##s, &rpc_server::rpc_name

    provider::define(EXPAND(ping));
    provider::define(EXPAND(deploy_adhoc_storage));
    provider::define(EXPAND(expand_adhoc_storage));
    provider::define(EXPAND(shrink_adhoc_storage));
    provider::define(EXPAND(terminate_adhoc_storage));

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
                LOGGER_INFO("          - {} = {:?}", k, (v));
            }
        }

        LOGGER_INFO("        - command:");
        LOGGER_INFO("            {:?}", (command.cmdline()));
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
        const network::request& req, const std::string& adhoc_uuid,
        enum scord::adhoc_storage::type adhoc_type,
        const scord::adhoc_storage::resources& adhoc_resources) {

    using network::get_address;
    using network::response_with_value;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));
    std::optional<std::filesystem::path> adhoc_dir;

    LOGGER_INFO("rpc {:>} body: {{uuid: {:?}, type: {}, resources: {}}}", rpc,
                (adhoc_uuid), adhoc_type, adhoc_resources);

    auto ec = scord::error_code::success;

    if(!m_config.has_value() || m_config->adhoc_storage_configs().empty()) {
        LOGGER_WARN("No adhoc storage configurations available");
        ec = scord::error_code::snafu;
        goto respond;
    }

    if(const auto it = m_config->adhoc_storage_configs().find(adhoc_type);
       it != m_config->adhoc_storage_configs().end()) {
        const auto& adhoc_cfg = it->second;

        LOGGER_DEBUG("deploy \"{:e}\" (ID: {})", adhoc_type, adhoc_uuid);

        // 1. Create a working directory for the adhoc storage instance
        adhoc_dir = adhoc_cfg.working_directory() / adhoc_uuid;

        LOGGER_DEBUG("[{}] mkdir {}", adhoc_uuid, adhoc_dir);
        std::error_code err;

        if(exists(*adhoc_dir)) {
            LOGGER_ERROR("[{}] Adhoc directory {} already exists", adhoc_uuid,
                         adhoc_cfg.working_directory());
            ec = scord::error_code::adhoc_dir_exists;
            goto respond;
        }

        if(!create_directories(*adhoc_dir, err)) {
            LOGGER_ERROR("[{}] Failed to create adhoc directory {}: {}",
                         adhoc_uuid, adhoc_cfg.working_directory(),
                         err.message());
            ec = scord::error_code::adhoc_dir_create_failed;
            goto respond;
        }

        // 3. Construct the startup command for the adhoc storage instance
        std::vector<std::string> hostnames;
        std::ranges::transform(
                adhoc_resources.nodes(), std::back_inserter(hostnames),
                [](const auto& node) { return node.hostname(); });

        const auto cmd = adhoc_cfg.startup_command().eval(
                adhoc_uuid, *adhoc_dir, hostnames);

        // 4. Execute the startup command
        try {
            LOGGER_DEBUG("[{}] exec: {}", adhoc_uuid, cmd);
            cmd.exec();
        } catch(const std::exception& ex) {
            LOGGER_ERROR("[{}] Failed to execute startup command: {}",
                         adhoc_uuid, ex.what());
            ec = scord::error_code::subprocess_error;
        }
    } else {
        LOGGER_WARN(
                "Failed to find adhoc storage configuration for type '{:e}'",
                adhoc_type);
        ec = scord::error_code::adhoc_type_unsupported;
    }

respond:
    const auto resp = response_with_value(rpc.id(), ec, adhoc_dir);

    LOGGER_INFO("rpc {:<} body: {{retval: {}, adhoc_dir: {}}}", rpc, ec,
                adhoc_dir.value_or(std::filesystem::path{}));

    req.respond(resp);
}

void
rpc_server::expand_adhoc_storage(
        const network::request& req, const std::string& adhoc_uuid,
        enum scord::adhoc_storage::type adhoc_type,
        const scord::adhoc_storage::resources& adhoc_resources) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));
    std::optional<std::filesystem::path> adhoc_dir;

    LOGGER_INFO("rpc {:>} body: {{uuid: {:?}, type: {}, resources: {}}}", rpc,
                (adhoc_uuid), adhoc_type, adhoc_resources);

    auto ec = scord::error_code::success;

    if(!m_config.has_value() || m_config->adhoc_storage_configs().empty()) {
        LOGGER_WARN("No adhoc storage configurations available");
        ec = scord::error_code::snafu;
        goto respond;
    }

    if(const auto it = m_config->adhoc_storage_configs().find(adhoc_type);
       it != m_config->adhoc_storage_configs().end()) {
        const auto& adhoc_cfg = it->second;

        LOGGER_DEBUG("deploy \"{:e}\" (ID: {})", adhoc_type, adhoc_uuid);

        // 1. Construct the expand command for the adhoc storage instance
        std::vector<std::string> hostnames;
        std::ranges::transform(
                adhoc_resources.nodes(), std::back_inserter(hostnames),
                [](const auto& node) { return node.hostname(); });

        const auto cmd = adhoc_cfg.expand_command().eval(adhoc_uuid, hostnames);

        // 4. Execute the startup command
        try {
            LOGGER_DEBUG("[{}] exec: {}", adhoc_uuid, cmd);
            cmd.exec();
        } catch(const std::exception& ex) {
            LOGGER_ERROR("[{}] Failed to execute expand command: {}",
                         adhoc_uuid, ex.what());
            ec = scord::error_code::subprocess_error;
        }
    } else {
        LOGGER_WARN(
                "Failed to find adhoc storage configuration for type '{:e}'",
                adhoc_type);
        ec = scord::error_code::adhoc_type_unsupported;
    }

respond:
    const generic_response resp{rpc.id(), ec};
    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc, resp.error_code());
    req.respond(resp);
}


void
rpc_server::shrink_adhoc_storage(
        const network::request& req, const std::string& adhoc_uuid,
        enum scord::adhoc_storage::type adhoc_type,
        const scord::adhoc_storage::resources& adhoc_resources) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));
    std::optional<std::filesystem::path> adhoc_dir;

    LOGGER_INFO("rpc {:>} body: {{uuid: {:?}, type: {}, resources: {}}}", rpc,
                (adhoc_uuid), adhoc_type, adhoc_resources);

    auto ec = scord::error_code::success;

    if(!m_config.has_value() || m_config->adhoc_storage_configs().empty()) {
        LOGGER_WARN("No adhoc storage configurations available");
        ec = scord::error_code::snafu;
        goto respond;
    }

    if(const auto it = m_config->adhoc_storage_configs().find(adhoc_type);
       it != m_config->adhoc_storage_configs().end()) {
        const auto& adhoc_cfg = it->second;

        LOGGER_DEBUG("deploy \"{:e}\" (ID: {})", adhoc_type, adhoc_uuid);

        // 1. Construct the expand command for the adhoc storage instance
        std::vector<std::string> hostnames;
        std::ranges::transform(
                adhoc_resources.nodes(), std::back_inserter(hostnames),
                [](const auto& node) { return node.hostname(); });

        const auto cmd = adhoc_cfg.shrink_command().eval(adhoc_uuid, hostnames);

        // 4. Execute the startup command
        try {
            LOGGER_DEBUG("[{}] exec: {}", adhoc_uuid, cmd);
            cmd.exec();
        } catch(const std::exception& ex) {
            LOGGER_ERROR("[{}] Failed to execute shrink command: {}",
                         adhoc_uuid, ex.what());
            ec = scord::error_code::subprocess_error;
        }
    } else {
        LOGGER_WARN(
                "Failed to find adhoc storage configuration for type '{:e}'",
                adhoc_type);
        ec = scord::error_code::adhoc_type_unsupported;
    }

respond:
    const generic_response resp{rpc.id(), ec};
    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc, resp.error_code());
    req.respond(resp);
}


void
rpc_server::terminate_adhoc_storage(
        const network::request& req, const std::string& adhoc_uuid,
        enum scord::adhoc_storage::type adhoc_type) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{uuid: {:?}, type: {}}}", rpc, (adhoc_uuid),
                adhoc_type);

    auto ec = scord::error_code::success;

    if(!m_config.has_value() || m_config->adhoc_storage_configs().empty()) {
        LOGGER_WARN("No adhoc storage configurations available");
        ec = scord::error_code::snafu;
        goto respond;
    }

    if(const auto it = m_config->adhoc_storage_configs().find(adhoc_type);
       it != m_config->adhoc_storage_configs().end()) {

        const auto& adhoc_cfg = it->second;
        const auto adhoc_dir = adhoc_cfg.working_directory() / adhoc_uuid;

        // 1. Construct the shutdown command for the adhoc storage instance
        const auto cmd =
                adhoc_cfg.shutdown_command().eval(adhoc_uuid, adhoc_dir, {});

        // 2. Execute the shutdown command
        try {
            LOGGER_DEBUG("[{}] exec: {}", adhoc_uuid, cmd);
            cmd.exec();
        } catch(const std::exception& ex) {
            LOGGER_ERROR("[{}] Failed to execute shutdown command: {}",
                         adhoc_uuid, ex.what());
            ec = scord::error_code::subprocess_error;
        }

    } else {
        LOGGER_WARN(
                "Failed to find adhoc storage configuration for type '{:e}'",
                adhoc_type);
        ec = scord::error_code::adhoc_type_unsupported;
        goto respond;
    }

respond:
    const generic_response resp{rpc.id(), ec};
    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc, resp.error_code());
    req.respond(resp);
}

} // namespace scord_ctl
