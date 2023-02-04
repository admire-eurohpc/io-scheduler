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

// clang-format off
#ifndef SCORD_RPC_HANDLERS_HPP
#define SCORD_RPC_HANDLERS_HPP

namespace scord::network::handlers {

void ping(const scord::network::request& req);

}

#include <margo.h>

#ifdef __cplusplus
extern "C" {
#endif

// FIXME: cannot be in a namespace due to Margo limitations
// namespace scord::network::rpc {

/// ADM_register_job
DECLARE_MARGO_RPC_HANDLER(ADM_register_job);

/// ADM_update_job
DECLARE_MARGO_RPC_HANDLER(ADM_update_job);

/// ADM_remove_job
DECLARE_MARGO_RPC_HANDLER(ADM_remove_job);

/// ADM_register_adhoc_storage
DECLARE_MARGO_RPC_HANDLER(ADM_register_adhoc_storage);

/// ADM_update_adhoc_storage
DECLARE_MARGO_RPC_HANDLER(ADM_update_adhoc_storage);

/// ADM_remove_adhoc_storage
DECLARE_MARGO_RPC_HANDLER(ADM_remove_adhoc_storage);

/// ADM_deploy_adhoc_storage
DECLARE_MARGO_RPC_HANDLER(ADM_deploy_adhoc_storage);

/// ADM_register_pfs_storage
DECLARE_MARGO_RPC_HANDLER(ADM_register_pfs_storage);

/// ADM_update_pfs_storage
DECLARE_MARGO_RPC_HANDLER(ADM_update_pfs_storage);

/// ADM_remove_pfs_storage
DECLARE_MARGO_RPC_HANDLER(ADM_remove_pfs_storage);

/// ADM_input
DECLARE_MARGO_RPC_HANDLER(ADM_input);

/// ADM_output
DECLARE_MARGO_RPC_HANDLER(ADM_output);

/// ADM_inout
DECLARE_MARGO_RPC_HANDLER(ADM_inout);

/// ADM_adhoc_context
DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_context);

/// ADM_adhoc_context_id
DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_context_id);

/// ADM_adhoc_nodes
DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_nodes)

/// ADM_adhoc_walltime
DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_walltime);


/// ADM_adhoc_access
DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_access);

/// ADM_adhoc_distribution
DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_distribution);

/// ADM_adhoc_background_flush
DECLARE_MARGO_RPC_HANDLER(ADM_adhoc_background_flush);

/// ADM_in_situ_ops
DECLARE_MARGO_RPC_HANDLER(ADM_in_situ_ops);

/// ADM_in_transit_ops
DECLARE_MARGO_RPC_HANDLER(ADM_in_transit_ops);


/// ADM_transfer_datasets
DECLARE_MARGO_RPC_HANDLER(ADM_transfer_datasets);

/// ADM_set_dataset_information
DECLARE_MARGO_RPC_HANDLER(ADM_set_dataset_information);

/// ADM_set_io_resources
DECLARE_MARGO_RPC_HANDLER(ADM_set_io_resources);

/// ADM_get_transfer_priority
DECLARE_MARGO_RPC_HANDLER(ADM_get_transfer_priority);

/// ADM_set_transfer_priority
DECLARE_MARGO_RPC_HANDLER(ADM_set_transfer_priority);

/// ADM_cancel_transfer
DECLARE_MARGO_RPC_HANDLER(ADM_cancel_transfer);

/// ADM_get_pending_transfers
DECLARE_MARGO_RPC_HANDLER(ADM_get_pending_transfers);

/// ADM_set_qos_constraints
DECLARE_MARGO_RPC_HANDLER(ADM_set_qos_constraints);

/// ADM_get_qos_constraints
DECLARE_MARGO_RPC_HANDLER(ADM_get_qos_constraints);

/// ADM_define_data_operation
DECLARE_MARGO_RPC_HANDLER(ADM_define_data_operation);

/// ADM_connect_data_operation
DECLARE_MARGO_RPC_HANDLER(ADM_connect_data_operation);

/// ADM_finalize_data_operation
DECLARE_MARGO_RPC_HANDLER(ADM_finalize_data_operation);

/// ADM_link_transfer_to_data_operation
DECLARE_MARGO_RPC_HANDLER(ADM_link_transfer_to_data_operation);

/// ADM_get_statistics
DECLARE_MARGO_RPC_HANDLER(ADM_get_statistics);


//} // namespace scord::network::rpc

#ifdef __cplusplus
};
#endif

#endif // SCORD_RPC_HANDLERS_HPP
// clang-format on
