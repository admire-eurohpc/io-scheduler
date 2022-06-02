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
#include <cstdarg>

#ifndef SCORD_ADMIRE_HPP
#define SCORD_ADMIRE_HPP

namespace admire {

ADM_return_t
register_job(ADM_server_t server, ADM_job_requirements_t reqs,
             ADM_job_handle_t* job);

ADM_return_t
update_job(ADM_server_t server, ADM_job_handle_t job,
           ADM_job_requirements_t reqs);

ADM_return_t
remove_job(ADM_server_t server, ADM_job_handle_t job);

ADM_return_t
register_adhoc_storage(ADM_server_t server, ADM_job_handle_t job,
                       ADM_adhoc_context_t ctx,
                       ADM_adhoc_storage_handle_t* adhoc_handle);

ADM_return_t
update_adhoc_storage(ADM_server_t server, ADM_job_handle_t job,
                     ADM_adhoc_context_t ctx,
                     ADM_adhoc_storage_handle_t adhoc_handle);

ADM_return_t
remove_adhoc_storage(ADM_server_t server, ADM_job_handle_t job,
                     ADM_adhoc_storage_handle_t adhoc_handle);

ADM_return_t
deploy_adhoc_storage(ADM_server_t server, ADM_job_handle_t job,
                     ADM_adhoc_storage_handle_t adhoc_handle);

ADM_return_t
transfer_dataset(ADM_server_t server, ADM_job_handle_t job,
                 ADM_dataset_handle_t** sources, ADM_dataset_handle_t** targets,
                 ADM_limit_t** limits, ADM_tx_mapping_t mapping,
                 ADM_transfer_handle_t* tx_handle);

ADM_return_t
set_dataset_information(ADM_server_t server, ADM_job_handle_t job,
                        ADM_dataset_handle_t target, ADM_dataset_info_t info);

ADM_return_t
set_io_resources(ADM_server_t server, ADM_job_handle_t job,
                 ADM_storage_handle_t tier, ADM_storage_resources_t resources);

ADM_return_t
get_transfer_priority(ADM_server_t server, ADM_job_handle_t job,
                      ADM_transfer_handle_t tx_handle,
                      ADM_transfer_priority_t* priority);

ADM_return_t
set_transfer_priority(ADM_server_t server, ADM_job_handle_t job,
                      ADM_transfer_handle_t tx_handle, int incr);

ADM_return_t
cancel_transfer(ADM_server_t server, ADM_job_handle_t job,
                ADM_transfer_handle_t tx_handle);

ADM_return_t
get_pending_transfers(ADM_server_t server, ADM_job_handle_t job,
                      ADM_transfer_handle_t** pending_transfers);

ADM_return_t
set_qos_constraints(ADM_server_t server, ADM_job_handle_t job,
                    ADM_limit_t limit);

ADM_return_t
get_qos_constraints(ADM_server_t server, ADM_job_handle_t job,
                    ADM_qos_scope_t scope, ADM_qos_entity_t entity,
                    ADM_limit_t** limits);

ADM_return_t
define_data_operation(ADM_server_t server, ADM_job_handle_t job,
                      const char* path, ADM_data_operation_handle_t* op,
                      va_list args);

ADM_return_t
connect_data_operation(ADM_server_t server, ADM_job_handle_t job,
                       ADM_dataset_handle_t input, ADM_dataset_handle_t output,
                       bool should_stream, va_list args);

ADM_return_t
finalize_data_operation(ADM_server_t server, ADM_job_handle_t job,
                        ADM_data_operation_handle_t op,
                        ADM_data_operation_status_t* status);

ADM_return_t
link_transfer_to_data_operation(ADM_server_t server, ADM_job_handle_t job,
                                ADM_data_operation_handle_t op,
                                bool should_stream, ...);

ADM_return_t
get_statistics(ADM_server_t server, ADM_job_handle_t job,
               ADM_job_stats_t** stats);

} // namespace admire

#endif // SCORD_ADMIRE_HPP
