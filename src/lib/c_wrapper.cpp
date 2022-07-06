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
#include "detail/impl.hpp"


/******************************************************************************/
/* Type definitions and related functions                                     */
/******************************************************************************/

struct adm_server {
    const char* s_protocol;
    const char* s_address;
};

struct adm_node {
    const char* n_hostname;
};

struct adm_dataset {
    const char* d_id;
};

struct adm_job {
    uint64_t j_id;
};

struct adm_qos_entity {
    ADM_qos_scope_t e_scope;
    union {
        ADM_node_t e_node;
        ADM_job_t e_job;
        ADM_dataset_t e_dataset;
        ADM_transfer_t e_transfer;
    };
};

struct adm_qos_limit {
    ADM_qos_entity_t l_entity;
    ADM_qos_class_t l_class;
    uint64_t l_value;
};

struct adm_transfer {
    // TODO: empty for now
};

/** The I/O requirements for a job */
struct adm_job_requirements {
    /** An array of input datasets */
    ADM_dataset_t* r_inputs;
    /** The number of datasets in r_inputs */
    size_t r_num_inputs;
    /** A list of output datasets */
    ADM_dataset_t* r_outputs;
    /** The number of datasets in r_outputs */
    size_t r_num_outputs;
    /** An optional definition for a specific adhoc storage instance */
    ADM_adhoc_storage_handle_t r_adhoc_storage;
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
        return ADM_EBADARGS;
    }

    free(server);
    return ret;
}


ADM_node_t
ADM_node_create(const char* hostname) {

    struct adm_node* adm_node =
            (struct adm_node*) malloc(sizeof(struct adm_node));

    if(!adm_node) {
        LOGGER_ERROR("Could not allocate ADM_node_t")
        return NULL;
    }

    adm_node->n_hostname = hostname;

    return adm_node;
}

ADM_return_t
ADM_node_destroy(ADM_node_t node) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!node) {
        LOGGER_ERROR("Invalid ADM_node_t")
        return ADM_EBADARGS;
    }

    free(node);
    return ret;
}


ADM_dataset_t
ADM_dataset_create(const char* id) {

    struct adm_dataset* adm_dataset =
            (struct adm_dataset*) malloc(sizeof(struct adm_dataset));

    if(!adm_dataset) {
        LOGGER_ERROR("Could not allocate ADM_dataset_t")
        return NULL;
    }

    adm_dataset->d_id = id;

    return adm_dataset;
}

ADM_return_t
ADM_dataset_destroy(ADM_dataset_t dataset) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!dataset) {
        LOGGER_ERROR("Invalid ADM_dataset_t")
        return ADM_EBADARGS;
    }

    free(dataset);
    return ret;
}

ADM_qos_entity_t
ADM_qos_entity_create(ADM_qos_scope_t scope, ...) {

    struct adm_qos_entity* adm_qos_entity =
            (struct adm_qos_entity*) malloc(sizeof(struct adm_qos_entity));

    if(!adm_qos_entity) {
        LOGGER_ERROR("Could not allocate ADM_qos_entity_t")
        return NULL;
    }

    adm_qos_entity->e_scope = scope;

    va_list ap;
    va_start(ap, scope);

    switch(scope) {
        case ADM_QOS_SCOPE_NODE:
            adm_qos_entity->e_node = va_arg(ap, ADM_node_t);
            break;
        case ADM_QOS_SCOPE_JOB:
            adm_qos_entity->e_job = va_arg(ap, ADM_job_t);
            break;
        case ADM_QOS_SCOPE_DATASET:
            adm_qos_entity->e_dataset = va_arg(ap, ADM_dataset_t);
            break;
    }
    va_end(ap);

    return adm_qos_entity;
}

ADM_return_t
ADM_qos_entity_destroy(ADM_qos_entity_t entity) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!entity) {
        LOGGER_ERROR("Invalid ADM_qos_entity_t")
        return ADM_EBADARGS;
    }

    free(entity);
    return ret;
}

ADM_qos_limit_t
ADM_qos_limit_create(ADM_qos_entity_t entity, ADM_qos_class_t cls,
                     uint64_t value) {

    struct adm_qos_limit* adm_qos_limit =
            (struct adm_qos_limit*) malloc(sizeof(struct adm_qos_limit));

    if(!adm_qos_limit) {
        LOGGER_ERROR("Could not allocate ADM_qos_limit_t")
        return NULL;
    }

    adm_qos_limit->l_entity = entity;
    adm_qos_limit->l_class = cls;
    adm_qos_limit->l_value = value;

    return adm_qos_limit;
}

ADM_return_t
ADM_qos_limit_destroy(ADM_qos_limit_t limit) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!limit) {
        LOGGER_ERROR("Invalid ADM_qos_limit_t")
        return ADM_EBADARGS;
    }

    free(limit);
    return ret;
}

ADM_job_requirements_t
ADM_job_requirements_create(ADM_dataset_t inputs[], size_t inputs_len,
                            ADM_dataset_t outputs[], size_t outputs_len,
                            ADM_adhoc_storage_handle_t adhoc_storage) {

    struct adm_job_requirements* adm_job_reqs =
            (struct adm_job_requirements*) malloc(
                    sizeof(struct adm_job_requirements));

    if(!adm_job_reqs) {
        LOGGER_ERROR("Could not allocate ADM_job_requirements_t")
        return NULL;
    }

    adm_job_reqs->r_inputs = inputs;
    adm_job_reqs->r_num_inputs = inputs_len;
    adm_job_reqs->r_outputs = outputs;
    adm_job_reqs->r_num_outputs = outputs_len;
    adm_job_reqs->r_adhoc_storage = adhoc_storage;

    return adm_job_reqs;
}

ADM_return_t
ADM_job_requirements_destroy(ADM_job_requirements_t reqs) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!reqs) {
        LOGGER_ERROR("Invalid ADM_job_requirements_t")
        return ADM_EBADARGS;
    }

    free(reqs);
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


/******************************************************************************/
/* C API implementation                                                       */
/******************************************************************************/
ADM_return_t
ADM_ping(ADM_server_t server) {
    const admire::server srv{server->s_protocol, server->s_address};
    return admire::detail::ping(srv);
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
        return ADM_EOTHER;
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
                     ADM_dataset_t** sources, ADM_dataset_t** targets,
                     ADM_qos_limit_t** limits, ADM_transfer_mapping_t mapping,
                     ADM_transfer_t* transfer) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::transfer_dataset(srv, job, sources, targets, limits, mapping,
                                    transfer);
}

ADM_return_t
ADM_set_dataset_information(ADM_server_t server, ADM_job_t job,
                            ADM_dataset_t target, ADM_dataset_info_t info) {

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
                          ADM_transfer_t transfer,
                          ADM_transfer_priority_t* priority) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::get_transfer_priority(srv, job, transfer, priority);
}

ADM_return_t
ADM_set_transfer_priority(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t transfer, int incr) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::set_transfer_priority(srv, job, transfer, incr);
}

ADM_return_t
ADM_cancel_transfer(ADM_server_t server, ADM_job_t job,
                    ADM_transfer_t transfer) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::cancel_transfer(srv, job, transfer);
}

ADM_return_t
ADM_get_pending_transfers(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t** pending_transfers) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::get_pending_transfers(srv, job, pending_transfers);
}

ADM_return_t
ADM_set_qos_constraints(ADM_server_t server, ADM_job_t job,
                        ADM_qos_entity_t entity, ADM_qos_limit_t limit) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::set_qos_constraints(srv, job, entity, limit);
}

ADM_return_t
ADM_get_qos_constraints(ADM_server_t server, ADM_job_t job,
                        ADM_qos_entity_t entity, ADM_qos_limit_t** limits) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::get_qos_constraints(srv, job, entity, limits);
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
                           ADM_dataset_t input, ADM_dataset_t output,
                           bool should_stream, ...) {

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
