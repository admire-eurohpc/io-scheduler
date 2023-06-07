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


#ifndef SCORD_CTL_CONFIG_HPP
#define SCORD_CTL_CONFIG_HPP

#include <scord/types.hpp>
#include <filesystem>
#include "command.hpp"

namespace scord_ctl::config {

/**
 * @brief A class representing the configuration of an adhoc storage system.
 */
class adhoc_storage_config {

public:
    /**
     * @brief Construct an adhoc_storage_config.
     *
     * @param working_directory The directory where the adhoc storage will run.
     * @param startup_command The command to be executed to start the adhoc
     * storage.
     * @param shutdown_command The command to be executed to stop the adhoc
     * storage.
     */
    adhoc_storage_config(std::filesystem::path working_directory,
                         command startup_command, command shutdown_command);

    /**
     * @brief Get the directory where the adhoc storage will run.
     *
     * @return The directory where the adhoc storage will run.
     */
    const std::filesystem::path&
    working_directory() const;

    /**
     * @brief Get the command to be executed to start the adhoc storage.
     *
     * @return The command to be executed to start the adhoc storage.
     */
    const command&
    startup_command() const;

    /**
     * @brief Get the command to be executed to stop the adhoc storage.
     *
     * @return The command to be executed to stop the adhoc storage.
     */
    const command&
    shutdown_command() const;

private:
    std::filesystem::path m_working_directory;
    command m_startup_command;
    command m_shutdown_command;
};

#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 11
typedef enum scord::adhoc_storage::type adhoc_storage_type;
#else
using adhoc_storage_type = enum scord::adhoc_storage::type;
#endif

using adhoc_storage_config_map =
        std::unordered_map<adhoc_storage_type, adhoc_storage_config>;

/**
 * @brief A class representing the configuration file of scord-ctl.
 */
class config_file {
public:
    /**
     * @brief Construct a config_file.
     *
     * @param path The path to the configuration file.
     */
    explicit config_file(const std::filesystem::path& path);

    /**
     * @brief Get the adhoc storage configurations.
     * @return The adhoc storage configurations.
     */
    const adhoc_storage_config_map&
    adhoc_storage_configs() const;

private:
    adhoc_storage_config_map m_adhoc_configs;
};

} // namespace scord_ctl::config

#endif // SCORD_CONFIG_HPP
