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

#ifndef SCORD_SERVER_HPP
#define SCORD_SERVER_HPP

#include <memory>
#include <atomic>

namespace scord {

namespace config {
struct settings;
} // namespace config

class server {

public:
    server();
    ~server();
    void
    configure(const config::settings& settings);
    config::settings
    get_configuration() const;
    int
    run();
    void
    shutdown();
    void
    teardown();
    void
    teardown_and_exit();

private:
    int
    daemonize();
    void
    signal_handler(int);

    void
    init_logger();
    void
    init_event_handlers();
    void
    init_namespace_manager();
    void
    init_task_manager();
    void
    load_backend_plugins();
    void
    load_transfer_plugins();
    void
    load_default_namespaces();
    void
    check_configuration();
    void
    print_greeting();
    void
    print_configuration();
    void
    print_farewell();

private:
    std::atomic<bool> m_is_paused;
    std::unique_ptr<config::settings> m_settings;
};


} // namespace scord

#endif // SCORD_SERVER_HPP
