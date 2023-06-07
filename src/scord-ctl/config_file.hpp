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

namespace scord_ctl::config {

/**
 * @brief A class representing the environment variables that
 * should be set when running a command.
 */
class environment {

public:
    /**
     * @brief Set an environment variable.
     *
     * @param key The name of the environment variable.
     * @param value The value of the environment variable.
     */
    void
    set(const std::string& key, const std::string& value);

    /**
     * @brief Get the value of an environment variable.
     *
     * @param key The name of the environment variable.
     * @return The value of the environment variable if it exists, an empty
     * string otherwise.
     */
    std::string
    get(const std::string& key) const;

    /**
     * @brief Get the environment variables as a vector of strings.
     * Each string is of the form `key=value`.
     *
     * @return The environment variables as a vector of strings.
     */
    std::vector<std::string>
    as_vector() const;

    /**
     * @brief Get the number of environment variables.
     *
     * @return The number of environment variables.
     */
    std::size_t
    size() const;

    /**
     * @brief Get an iterator to the beginning of the environment variables.
     *
     * @return An iterator to the beginning of the environment variables.
     */
    std::unordered_map<std::string, std::string>::const_iterator
    begin() const;

    /**
     * @brief Get an iterator to the end of the environment variables.
     *
     * @return An iterator to the end of the environment variables.
     */
    std::unordered_map<std::string, std::string>::const_iterator
    end() const;

private:
    std::unordered_map<std::string, std::string> m_env;
};

/**
 * @brief A class representing a command to be executed.
 */
class command {
public:
    /**
     * @brief Keywords that can be used in the command line and
     * will be expanded with appropriate values when calling `eval()`.
     */
    static constexpr std::array<std::string_view, 3> keywords = {
            "{ADHOC_ID}", "{ADHOC_DIRECTORY}", "{ADHOC_NODES}"};

    /**
     * @brief Construct a command.
     *
     * @param cmdline The command line to be executed.
     * @param env The environment variables to be set when executing the
     * command.
     */
    explicit command(std::string cmdline,
                     std::optional<environment> env = std::nullopt);

    /**
     * @brief Get the template command line to be executed (i.e. without having
     * keywords expanded).
     *
     * @return The command line to be executed.
     */
    const std::string&
    cmdline() const;

    /**
     * @brief Get the environment variables to be set when executing the
     * command.
     *
     * @return The environment variables to be set when executing the command.
     */
    const std::optional<environment>&
    env() const;

    /**
     * @brief Return a copy of the current `command` where all the keywords in
     * its command line template have been replaced with string
     * representations of the arguments provided.
     *
     * @param adhoc_id The ID of the adhoc storage system.
     * @param adhoc_directory The directory where the adhoc storage will run.
     * @param adhoc_nodes The nodes where the adhoc storage will run.
     * @return The evaluated command.
     */
    command
    eval(const std::string& adhoc_id,
         const std::filesystem::path& adhoc_directory,
         const std::vector<std::string>& adhoc_nodes) const;

    /**
     * @brief Get the command line to be executed as a vector of strings. The
     * command line is split on spaces with each string in the resulting
     * vector being a token in the command line.
     *
     * @return The command line to be executed as a vector of strings.
     */
    std::vector<std::string>
    as_vector() const;


private:
    std::string m_cmdline;
    std::optional<environment> m_env;
};

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

/**
 * @brief Formatter for `scord_ctl::config::command`.
 */
template <>
struct fmt::formatter<scord_ctl::config::command> {
    template <typename ParseContext>
    constexpr auto
    parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const scord_ctl::config::command& cmd, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{}", cmd.cmdline());
    }
};


#endif // SCORD_CONFIG_HPP
