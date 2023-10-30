/******************************************************************************
 * Copyright 2021-2023, Barcelona Supercomputing Center (BSC), Spain
 *
 * This software was partially supported by the EuroHPC-funded project ADMIRE
 *   (Project ID: 956748, https://www.admire-eurohpc.eu).
 *
 * This file is part of the scord API.
 *
 * The scord API is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The scord API is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with the scord API.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *****************************************************************************/

#include <scord/scord.h>
#include <string>
#include <utility>
#include "scord/types.hpp"

#ifndef SCORD_HPP
#define SCORD_HPP

/******************************************************************************/
/* Public type definitions and type-related functions                         */
/******************************************************************************/

// See scord/types.hpp


/******************************************************************************/
/* Public prototypes                                                          */
/******************************************************************************/

namespace scord {

void
ping(const server& srv);

job_info
query(const server& srv, slurm_job_id job_id);

scord::job
register_job(const server& srv, const job::resources& job_resources,
             const job::requirements& job_requirements,
             scord::slurm_job_id slurm_id);

void
update_job(const server& srv, const job&, const job::resources& job_resources);

void
remove_job(const server& srv, const job& job);

scord::adhoc_storage
register_adhoc_storage(const server& srv, const std::string& name,
                       enum adhoc_storage::type type,
                       const adhoc_storage::ctx& ctx,
                       const adhoc_storage::resources& resources);

void
update_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage,
                     const adhoc_storage::resources& new_resources);

void
remove_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage);

std::string
deploy_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage);

void
terminate_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage);

scord::pfs_storage
register_pfs_storage(const server& srv, const std::string& name,
                     enum scord::pfs_storage::type type,
                     const scord::pfs_storage::ctx& ctx);

void
update_pfs_storage(const server& srv, const pfs_storage& pfs_storage,
                   const scord::pfs_storage::ctx& pfs_storage_ctx);

void
remove_pfs_storage(const server& srv, const pfs_storage& pfs_storage);

scord::transfer
transfer_datasets(const server& srv, const job& job,
                  const std::vector<dataset>& sources,
                  const std::vector<dataset>& targets,
                  const std::vector<qos::limit>& limits,
                  transfer::mapping mapping);

ADM_return_t
set_dataset_information(const server& srv, ADM_job_t job, ADM_dataset_t target,
                        ADM_dataset_info_t info);

ADM_return_t
set_io_resources(const server& srv, ADM_job_t job, ADM_adhoc_storage_t tier,
                 ADM_adhoc_resources_t resources);

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
                                ADM_data_operation_t op,
                                ADM_transfer_t transfer, bool should_stream,
                                va_list args);

ADM_return_t
get_statistics(const server& srv, ADM_job_t job, ADM_job_stats_t** stats);

} // namespace scord

#endif // SCORD_HPP
