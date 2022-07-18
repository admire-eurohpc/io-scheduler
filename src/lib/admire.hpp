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
#include <tl/expected.hpp>
#include <optional>
#include <cstdarg>
#include <string>
#include <utility>
#include "network/proto/rpc_types.h"

#ifndef SCORD_ADMIRE_HPP
#define SCORD_ADMIRE_HPP

namespace admire {

using job_id = int64_t;

struct server {
    std::string m_protocol;
    std::string m_address;
};

struct job {
    job_id m_id;
};

struct dataset {
    explicit dataset(std::string id) : m_id(std::move(id)) {}
    std::string m_id;
};

namespace storage::adhoc {

enum class execution_mode : std::underlying_type<ADM_adhoc_mode_t>::type {
    in_job_shared = ADM_ADHOC_MODE_IN_JOB_SHARED,
    in_job_dedicated = ADM_ADHOC_MODE_IN_JOB_DEDICATED,
    separate_new = ADM_ADHOC_MODE_SEPARATE_NEW,
    separate_existing = ADM_ADHOC_MODE_SEPARATE_EXISTING
};

enum class access_mode : std::underlying_type<ADM_adhoc_mode_t>::type {
    read_only = ADM_ADHOC_ACCESS_RDONLY,
    write_only = ADM_ADHOC_ACCESS_WRONLY,
    read_write = ADM_ADHOC_ACCESS_RDWR,
};

struct context {
    execution_mode m_exec_mode;
    access_mode m_access_mode;
    std::uint32_t m_nodes;
    std::uint32_t m_walltime;
    bool m_should_flush;
};

} // namespace storage::adhoc

struct job_requirements {

    job_requirements(std::vector<admire::dataset> inputs,
                     std::vector<admire::dataset> outputs);

    job_requirements(std::vector<admire::dataset> inputs,
                     std::vector<admire::dataset> outputs,
                     std::optional<storage::adhoc::context> adhoc_context);

    explicit job_requirements(ADM_job_requirements_t reqs);

    ADM_job_requirements_t
    to_rpc_type() const;

    std::vector<admire::dataset> m_inputs;
    std::vector<admire::dataset> m_outputs;
    std::optional<storage::adhoc::context> m_adhoc_context;
};


void
ping(const server& srv);

admire::job
register_job(const server& srv, const job_requirements& reqs);

ADM_return_t
update_job(const server& srv, ADM_job_t job, const job_requirements& reqs);

ADM_return_t
remove_job(const server& srv, ADM_job_t job);

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
