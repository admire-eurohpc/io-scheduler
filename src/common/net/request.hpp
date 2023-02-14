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

#ifndef SCORD_NET_REQUEST_HPP
#define SCORD_NET_REQUEST_HPP

#include <thallium.hpp>
#include <scord/types.hpp>

namespace scord::network {

using request = thallium::request;

template <typename Request>
inline std::string
get_address(Request&& req) {
    return std::forward<Request>(req).get_endpoint();
}

class generic_response {

public:
    constexpr generic_response() noexcept = default;
    constexpr generic_response(std::uint64_t op_id,
                               scord::error_code ec) noexcept
        : m_op_id(op_id), m_error_code(ec) {}

    constexpr std::uint64_t
    op_id() const noexcept {
        return m_op_id;
    }

    constexpr scord::error_code
    error_code() const noexcept {
        return m_error_code;
    }

    template <typename Archive>
    constexpr void
    serialize(Archive&& ar) {
        ar& m_op_id;
        ar& m_error_code;
    }

private:
    std::uint64_t m_op_id;
    scord::error_code m_error_code;
};

template <typename Value>
class response_with_value : public generic_response {

public:
    constexpr response_with_value() noexcept = default;

    constexpr response_with_value(std::uint64_t op_id, scord::error_code ec,
                                  std::optional<Value> value) noexcept
        : generic_response(op_id, ec), m_value(std::move(value)) {}

    constexpr auto
    value() const noexcept {
        return m_value.value();
    }

    constexpr auto
    has_value() const noexcept {
        return m_value.has_value();
    }

    template <typename Archive>
    constexpr void
    serialize(Archive&& ar) {
        ar(cereal::base_class<generic_response>(this), m_value);
    }

private:
    std::optional<Value> m_value;
};

using response_with_id = response_with_value<std::uint64_t>;

} // namespace scord::network

#endif // SCORD_NET_REQUEST_HPP
