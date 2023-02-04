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

#include <config/settings.hpp>
#include <utils/signal_listener.hpp>
#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/vector.hpp>

namespace scord::network {

using request = thallium::request;

class server {

public:
    template <typename... Handlers>
    explicit server(config::settings cfg, Handlers&&... handlers)
        : m_settings(std::move(cfg)) {

        using namespace std::literals;

        const std::string thallim_address =
                m_settings.transport_protocol() + "://"s +
                m_settings.bind_address() + ":"s +
                std::to_string(m_settings.remote_port());

        m_network_engine =
                thallium::engine(thallim_address, THALLIUM_SERVER_MODE);

        (set_handler(std::forward<Handlers>(handlers)), ...);
    }

    ~server();

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
    scord::config::settings m_settings;
    thallium::engine m_network_engine;
    scord::utils::signal_listener m_signal_listener;
};

} // namespace scord::network

#endif // SCORD_SERVER_HPP
