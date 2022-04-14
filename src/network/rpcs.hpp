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

#ifndef SCORD_NETWORK_RPCS_HPP
#define SCORD_NETWORK_RPCS_HPP

#include <margo.h>
#include <mercury.h>
#include <mercury_macros.h>
#include <mercury_proc_string.h> 
#include <logger.hpp>

// FIXME: cannot be in a namespace due to Margo limitations
// namespace scord::network::rpc {

/// ping
DECLARE_MARGO_RPC_HANDLER(ping);

/// ADM_input
MERCURY_GEN_PROC(ADM_input_in_t,
        ((hg_const_string_t)(origin))\
        ((hg_const_string_t)(target)))

MERCURY_GEN_PROC(ADM_input_out_t, ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_input);

/// ADM_output

MERCURY_GEN_PROC(ADM_output_in_t,
        ((hg_const_string_t)(origin))\
        ((hg_const_string_t)(target)))

MERCURY_GEN_PROC(ADM_output_out_t, ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_output);

/// ADM_inout

MERCURY_GEN_PROC(ADM_inout_in_t,
        ((hg_const_string_t)(origin))\
        ((hg_const_string_t)(target)))

MERCURY_GEN_PROC(ADM_inout_out_t, ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_inout);

/// ADM_adhoc_nodes

MERCURY_GEN_PROC(ADM_adhoc_nodes_in_t,
        ((hg_const_string_t)(nodes)))

MERCURY_GEN_PROC(ADM_adhoc_nodes_out_t, ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_nodes);

//} // namespace scord::network::rpc

#endif // SCORD_NETWORK_RPCS_HPP
