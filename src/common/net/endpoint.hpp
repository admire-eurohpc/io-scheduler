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

#ifndef NETWORK_ENDPOINT_HPP
#define NETWORK_ENDPOINT_HPP

#include <thallium.hpp>
#include <optional>
#include <logger/logger.hpp>

namespace network {

class endpoint {

public:
    endpoint(thallium::engine& engine, thallium::endpoint endpoint);

    std::string
    address() const;

    template <typename... Args>
    inline std::optional<thallium::packed_data<>>
    call(const std::string& rpc_name, Args&&... args) const {

        try {
            const auto rpc = m_engine.define(rpc_name);
            return std::make_optional(
                    rpc.on(m_endpoint)(std::forward<Args>(args)...));
        } catch(const std::exception& ex) {
            LOGGER_ERROR("endpoint::call() failed: {}", ex.what());
            return std::nullopt;
        }
    }

    template <typename Rep, typename Period, typename... Args>
    inline std::optional<thallium::packed_data<>>
    timed_call(const std::string& rpc_name,
               const std::chrono::duration<Rep, Period>& timeout,
               Args&&... args) const {

        using namespace std::chrono_literals;

        try {
            const auto rpc = m_engine.define(rpc_name);
            return std::make_optional(
                    rpc.on(m_endpoint)
                            .timed(timeout, std::forward<Args>(args)...));
        } catch(const std::exception& ex) {
            LOGGER_ERROR("endpoint::timed_call() failed: {}", ex.what());
            return std::nullopt;
        }
    }

    auto
    endp() const {
        return m_endpoint;
    }

    auto
    engine() const {
        return m_engine;
    }

private:
    mutable thallium::engine m_engine;
    thallium::endpoint m_endpoint;
};

} // namespace network

#endif // NETWORK_ENDPOINT_HPP
