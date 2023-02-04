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

#ifndef SCORD_ENDPOINT_HPP
#define SCORD_ENDPOINT_HPP

#include <thallium.hpp>
#include <optional>
#include <logger/logger.hpp>

namespace scord::network {

class endpoint {

public:
    endpoint(std::shared_ptr<thallium::engine> engine,
             thallium::endpoint endpoint);

    std::string
    address() const;

    template <typename... Args>
    auto
    call(const std::string& rpc_name, Args&&... args) const {

        // deduce the return type of the expression in the try-block below so
        // that we know the type to return within std::optional
        using rpc_function_type =
                decltype(m_engine->define(std::declval<decltype(rpc_name)>()));
        using rpc_return_type = decltype(std::declval<rpc_function_type>().on(
                m_endpoint)(std::forward<Args>(args)...));
        using return_type = std::optional<rpc_return_type>;

        try {
            const auto& rpc = m_engine->define(rpc_name);
            const auto& rv = rpc.on(m_endpoint)(std::forward<Args>(args)...);
            return return_type{rv};
        } catch(const std::exception& ex) {
            LOGGER_ERROR("endpoint::call() failed: {}", ex.what());
            return return_type{};
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
    std::shared_ptr<thallium::engine> m_engine;
    thallium::endpoint m_endpoint;
};

} // namespace scord::network

#endif // SCORD_ENDPOINT_HPP
