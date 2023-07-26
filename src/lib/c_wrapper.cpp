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

#include <scord/scord.h>
#include <scord/scord.hpp>
#include <logger/logger.hpp>
#include <stdarg.h>
#include <scord/types.hpp>
#include <scord/types.h>
#include "detail/impl.hpp"

namespace {

std::vector<scord::dataset>
convert(ADM_dataset_t datasets[], size_t datasets_len) {

    std::vector<scord::dataset> rv;
    rv.reserve(datasets_len);

    for(size_t i = 0; i < datasets_len; ++i) {
        rv.emplace_back(datasets[i]);
    }

    return rv;
}

std::vector<scord::qos::limit>
convert(ADM_qos_limit_t limits[], size_t limits_len) {

    std::vector<scord::qos::limit> rv;
    rv.reserve(limits_len);

    for(size_t i = 0; i < limits_len; ++i) {
        rv.emplace_back(limits[i]);
    }

    return rv;
}

} // namespace


/******************************************************************************/
/* C API implementation                                                       */
/******************************************************************************/
ADM_return_t
ADM_ping(ADM_server_t server) {
    const scord::server srv{server};
    return scord::detail::ping(srv);
}

ADM_return_t
ADM_register_job(ADM_server_t server, ADM_job_resources_t res,
                 ADM_job_requirements_t reqs, uint64_t slurm_id,
                 ADM_job_t* job) {

    const scord::server srv{server};

    const auto rv = scord::detail::register_job(srv, scord::job::resources{res},
                                                scord::job::requirements{reqs},
                                                slurm_id);

    if(!rv) {
        return rv.error();
    }

    *job = static_cast<ADM_job_t>(rv.value());

    return ADM_SUCCESS;
}

ADM_return_t
ADM_update_job(ADM_server_t server, ADM_job_t job,
               ADM_job_resources_t job_resources) {

    const scord::server srv{server};

    return scord::detail::update_job(srv, scord::job{job},
                                     scord::job::resources{job_resources});
}

ADM_return_t
ADM_remove_job(ADM_server_t server, ADM_job_t job) {

    const scord::server srv{server};

    return scord::detail::remove_job(srv, scord::job{job});
}

ADM_return_t
ADM_register_adhoc_storage(ADM_server_t server, const char* name,
                           ADM_adhoc_storage_type_t type,
                           ADM_adhoc_context_t ctx,
                           ADM_adhoc_resources_t resources,
                           ADM_adhoc_storage_t* adhoc_storage) {

    const auto rv = scord::detail::register_adhoc_storage(
            scord::server{server}, name,
            static_cast<enum scord::adhoc_storage::type>(type),
            scord::adhoc_storage::ctx{ctx},
            scord::adhoc_storage::resources{resources});

    if(!rv) {
        return rv.error();
    }

    *adhoc_storage = static_cast<ADM_adhoc_storage_t>(rv.value());

    return ADM_SUCCESS;
}

ADM_return_t
ADM_update_adhoc_storage(ADM_server_t server, ADM_adhoc_storage_t adhoc_storage,
                         ADM_adhoc_resources_t new_resources) {

    const scord::server srv{server};

    return scord::detail::update_adhoc_storage(
            srv, scord::adhoc_storage{adhoc_storage},
            scord::adhoc_storage::resources{new_resources});
}

ADM_return_t
ADM_remove_adhoc_storage(ADM_server_t server,
                         ADM_adhoc_storage_t adhoc_storage) {

    const scord::server srv{server};

    return scord::detail::remove_adhoc_storage(
            srv, scord::adhoc_storage{adhoc_storage});
}

ADM_return_t
ADM_deploy_adhoc_storage(ADM_server_t server, ADM_adhoc_storage_t adhoc_storage,
                         char** adhoc_storage_path) {

    const scord::server srv{server};

    const auto rv = scord::detail::deploy_adhoc_storage(
            srv, scord::adhoc_storage{adhoc_storage});

    if(!rv) {
        *adhoc_storage_path = nullptr;
        return rv.error();
    }

    const auto s = rv.value().string();
    char* buf = static_cast<char*>(std::malloc(s.size() + 1));

    if(!buf) {
        *adhoc_storage_path = nullptr;
        return ADM_ENOMEM;
    }

    s.copy(buf, s.size());
    buf[s.size()] = '\0';
    *adhoc_storage_path = buf;

    return ADM_SUCCESS;
}

ADM_return_t
ADM_terminate_adhoc_storage(ADM_server_t server,
                            ADM_adhoc_storage_t adhoc_storage) {

    return scord::detail::terminate_adhoc_storage(
            scord::server{server}, scord::adhoc_storage{adhoc_storage});
}

ADM_return_t
ADM_register_pfs_storage(ADM_server_t server, const char* name,
                         ADM_pfs_storage_type_t type, ADM_pfs_context_t ctx,
                         ADM_pfs_storage_t* pfs_storage) {

    const auto rv = scord::detail::register_pfs_storage(
            scord::server{server}, name,
            static_cast<enum scord::pfs_storage::type>(type),
            scord::pfs_storage::ctx{ctx});

    if(!rv) {
        return rv.error();
    }

    *pfs_storage = static_cast<ADM_pfs_storage_t>(rv.value());

    return ADM_SUCCESS;
}

ADM_return_t
ADM_update_pfs_storage(ADM_server_t server, ADM_pfs_storage_t pfs_storage,
                       ADM_pfs_context_t ctx) {

    return scord::detail::update_pfs_storage(scord::server{server},
                                             scord::pfs_storage{pfs_storage},
                                             scord::pfs_storage::ctx{ctx});
}

ADM_return_t
ADM_remove_pfs_storage(ADM_server_t server, ADM_pfs_storage_t pfs_storage) {
    return scord::detail::remove_pfs_storage(scord::server{server},
                                             scord::pfs_storage{pfs_storage});
}

ADM_return_t
ADM_transfer_datasets(ADM_server_t server, ADM_job_t job,
                      ADM_dataset_t sources[], size_t sources_len,
                      ADM_dataset_t targets[], size_t targets_len,
                      ADM_qos_limit_t limits[], size_t limits_len,
                      ADM_transfer_mapping_t mapping,
                      ADM_transfer_t* transfer) {

    const auto rv = scord::detail::transfer_datasets(
            scord::server{server}, scord::job{job},
            ::convert(sources, sources_len), ::convert(targets, targets_len),
            ::convert(limits, limits_len),
            static_cast<scord::transfer::mapping>(mapping));

    if(!rv) {
        return rv.error();
    }

    *transfer = static_cast<ADM_transfer_t>(rv.value());

    return ADM_SUCCESS;
}

ADM_return_t
ADM_transfer_update(ADM_server_t server, uint64_t transfer_id,
                    float obtained_bw) {

    return scord::detail::transfer_update(scord::server{server}, transfer_id,
                                          obtained_bw);
}


ADM_return_t
ADM_set_dataset_information(ADM_server_t server, ADM_job_t job,
                            ADM_dataset_t target, ADM_dataset_info_t info) {

    const scord::server srv{server};

    return scord::set_dataset_information(srv, job, target, info);
}

ADM_return_t
ADM_set_io_resources(ADM_server_t server, ADM_job_t job,
                     ADM_adhoc_storage_t tier,
                     ADM_adhoc_resources_t resources) {

    const scord::server srv{server};

    return scord::set_io_resources(srv, job, tier, resources);
}

ADM_return_t
ADM_get_transfer_priority(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t transfer,
                          ADM_transfer_priority_t* priority) {

    const scord::server srv{server};

    return scord::get_transfer_priority(srv, job, transfer, priority);
}

ADM_return_t
ADM_set_transfer_priority(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t transfer, int incr) {

    const scord::server srv{server};

    return scord::set_transfer_priority(srv, job, transfer, incr);
}

ADM_return_t
ADM_cancel_transfer(ADM_server_t server, ADM_job_t job,
                    ADM_transfer_t transfer) {

    const scord::server srv{server};

    return scord::cancel_transfer(srv, job, transfer);
}

ADM_return_t
ADM_get_pending_transfers(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t** pending_transfers) {

    const scord::server srv{server};

    return scord::get_pending_transfers(srv, job, pending_transfers);
}

ADM_return_t
ADM_set_qos_constraints(ADM_server_t server, ADM_job_t job,
                        ADM_qos_entity_t entity, ADM_qos_limit_t limit) {

    const scord::server srv{server};

    return scord::set_qos_constraints(srv, job, entity, limit);
}

ADM_return_t
ADM_get_qos_constraints(ADM_server_t server, ADM_job_t job,
                        ADM_qos_entity_t entity, ADM_qos_limit_t** limits) {

    const scord::server srv{server};

    return scord::get_qos_constraints(srv, job, entity, limits);
}

ADM_return_t
ADM_define_data_operation(ADM_server_t server, ADM_job_t job, const char* path,
                          ADM_data_operation_t* op, ...) {

    const scord::server srv{server};

    va_list args;
    va_start(args, op);
    auto ret = scord::define_data_operation(srv, job, path, op, args);
    va_end(args);

    return ret;
}

ADM_return_t
ADM_connect_data_operation(ADM_server_t server, ADM_job_t job,
                           ADM_dataset_t input, ADM_dataset_t output,
                           int should_stream, ...) {

    const scord::server srv{server};

    va_list args;
    va_start(args, should_stream);
    auto ret = scord::connect_data_operation(srv, job, input, output,
                                             should_stream, args);
    va_end(args);

    return ret;
}

ADM_return_t
ADM_finalize_data_operation(ADM_server_t server, ADM_job_t job,
                            ADM_data_operation_t op,
                            ADM_data_operation_status_t* status) {

    const scord::server srv{server};

    return scord::finalize_data_operation(srv, job, op, status);
}


ADM_return_t
ADM_link_transfer_to_data_operation(ADM_server_t server, ADM_job_t job,
                                    ADM_data_operation_t op,
                                    ADM_transfer_t transfer, int should_stream,
                                    ...) {

    const scord::server srv{server};

    va_list args;
    va_start(args, should_stream);
    auto ret = scord::link_transfer_to_data_operation(srv, job, op, transfer,
                                                      should_stream, args);
    va_end(args);

    return ret;
}

ADM_return_t
ADM_get_statistics(ADM_server_t server, ADM_job_t job,
                   ADM_job_stats_t** stats) {
    const scord::server srv{server};
    return scord::get_statistics(srv, job, stats);
}
