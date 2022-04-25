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

/// ADM_adhoc_context

MERCURY_GEN_PROC(ADM_adhoc_context_in_t,
        ((hg_const_string_t)(context)))

MERCURY_GEN_PROC(ADM_adhoc_context_out_t, 
                ((int32_t)(ret))\
                ((int32_t)(adhoc_context)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_context);

/// ADM_adhoc_context_id

MERCURY_GEN_PROC(ADM_adhoc_context_id_in_t,
        ((int32_t)(context_id)))

MERCURY_GEN_PROC(ADM_adhoc_context_id_out_t, 
        ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_context_id);

/// ADM_adhoc_nodes

MERCURY_GEN_PROC(ADM_adhoc_nodes_in_t,
        ((int32_t)(nodes)))

MERCURY_GEN_PROC(ADM_adhoc_nodes_out_t, ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_nodes);

/// ADM_adhoc_walltime

MERCURY_GEN_PROC(ADM_adhoc_walltime_in_t,
        ((int32_t)(walltime)))

MERCURY_GEN_PROC(ADM_adhoc_walltime_out_t, 
        ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_walltime);

/// ADM_adhoc_access

MERCURY_GEN_PROC(ADM_adhoc_access_in_t,
        ((hg_const_string_t)(access)))

MERCURY_GEN_PROC(ADM_adhoc_access_out_t, ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_access);

/// ADM_adhoc_distribution

MERCURY_GEN_PROC(ADM_adhoc_distribution_in_t,
        ((hg_const_string_t)(data_distribution)))

MERCURY_GEN_PROC(ADM_adhoc_distribution_out_t, ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_distribution);

/// ADM_adhoc_background_flush

MERCURY_GEN_PROC(ADM_adhoc_background_flush_in_t,
        ((hg_bool_t)(b_flush)))

MERCURY_GEN_PROC(ADM_adhoc_background_flush_out_t, ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_background_flush);

/// ADM_in_situ_ops

MERCURY_GEN_PROC(ADM_in_situ_ops_in_t,
        ((hg_const_string_t)(in_situ)))

MERCURY_GEN_PROC(ADM_in_situ_ops_out_t, ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_in_situ_ops);

/// ADM_in_transit_ops

MERCURY_GEN_PROC(ADM_in_transit_ops_in_t,
        ((hg_const_string_t)(in_transit)))

MERCURY_GEN_PROC(ADM_in_transit_ops_out_t, ((int32_t)(ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_in_transit_ops);

/// ADM_transfer_dataset

MERCURY_GEN_PROC(ADM_transfer_dataset_in_t,
        ((hg_const_string_t)(source))\
        ((hg_const_string_t)(destination))\
        ((hg_const_string_t)(qos_constraints))\
        ((hg_const_string_t)(distribution))\
        ((int32_t)(job_id)))

MERCURY_GEN_PROC(ADM_transfer_dataset_out_t, 
        ((int32_t)(ret))\
        ((hg_const_string_t)(transfer_handle)))

DECLARE_MARGO_RPC_HANDLER(ADM_transfer_dataset);

/// ADM_set_dataset_information

MERCURY_GEN_PROC(ADM_set_dataset_information_in_t,
        ((int32_t)(resource_id))\
        ((hg_const_string_t)(info))\
        ((int32_t)(job_id)))

MERCURY_GEN_PROC(ADM_set_dataset_information_out_t, 
        ((int32_t)(ret))\
        ((int32_t)(status)))

DECLARE_MARGO_RPC_HANDLER(ADM_set_dataset_information);

/// ADM_set_io_resources

MERCURY_GEN_PROC(ADM_set_io_resources_in_t,
        ((int32_t)(tier_id))\
        ((hg_const_string_t)(resources))\
        ((int32_t)(job_id)))

MERCURY_GEN_PROC(ADM_set_io_resources_out_t, 
        ((int32_t)(ret))\
        ((int32_t)(status)))

DECLARE_MARGO_RPC_HANDLER(ADM_set_io_resources);


//} // namespace scord::network::rpc

#endif // SCORD_NETWORK_RPCS_HPP
