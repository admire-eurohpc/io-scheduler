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

#ifndef SCORD_CONFIG_SCHEMA_HPP
#define SCORD_CONFIG_SCHEMA_HPP

#include <filesystem>

#include "file_options/file_options.hpp"
#include "parsers.hpp"
#include "keywords.hpp"
#include "defaults.hpp"

namespace fs = std::filesystem;

namespace scord::config {

using file_options::converter;
using file_options::declare_file;
using file_options::declare_group;
using file_options::declare_list;
using file_options::declare_option;
using file_options::file_schema;
using file_options::opt_type;
using file_options::sec_type;

// define the configuration file structure and declare the supported options
const file_schema valid_options = declare_file({
        // section for global settings
        declare_section(
                keywords::global_settings, sec_type::mandatory,
                declare_group({

                        declare_option<bool>(
                                keywords::use_syslog, opt_type::mandatory,
                                converter<bool>(parsers::parse_bool)),

                        declare_option<fs::path>(
                                keywords::log_file, opt_type::optional,
                                converter<fs::path>(parsers::parse_path)),

                        declare_option<uint32_t>(
                                keywords::log_file_max_size, opt_type::optional,
                                converter<uint32_t>(parsers::parse_capacity)),

                        declare_option<fs::path>(
                                keywords::global_socket, opt_type::mandatory,
                                converter<fs::path>(parsers::parse_path)),

                        declare_option<fs::path>(
                                keywords::control_socket, opt_type::mandatory,
                                converter<fs::path>(parsers::parse_path)),

                        declare_option<std::string>(
                                keywords::transport_protocol,
                                opt_type::mandatory),

                        declare_option<std::string>(keywords::bind_address,
                                                    opt_type::mandatory),

                        declare_option<uint32_t>(
                                keywords::remote_port, opt_type::mandatory,
                                converter<uint32_t>(parsers::parse_number)),

                        declare_option<fs::path>(
                                keywords::pidfile, opt_type::mandatory,
                                converter<fs::path>(parsers::parse_path)),

                        declare_option<uint32_t>(
                                keywords::workers, opt_type::mandatory,
                                converter<uint32_t>(parsers::parse_number)),
                })),
});

} // namespace scord::config

#endif /* SCORD_CONFIG_SCHEMA_HPP */
