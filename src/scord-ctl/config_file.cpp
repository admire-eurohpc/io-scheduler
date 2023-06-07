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

#include <ryml_std.hpp>
#include <ryml.hpp>
#include <fmt/ostream.h>
#include <fstream>
#include "config_file.hpp"

namespace {

using namespace scord_ctl::config;

// convenience operator for creating ryml::csubstr literals
ryml::csubstr
operator""_s(const char* str, std::size_t len) {
    return {str, len};
}

// convenience function for converting ryml::csubstr to std::string
std::string
to_string(ryml::csubstr str) {
    return str.has_str() ? std::string{str.data(), str.size()} : std::string{};
}

// hash function for ryml::csubstr
constexpr auto hash = [](const ryml::csubstr& key) {
    std::string tmp{key.data(), key.size()};
    return std::hash<std::string>{}(tmp);
};

// convenience function for converting ryml::csubstr to
// scord::adhoc_storage::types
enum scord::adhoc_storage::type
to_adhoc_storage_type(const ryml::csubstr& type) {

    using scord::adhoc_storage;

    const std::unordered_map<ryml::csubstr, enum scord::adhoc_storage::type,
                             decltype(hash)>
            valid_types {
                std::make_pair("dataclay"_s, adhoc_storage::type::dataclay),
                std::make_pair("expand"_s, adhoc_storage::type::expand),
                std::make_pair("gekkofs"_s, adhoc_storage::type::gekkofs),
                std::make_pair("hercules"_s, adhoc_storage::type::hercules),
            };

    if(valid_types.count(type) == 0) {
        throw std::runtime_error{
                fmt::format("Unsupported adhoc storage type '{}' in "
                            "configuration file",
                            type)};
    }

    return valid_types.at(type);
}

/**
 * @brief Parse a ryml node into a `scord_ctl::config::environment` object.
 *
 * The node is expected to be a map with the following structure:
 *  environment:
 *  <key>: <value>
 *  ...
 *
 * @param node The ryml node to parse.
 *
 * @return The parsed `scord_ctl::config::environment` object.
 */
scord_ctl::environment
parse_environment_node(const ryml::ConstNodeRef& node) {

    scord_ctl::environment env;

    for(const auto& child : node) {
        if(!child.has_key()) {
            continue;
        }

        env.set(::to_string(child.key()), ::to_string(child.val()));
    }

    return env;
}

/**
 * @brief Parse a ryml node into a `scord_ctl::config::command` object.
 *
 * The node is expected to be a map with the following structure:
 *  environment:
 *    <key>: <value>
 *    ...
 *  command: <value>
 *
 * @param node The ryml node to parse.
 *
 * @return The parsed `scord_ctl::config::command` object.
 */
scord_ctl::command
parse_command_node(const ryml::ConstNodeRef& node) {

    std::string cmdline;
    std::optional<scord_ctl::environment> env;

    for(const auto& child : node) {
        if(!child.has_key()) {
            continue;
        }

        if(child.key() == "environment") {
            env = ::parse_environment_node(child);
        } else if(child.key() == "command") {
            if(child.val_is_null()) {
                throw std::runtime_error{"`command` key cannot be empty"};
            }
            cmdline = ::to_string(child.val());
        } else {
            fmt::print(stderr, "WARNING: Unknown key: '{}'. Ignored.\n",
                       child.key());
        }
    }

    if(cmdline.empty()) {
        throw std::runtime_error{"missing required `command` key"};
    }

    return scord_ctl::command{cmdline, env};
}

/**
 * @brief Parse a ryml node into a `scord_ctl::config::adhoc_storage_config`
 * object.
 *
 * The node is expected to be a map with the following structure:
 *  <adhoc_type>:
 *    working_directory: <value>
 *    startup:
 *      environment:
 *        <key>: <value>
 *        ...
 *      command: <value>
 *    shutdown:
 *      environment:
 *        <key>: <value>
 *        ...
 *      command: <value>
 *
 * @param node The ryml node to parse.
 * @param tag  A tag to dispatch the parsing to the correct overload.
 *
 * @return The parsed `scord_ctl::config::adhoc_storage_config` object.
 */
adhoc_storage_config
parse_adhoc_config_node(const ryml::ConstNodeRef& node) {

    std::filesystem::path working_directory;
    std::optional<scord_ctl::command> startup_command;
    std::optional<scord_ctl::command> shutdown_command;

    for(const auto& child : node) {

        if(!child.has_key()) {
            continue;
        }

        if(child.key() == "working_directory") {
            if(child.val_is_null()) {
                throw std::runtime_error{
                        "`working_directory` key cannot be empty"};
            }
            working_directory = ::to_string(child.val());
        } else if(child.key() == "startup") {
            startup_command = ::parse_command_node(child);
        } else if(child.key() == "shutdown") {
            shutdown_command = ::parse_command_node(child);
        } else {
            fmt::print(stderr, "WARNING: Unknown key: '{}'. Ignored.\n",
                       child.key());
        }
    }

    if(working_directory.empty()) {
        throw std::runtime_error{"missing required `working_directory` key"};
    }

    return {working_directory, *startup_command, *shutdown_command};
}

/**
 * @brief Parse a ryml node into a `scord_ctl::config::adhoc_storage_config_map`
 * object.
 *
 * The node is expected to be a map with the following structure:
 * adhoc_storage:
 *   <adhoc_type>:
 *     <adhoc_storage_config>
 *   <adhoc_type>:
 *     <adhoc_storage_config>
 *   ...
 *
 * @param node
 * @return The parsed `scord_ctl::config::adhoc_storage_config_map` object.
 */
adhoc_storage_config_map
parse_adhoc_storage_node(const ryml::ConstNodeRef& node) {

    adhoc_storage_config_map adhoc_configs;

    for(const auto& child : node) {
        if(!child.has_key()) {
            continue;
        }

        const auto adhoc_type = ::to_adhoc_storage_type(child.key());
        const auto adhoc_config = ::parse_adhoc_config_node(child);
        adhoc_configs.emplace(adhoc_type, adhoc_config);
    }

    return adhoc_configs;
}

/**
 * @brief Parse a ryml node into a `scord_ctl::config::adhoc_storage_config_map`
 * object.
 *
 * The node is expected to be a map with the following structure:
 *
 * config:
 *  adhoc_storage:
 *    <adhoc_type>:
 *      <adhoc_storage_config>
 *    <adhoc_type>:
 *      <adhoc_storage_config>
 *    ...
 *
 * @param node The ryml node to parse.
 * @return The parsed `scord_ctl::config::adhoc_storage_config_map` object.
 */
adhoc_storage_config_map
parse_config_node(const ryml::ConstNodeRef& node) {

    adhoc_storage_config_map adhoc_configs;

    for(const auto& child : node) {
        if(!child.has_key()) {
            continue;
        }

        if(child.key() == "adhoc_storage") {
            adhoc_configs = ::parse_adhoc_storage_node(child);
        } else {
            fmt::print(stderr, "WARNING: Unknown key: '{}'. Ignored.\n",
                       child.key());
        }
    }

    return adhoc_configs;
}

} // namespace

namespace scord_ctl::config {

adhoc_storage_config::adhoc_storage_config(
        std::filesystem::path working_directory, command startup_command,
        command shutdown_command)
    : m_working_directory(std::move(working_directory)),
      m_startup_command(std::move(startup_command)),
      m_shutdown_command(std::move(shutdown_command)) {}

const std::filesystem::path&
adhoc_storage_config::working_directory() const {
    return m_working_directory;
}

const command&
adhoc_storage_config::startup_command() const {
    return m_startup_command;
}

const command&
adhoc_storage_config::shutdown_command() const {
    return m_shutdown_command;
}

config_file::config_file(const std::filesystem::path& path) {
    std::ifstream input{path};

    if(!input) {
        throw std::runtime_error{"Failed to open configuration file: " +
                                 path.string()};
    }

    std::string input_str{std::istreambuf_iterator<char>(input),
                          std::istreambuf_iterator<char>()};


    const auto tree = ryml::parse_in_arena(ryml::to_csubstr(input_str));

    for(const auto& child : tree.crootref()) {
        if(!child.has_key()) {
            continue;
        }

        try {
            if(child.key() == "config"_s) {
                m_adhoc_configs = ::parse_config_node(child);
            }
        } catch(const std::exception& e) {
            throw std::runtime_error{
                    fmt::format("Failed parsing configuration in {}:\n  {}",
                                path, e.what())};
        }
    }
}

const adhoc_storage_config_map&
config_file::adhoc_storage_configs() const {
    return m_adhoc_configs;
}

} // namespace scord_ctl::config
