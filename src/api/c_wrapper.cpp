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
#include <logger.hpp>
#include "detail/impl.hpp"

struct adm_server {
    const char* s_protocol;
    const char* s_address;
};

struct adm_job {
    uint64_t j_id;
};

ADM_server_t
ADM_server_create(const char* protocol, const char* address) {

    struct adm_server* adm_server =
            (struct adm_server*) malloc(sizeof(struct adm_server));

    if(!adm_server) {
        LOGGER_ERROR("Could not allocate ADM_server_t")
        return NULL;
    }

    adm_server->s_protocol = protocol;
    adm_server->s_address = address;

    return adm_server;
}

ADM_return_t
ADM_server_destroy(ADM_server_t server) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!server) {
        LOGGER_ERROR("Invalid ADM_server_t")
        return ADM_EINVAL;
    }

    free(server);
    return ret;
}

/**
 * Initialize a job handle that can be used by clients to refer to a job.
 *
 * @remark This function is not actually part of the public API, but it is
 * useful to have for internal purposes
 *
 * @param [in] id The identifier for this job
 * @return A valid JOB HANDLE or NULL in case of failure.
 */
static ADM_job_t
ADM_job_create(uint64_t id) {

    struct adm_job* adm_job = (struct adm_job*) malloc(sizeof(struct adm_job));

    if(!adm_job) {
        LOGGER_ERROR("Could not allocate ADM_job_t")
        return NULL;
    }

    adm_job->j_id = id;

    return adm_job;
}

ADM_return_t
ADM_register_job(ADM_server_t server, ADM_job_requirements_t reqs,
                 ADM_job_t* job) {

    const admire::server srv{server->s_protocol, server->s_address};

    const auto rv = admire::detail::register_job(srv, reqs);

    if(!rv) {
        return rv.error();
    }

    const auto jh = ADM_job_create(rv->m_id);

    if(!jh) {
        return ADM_OTHER_ERROR;
    }

    *job = jh;

    return ADM_SUCCESS;
}

ADM_return_t
ADM_update_job(ADM_server_t server, ADM_job_t job,
               ADM_job_requirements_t reqs) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::update_job(srv, job, reqs);
}

ADM_return_t
ADM_remove_job(ADM_server_t server, ADM_job_t job) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::remove_job(srv, job);
}

ADM_return_t
ADM_register_adhoc_storage(ADM_server_t server, ADM_job_t job,
                           ADM_adhoc_context_t ctx,
                           ADM_adhoc_storage_handle_t* adhoc_handle) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::register_adhoc_storage(srv, job, ctx, adhoc_handle);
}

ADM_return_t
ADM_update_adhoc_storage(ADM_server_t server, ADM_job_t job,
                         ADM_adhoc_context_t ctx,
                         ADM_adhoc_storage_handle_t adhoc_handle) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::update_adhoc_storage(srv, job, ctx, adhoc_handle);
}

ADM_return_t
ADM_remove_adhoc_storage(ADM_server_t server, ADM_job_t job,
                         ADM_adhoc_storage_handle_t adhoc_handle) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::remove_adhoc_storage(srv, job, adhoc_handle);
}

ADM_return_t
ADM_deploy_adhoc_storage(ADM_server_t server, ADM_job_t job,
                         ADM_adhoc_storage_handle_t adhoc_handle) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::deploy_adhoc_storage(srv, job, adhoc_handle);
}

ADM_return_t
ADM_transfer_dataset(ADM_server_t server, ADM_job_t job,
                     ADM_dataset_handle_t** sources,
                     ADM_dataset_handle_t** targets, ADM_limit_t** limits,
                     ADM_tx_mapping_t mapping,
                     ADM_transfer_handle_t* tx_handle) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::transfer_dataset(srv, job, sources, targets, limits, mapping,
                                    tx_handle);
}

ADM_return_t
ADM_set_dataset_information(ADM_server_t server, ADM_job_t job,
                            ADM_dataset_handle_t target,
                            ADM_dataset_info_t info) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::set_dataset_information(srv, job, target, info);
}

ADM_return_t
ADM_set_io_resources(ADM_server_t server, ADM_job_t job,
                     ADM_storage_handle_t tier,
                     ADM_storage_resources_t resources) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::set_io_resources(srv, job, tier, resources);
}

ADM_return_t
ADM_get_transfer_priority(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_handle_t tx_handle,
                          ADM_transfer_priority_t* priority) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::get_transfer_priority(srv, job, tx_handle, priority);
}

ADM_return_t
ADM_set_transfer_priority(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_handle_t tx_handle, int incr) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::set_transfer_priority(srv, job, tx_handle, incr);
}

ADM_return_t
ADM_cancel_transfer(ADM_server_t server, ADM_job_t job,
                    ADM_transfer_handle_t tx_handle) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::cancel_transfer(srv, job, tx_handle);
}

ADM_return_t
ADM_get_pending_transfers(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_handle_t** pending_transfers) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::get_pending_transfers(srv, job, pending_transfers);
}

ADM_return_t
ADM_set_qos_constraints(ADM_server_t server, ADM_job_t job, ADM_limit_t limit) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::set_qos_constraints(srv, job, limit);
}

ADM_return_t
ADM_get_qos_constraints(ADM_server_t server, ADM_job_t job,
                        ADM_qos_scope_t scope, ADM_qos_entity_t entity,
                        ADM_limit_t** limits) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::get_qos_constraints(srv, job, scope, entity, limits);
}

ADM_return_t
ADM_define_data_operation(ADM_server_t server, ADM_job_t job, const char* path,
                          ADM_data_operation_handle_t* op, ...) {

    const admire::server srv{server->s_protocol, server->s_address};

    va_list args;
    va_start(args, op);
    auto ret = admire::define_data_operation(srv, job, path, op, args);
    va_end(args);

    return ret;
}

ADM_return_t
ADM_connect_data_operation(ADM_server_t server, ADM_job_t job,
                           ADM_dataset_handle_t input,
                           ADM_dataset_handle_t output, bool should_stream,
                           ...) {

    const admire::server srv{server->s_protocol, server->s_address};

    va_list args;
    va_start(args, should_stream);
    auto ret = admire::connect_data_operation(srv, job, input, output,
                                              should_stream, args);
    va_end(args);

    return ret;
}

ADM_return_t
ADM_finalize_data_operation(ADM_server_t server, ADM_job_t job,
                            ADM_data_operation_handle_t op,
                            ADM_data_operation_status_t* status) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::finalize_data_operation(srv, job, op, status);
}

ADM_return_t
ADM_link_transfer_to_data_operation(ADM_server_t server, ADM_job_t job,
                                    ADM_data_operation_handle_t op,
                                    bool should_stream, ...) {

    const admire::server srv{server->s_protocol, server->s_address};

    va_list args;
    va_start(args, should_stream);
    auto ret = admire::link_transfer_to_data_operation(srv, job, op,
                                                       should_stream, args);
    va_end(args);

    return ret;
}

ADM_return_t
ADM_get_statistics(ADM_server_t server, ADM_job_t job,
                   ADM_job_stats_t** stats) {
    const admire::server srv{server->s_protocol, server->s_address};
    return admire::get_statistics(srv, job, stats);
}
