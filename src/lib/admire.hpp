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

#include <admire.h>
#include <string>
#include <utility>
#include "admire_types.hpp"
#include "net/proto/rpc_types.h"

#ifndef SCORD_ADMIRE_HPP
#define SCORD_ADMIRE_HPP

/******************************************************************************/
/* Public type definitions and type-related functions                         */
/******************************************************************************/

// See admire_types.hpp


/******************************************************************************/
/* Public prototypes                                                          */
/******************************************************************************/

namespace admire {

void
ping(const server& srv);

admire::job
register_job(const server& srv, const job_requirements& reqs);

ADM_return_t
update_job(const server& srv, const job&, const job_requirements& reqs);

ADM_return_t
remove_job(const server& srv, const job& job);

ADM_return_t
register_adhoc_storage(const server& srv, ADM_job_t job,
                       ADM_adhoc_context_t ctx, ADM_storage_t* adhoc_storage);

ADM_return_t
update_adhoc_storage(const server& srv, ADM_job_t job, ADM_adhoc_context_t ctx,
                     ADM_storage_t adhoc_storage);

ADM_return_t
remove_adhoc_storage(const server& srv, ADM_job_t job,
                     ADM_storage_t adhoc_storage);

ADM_return_t
deploy_adhoc_storage(const server& srv, ADM_job_t job,
                     ADM_storage_t adhoc_storage);

ADM_return_t
register_pfs_storage(const server& srv, ADM_job_t job, ADM_pfs_context_t ctx,
                     ADM_storage_t* pfs_storage);

ADM_return_t
update_pfs_storage(const server& srv, ADM_job_t job, ADM_pfs_context_t ctx,
                   ADM_storage_t pfs_storage);

ADM_return_t
remove_pfs_storage(const server& srv, ADM_job_t job, ADM_storage_t pfs_storage);

ADM_return_t
transfer_dataset(const server& srv, ADM_job_t job, ADM_dataset_t** sources,
                 ADM_dataset_t** targets, ADM_qos_limit_t** limits,
                 ADM_transfer_mapping_t mapping, ADM_transfer_t* transfer);

ADM_return_t
set_dataset_information(const server& srv, ADM_job_t job, ADM_dataset_t target,
                        ADM_dataset_info_t info);

ADM_return_t
set_io_resources(const server& srv, ADM_job_t job, ADM_storage_t tier,
                 ADM_storage_resources_t resources);

ADM_return_t
get_transfer_priority(const server& srv, ADM_job_t job, ADM_transfer_t transfer,
                      ADM_transfer_priority_t* priority);

ADM_return_t
set_transfer_priority(const server& srv, ADM_job_t job, ADM_transfer_t transfer,
                      int incr);

ADM_return_t
cancel_transfer(const server& srv, ADM_job_t job, ADM_transfer_t transfer);

ADM_return_t
get_pending_transfers(const server& srv, ADM_job_t job,
                      ADM_transfer_t** pending_transfers);

ADM_return_t
set_qos_constraints(const server& srv, ADM_job_t job, ADM_qos_entity_t entity,
                    ADM_qos_limit_t limit);

ADM_return_t
get_qos_constraints(const server& srv, ADM_job_t job, ADM_qos_entity_t entity,
                    ADM_qos_limit_t** limits);

ADM_return_t
define_data_operation(const server& srv, ADM_job_t job, const char* path,
                      ADM_data_operation_t* op, va_list args);

ADM_return_t
connect_data_operation(const server& srv, ADM_job_t job, ADM_dataset_t input,
                       ADM_dataset_t output, bool should_stream, va_list args);

ADM_return_t
finalize_data_operation(const server& srv, ADM_job_t job,
                        ADM_data_operation_t op,
                        ADM_data_operation_status_t* status);

ADM_return_t
link_transfer_to_data_operation(const server& srv, ADM_job_t job,
                                ADM_data_operation_t op, bool should_stream,
                                va_list args);

ADM_return_t
get_statistics(const server& srv, ADM_job_t job, ADM_job_stats_t** stats);

} // namespace admire

#endif // SCORD_ADMIRE_HPP
