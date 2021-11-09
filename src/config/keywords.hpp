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

#ifndef SCORD_CONFIG_KEYWORDS_HPP
#define SCORD_CONFIG_KEYWORDS_HPP

namespace scord::config::keywords {

// section names
constexpr static const auto global_settings = "global_settings";
constexpr static const auto namespaces = "namespaces";

// option names for 'global-settings' section
constexpr static const auto use_syslog = "use_syslog";
constexpr static const auto log_file = "log_file";
constexpr static const auto log_file_max_size = "log_file_max_size";
constexpr static const auto global_socket = "global_socket";
constexpr static const auto control_socket = "control_socket";
constexpr static const auto bind_address = "bind_address";
constexpr static const auto remote_port = "remote_port";
constexpr static const auto pidfile = "pidfile";
constexpr static const auto workers = "workers";
constexpr static const auto staging_directory = "staging_directory";

// option names for 'namespaces' section
constexpr static const auto nsid = "nsid";
constexpr static const auto track_contents = "track_contents";
constexpr static const auto mountpoint = "mountpoint";
constexpr static const auto type = "type";
constexpr static const auto capacity = "capacity";
constexpr static const auto visibility = "visibility";

} // namespace scord::config::keywords

#endif /* SCORD_CONFIG_KEYWORDS_HPP */
