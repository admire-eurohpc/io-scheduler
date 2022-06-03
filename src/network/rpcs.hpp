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
#include <logger.hpp>
#include <mercury.h>
#include <mercury_macros.h>
#include <mercury_proc_string.h>


// FIXME: cannot be in a namespace due to Margo limitations
// namespace scord::network::rpc {

/// ping
DECLARE_MARGO_RPC_HANDLER(ping);

/// ADM_input
MERCURY_GEN_PROC(ADM_input_in_t,
                 ((hg_const_string_t) (origin))((hg_const_string_t) (target)))

MERCURY_GEN_PROC(ADM_input_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_input);

/// ADM_output

MERCURY_GEN_PROC(ADM_output_in_t,
                 ((hg_const_string_t) (origin))((hg_const_string_t) (target)))

MERCURY_GEN_PROC(ADM_output_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_output);

/// ADM_inout

MERCURY_GEN_PROC(ADM_inout_in_t,
                 ((hg_const_string_t) (origin))((hg_const_string_t) (target)))

MERCURY_GEN_PROC(ADM_inout_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_inout);

/// ADM_adhoc_context

MERCURY_GEN_PROC(ADM_adhoc_context_in_t, ((hg_const_string_t) (context)))

MERCURY_GEN_PROC(ADM_adhoc_context_out_t,
                 ((int32_t) (ret))((int32_t) (adhoc_context)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_context);

/// ADM_adhoc_context_id

MERCURY_GEN_PROC(ADM_adhoc_context_id_in_t, ((int32_t) (context_id)))

MERCURY_GEN_PROC(ADM_adhoc_context_id_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_context_id);

/// ADM_adhoc_nodes

MERCURY_GEN_PROC(ADM_adhoc_nodes_in_t, ((int32_t) (nodes)))

MERCURY_GEN_PROC(ADM_adhoc_nodes_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_nodes)

/// ADM_adhoc_walltime

MERCURY_GEN_PROC(ADM_adhoc_walltime_in_t, ((int32_t) (walltime)))

MERCURY_GEN_PROC(ADM_adhoc_walltime_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_walltime);


/// ADM_adhoc_access

MERCURY_GEN_PROC(ADM_adhoc_access_in_t, ((hg_const_string_t) (access)))

MERCURY_GEN_PROC(ADM_adhoc_access_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_access);

/// ADM_adhoc_distribution

MERCURY_GEN_PROC(ADM_adhoc_distribution_in_t,
                 ((hg_const_string_t) (data_distribution)))

MERCURY_GEN_PROC(ADM_adhoc_distribution_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_distribution);

/// ADM_adhoc_background_flush

MERCURY_GEN_PROC(ADM_adhoc_background_flush_in_t, ((hg_bool_t) (b_flush)))

MERCURY_GEN_PROC(ADM_adhoc_background_flush_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_background_flush);

/// ADM_in_situ_ops

MERCURY_GEN_PROC(ADM_in_situ_ops_in_t, ((hg_const_string_t) (in_situ)))

MERCURY_GEN_PROC(ADM_in_situ_ops_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_in_situ_ops);

/// ADM_in_transit_ops

MERCURY_GEN_PROC(ADM_in_transit_ops_in_t, ((hg_const_string_t) (in_transit)))

MERCURY_GEN_PROC(ADM_in_transit_ops_out_t, ((int32_t) (ret)))

DECLARE_MARGO_RPC_HANDLER(ADM_in_transit_ops);


/// ADM_transfer_dataset

MERCURY_GEN_PROC(
        ADM_transfer_dataset_in_t,
        ((hg_const_string_t) (source))((hg_const_string_t) (destination))(
                (hg_const_string_t) (qos_constraints))(
                (hg_const_string_t) (distribution))((int32_t) (job_id)))

MERCURY_GEN_PROC(ADM_transfer_dataset_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (transfer_handle)))

DECLARE_MARGO_RPC_HANDLER(ADM_transfer_dataset);

/// ADM_set_dataset_information

MERCURY_GEN_PROC(ADM_set_dataset_information_in_t,
                 ((int32_t) (resource_id))((hg_const_string_t) (info))(
                         (int32_t) (job_id)))

MERCURY_GEN_PROC(ADM_set_dataset_information_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

DECLARE_MARGO_RPC_HANDLER(ADM_set_dataset_information);

/// ADM_set_io_resources

MERCURY_GEN_PROC(ADM_set_io_resources_in_t,
                 ((int32_t) (tier_id))((hg_const_string_t) (resources))(
                         (int32_t) (job_id)))

MERCURY_GEN_PROC(ADM_set_io_resources_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

DECLARE_MARGO_RPC_HANDLER(ADM_set_io_resources);

/// ADM_get_transfer_priority

MERCURY_GEN_PROC(ADM_get_transfer_priority_in_t, ((int32_t) (transfer_id)))

MERCURY_GEN_PROC(ADM_get_transfer_priority_out_t,
                 ((int32_t) (ret))((int32_t) (priority)))

DECLARE_MARGO_RPC_HANDLER(ADM_get_transfer_priority);

/// ADM_set_transfer_priority

MERCURY_GEN_PROC(ADM_set_transfer_priority_in_t,
                 ((int32_t) (transfer_id))((int32_t) (n_positions)))

MERCURY_GEN_PROC(ADM_set_transfer_priority_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

DECLARE_MARGO_RPC_HANDLER(ADM_set_transfer_priority);

/// ADM_cancel_transfer

MERCURY_GEN_PROC(ADM_cancel_transfer_in_t, ((int32_t) (transfer_id)))

MERCURY_GEN_PROC(ADM_cancel_transfer_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

DECLARE_MARGO_RPC_HANDLER(ADM_cancel_transfer);

/// ADM_get_pending_transfers

MERCURY_GEN_PROC(ADM_get_pending_transfers_in_t, ((hg_const_string_t) (value)))

MERCURY_GEN_PROC(ADM_get_pending_transfers_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (pending_transfers)))

DECLARE_MARGO_RPC_HANDLER(ADM_get_pending_transfers);

/// ADM_set_qos_constraints

MERCURY_GEN_PROC(
        ADM_set_qos_constraints_in_t,
        ((hg_const_string_t) (scope))((hg_const_string_t) (qos_class))(
                (int32_t) (element_id))((hg_const_string_t) (class_value)))

MERCURY_GEN_PROC(ADM_set_qos_constraints_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

DECLARE_MARGO_RPC_HANDLER(ADM_set_qos_constraints);

/// ADM_set_qos_constraints_pull

MERCURY_GEN_PROC(ADM_set_qos_constraints_pull_in_t,
                 ((hg_const_string_t) (scope))((int32_t) (element_id)))

MERCURY_GEN_PROC(ADM_set_qos_constraints_pull_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (list)))

DECLARE_MARGO_RPC_HANDLER(ADM_set_qos_constraints_pull);

/// ADM_define_data_operation

MERCURY_GEN_PROC(ADM_define_data_operation_in_t,
                 ((hg_const_string_t) (path))((int32_t) (operation_id))(
                         (hg_const_string_t) (arguments)))

MERCURY_GEN_PROC(ADM_define_data_operation_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

DECLARE_MARGO_RPC_HANDLER(ADM_define_data_operation);

/// ADM_connect_data_operation

MERCURY_GEN_PROC(ADM_connect_data_operation_in_t,
                 ((int32_t) (operation_id))((hg_const_string_t) (input))(
                         (hg_bool_t) (stream))((hg_const_string_t) (arguments))(
                         (int32_t) (job_id)))

MERCURY_GEN_PROC(ADM_connect_data_operation_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (data))(
                         (hg_const_string_t) (operation_handle)))

DECLARE_MARGO_RPC_HANDLER(ADM_connect_data_operation);

/// ADM_finalize_data_operation

MERCURY_GEN_PROC(ADM_finalize_data_operation_in_t, ((int32_t) (operation_id)))

MERCURY_GEN_PROC(ADM_finalize_data_operation_out_t,
                 ((int32_t) (ret))((int32_t) (status)))


DECLARE_MARGO_RPC_HANDLER(ADM_finalize_data_operation);

/// ADM_link_transfer_to_data_operation

MERCURY_GEN_PROC(ADM_link_transfer_to_data_operation_in_t,
                 ((int32_t) (operation_id))((int32_t) (transfer_id))(
                         (hg_bool_t) (stream))((hg_const_string_t) (arguments))(
                         (int32_t) (job_id)))

MERCURY_GEN_PROC(ADM_link_transfer_to_data_operation_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (operation_handle)))

DECLARE_MARGO_RPC_HANDLER(ADM_link_transfer_to_data_operation);

/// ADM_get_statistics

MERCURY_GEN_PROC(ADM_get_statistics_in_t,
                 ((int32_t) (job_id))((int32_t) (job_step)))

MERCURY_GEN_PROC(ADM_get_statistics_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (job_statistics)))

DECLARE_MARGO_RPC_HANDLER(ADM_get_statistics);


//} // namespace scord::network::rpc

#endif // SCORD_NETWORK_RPCS_HPP
