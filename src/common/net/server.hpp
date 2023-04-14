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

#ifndef SCORD_SERVER_HPP
#define SCORD_SERVER_HPP

#include <optional>
#include <logger/logger.hpp>
#include <utility>
#include <utils/signal_listener.hpp>
#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/vector.hpp>

namespace scord::network {

using request = thallium::request;

class server {

public:
    server(std::string name, std::string address, bool daemonize,
           fs::path rundir);

    ~server();

    template <typename... Args>
    void
    configure_logger(scord::logger_type type, Args&&... args) {
        m_logger_config =
                logger_config(m_name, type, std::forward<Args>(args)...);
    }

    int
    run();
    void
    shutdown();
    void
    teardown();
    void
    teardown_and_exit();

    template <typename Callable>
    void
    set_handler(const std::string& name, Callable&& handler) {
        m_network_engine.define(name, handler);
    }

private:
    int
    daemonize();
    void
    signal_handler(int);

    void
    init_logger();
    void
    install_signal_handlers();

    void
    check_configuration();
    void
    print_greeting();
    void
    print_configuration();
    void
    print_farewell();

private:
    std::string m_name;
    std::string m_address;
    bool m_daemonize;
    fs::path m_rundir;
    std::optional<fs::path> m_pidfile;
    logger_config m_logger_config;
    thallium::engine m_network_engine;
    scord::utils::signal_listener m_signal_listener;
};

} // namespace scord::network

#endif // SCORD_SERVER_HPP
