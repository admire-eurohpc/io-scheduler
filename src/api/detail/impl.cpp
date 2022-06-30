/******************************************************************************
 * Copyright 2021-2022, Barcelona Supercomputing Center (BSC), Spain
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

#include <tl/expected.hpp>
#include <engine.hpp>
#include "impl.hpp"

namespace admire::detail {

admire::error_code
ping(const server& srv) {

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_ping()");
    endp.call("ADM_ping");

    LOGGER_INFO("ADM_register_job() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

tl::expected<admire::job, admire::error_code>
register_job(const admire::server& srv, ADM_job_requirements_t reqs) {
    (void) srv;
    (void) reqs;

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_register_job(...)");

    ADM_register_job_in_t in{};
    ADM_register_job_out_t out;

    endp.call("ADM_register_job", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_register_job() = {}", out.ret);
        return tl::make_unexpected(static_cast<admire::error_code>(out.ret));
    }

    LOGGER_INFO("ADM_register_job() = {}", ADM_SUCCESS);
    return admire::job{42};
}

} // namespace admire::detail
