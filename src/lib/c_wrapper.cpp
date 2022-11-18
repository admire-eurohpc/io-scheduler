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
#include <admire.hpp>
#include <logger/logger.hpp>
#include <net/proto/rpc_types.h>
#include <stdarg.h>
#include <admire_types.hpp>
#include <admire_types.h>
#include <api/convert.hpp>
#include "detail/impl.hpp"


/******************************************************************************/
/* C API implementation                                                       */
/******************************************************************************/
ADM_return_t
ADM_ping(ADM_server_t server) {
    const admire::server srv{server};
    return admire::detail::ping(srv);
}

ADM_return_t
ADM_register_job(ADM_server_t server, ADM_job_resources_t res,
                 ADM_job_requirements_t reqs, uint64_t slurm_id,
                 ADM_job_t* job) {

    const admire::server srv{server};

    const auto rv = admire::detail::register_job(
            srv, admire::job::resources{res}, admire::job_requirements{reqs},
            slurm_id);

    if(!rv) {
        return rv.error();
    }

    *job = admire::api::convert(*rv).release();

    return ADM_SUCCESS;
}

ADM_return_t
ADM_update_job(ADM_server_t server, ADM_job_t job,
               ADM_job_resources_t job_resources) {

    const admire::server srv{server};

    return admire::detail::update_job(srv, admire::job{job},
                                      admire::job::resources{job_resources});
}

ADM_return_t
ADM_remove_job(ADM_server_t server, ADM_job_t job) {

    const admire::server srv{server};

    return admire::detail::remove_job(srv, admire::job{job});
}

ADM_return_t
ADM_register_adhoc_storage(ADM_server_t server, const char* name,
                           ADM_adhoc_storage_type_t type,
                           ADM_adhoc_context_t ctx,
                           ADM_adhoc_storage_t* adhoc_storage) {

    const admire::server srv{server};

    const auto cxx_type = static_cast<enum admire::adhoc_storage::type>(type);
    const auto rv = admire::detail::register_adhoc_storage(
            srv, name, cxx_type, admire::adhoc_storage::ctx{ctx});

    if(!rv) {
        return rv.error();
    }

    *adhoc_storage = admire::api::convert(*rv).release();

    return ADM_SUCCESS;
}

ADM_return_t
ADM_update_adhoc_storage(ADM_server_t server, ADM_adhoc_storage_t adhoc_storage,
                         ADM_adhoc_context_t ctx) {

    const admire::server srv{server};

    return admire::detail::update_adhoc_storage(
            srv, admire::adhoc_storage::ctx{ctx},
            admire::adhoc_storage{adhoc_storage});
}

ADM_return_t
ADM_remove_adhoc_storage(ADM_server_t server,
                         ADM_adhoc_storage_t adhoc_storage) {

    const admire::server srv{server};

    return admire::detail::remove_adhoc_storage(
            srv, admire::adhoc_storage{adhoc_storage});
}

ADM_return_t
ADM_deploy_adhoc_storage(ADM_server_t server,
                         ADM_adhoc_storage_t adhoc_storage) {

    const admire::server srv{server};

    return admire::detail::deploy_adhoc_storage(
            srv, admire::adhoc_storage{adhoc_storage});
}

ADM_return_t
ADM_register_pfs_storage(ADM_server_t server, const char* name, 
                         ADM_pfs_storage_type_t type, ADM_pfs_context_t ctx,
                         ADM_pfs_storage_t* pfs_storage) {

    const auto rv = admire::detail::register_pfs_storage(
            admire::server{server}, name,
            static_cast<enum admire::pfs_storage::type>(type),
            admire::pfs_storage::ctx{ctx});

    if(!rv) {
        return rv.error();
    }

    *pfs_storage = admire::api::convert(rv.value()).release();

    return ADM_SUCCESS;
}

ADM_return_t
ADM_update_pfs_storage(ADM_server_t server, ADM_pfs_context_t ctx,
                       ADM_pfs_storage_t pfs_storage) {

    const admire::server srv{server};

    return admire::update_pfs_storage(srv, ctx, pfs_storage);
}

ADM_return_t
ADM_remove_pfs_storage(ADM_server_t server, ADM_pfs_storage_t pfs_storage) {

    const admire::server srv{server};

    return admire::remove_pfs_storage(srv, pfs_storage);
}

ADM_return_t
ADM_transfer_datasets(ADM_server_t server, ADM_job_t job,
                      ADM_dataset_t sources[], size_t sources_len,
                      ADM_dataset_t targets[], size_t targets_len,
                      ADM_qos_limit_t limits[], size_t limits_len,
                      ADM_transfer_mapping_t mapping,
                      ADM_transfer_t* transfer) {

    const auto rv = admire::detail::transfer_datasets(
            admire::server{server}, admire::job{job},
            admire::api::convert(sources, sources_len),
            admire::api::convert(targets, targets_len),
            admire::api::convert(limits, limits_len),
            static_cast<admire::transfer::mapping>(mapping));

    if(!rv) {
        return rv.error();
    }

    *transfer = admire::api::convert(*rv).release();

    return ADM_SUCCESS;
}

ADM_return_t
ADM_set_dataset_information(ADM_server_t server, ADM_job_t job,
                            ADM_dataset_t target, ADM_dataset_info_t info) {

    const admire::server srv{server};

    return admire::set_dataset_information(srv, job, target, info);
}

ADM_return_t
ADM_set_io_resources(ADM_server_t server, ADM_job_t job,
                     ADM_adhoc_storage_t tier,
                     ADM_adhoc_resources_t resources) {

    const admire::server srv{server};

    return admire::set_io_resources(srv, job, tier, resources);
}

ADM_return_t
ADM_get_transfer_priority(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t transfer,
                          ADM_transfer_priority_t* priority) {

    const admire::server srv{server};

    return admire::get_transfer_priority(srv, job, transfer, priority);
}

ADM_return_t
ADM_set_transfer_priority(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t transfer, int incr) {

    const admire::server srv{server};

    return admire::set_transfer_priority(srv, job, transfer, incr);
}

ADM_return_t
ADM_cancel_transfer(ADM_server_t server, ADM_job_t job,
                    ADM_transfer_t transfer) {

    const admire::server srv{server};

    return admire::cancel_transfer(srv, job, transfer);
}

ADM_return_t
ADM_get_pending_transfers(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t** pending_transfers) {

    const admire::server srv{server};

    return admire::get_pending_transfers(srv, job, pending_transfers);
}

ADM_return_t
ADM_set_qos_constraints(ADM_server_t server, ADM_job_t job,
                        ADM_qos_entity_t entity, ADM_qos_limit_t limit) {

    const admire::server srv{server};

    return admire::set_qos_constraints(srv, job, entity, limit);
}

ADM_return_t
ADM_get_qos_constraints(ADM_server_t server, ADM_job_t job,
                        ADM_qos_entity_t entity, ADM_qos_limit_t** limits) {

    const admire::server srv{server};

    return admire::get_qos_constraints(srv, job, entity, limits);
}

ADM_return_t
ADM_define_data_operation(ADM_server_t server, ADM_job_t job, const char* path,
                          ADM_data_operation_t* op, ...) {

    const admire::server srv{server};

    va_list args;
    va_start(args, op);
    auto ret = admire::define_data_operation(srv, job, path, op, args);
    va_end(args);

    return ret;
}

ADM_return_t
ADM_connect_data_operation(ADM_server_t server, ADM_job_t job,
                           ADM_dataset_t input, ADM_dataset_t output,
                           bool should_stream, ...) {

    const admire::server srv{server};

    va_list args;
    va_start(args, should_stream);
    auto ret = admire::connect_data_operation(srv, job, input, output,
                                              should_stream, args);
    va_end(args);

    return ret;
}

ADM_return_t
ADM_finalize_data_operation(ADM_server_t server, ADM_job_t job,
                            ADM_data_operation_t op,
                            ADM_data_operation_status_t* status) {

    const admire::server srv{server};

    return admire::finalize_data_operation(srv, job, op, status);
}


ADM_return_t
ADM_link_transfer_to_data_operation(ADM_server_t server, ADM_job_t job,
                                    ADM_data_operation_t op,
                                    ADM_transfer_t transfer, bool should_stream,
                                    ...) {

    const admire::server srv{server};

    va_list args;
    va_start(args, should_stream);
    auto ret = admire::link_transfer_to_data_operation(srv, job, op, transfer,
                                                       should_stream, args);
    va_end(args);

    return ret;
}

ADM_return_t
ADM_get_statistics(ADM_server_t server, ADM_job_t job,
                   ADM_job_stats_t** stats) {
    const admire::server srv{server};
    return admire::get_statistics(srv, job, stats);
}
