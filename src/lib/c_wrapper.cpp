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
#include <network/proto/rpc_types.h>
#include "detail/impl.hpp"


/******************************************************************************/
/* Type definitions and related functions                                     */
/******************************************************************************/

struct adm_server {
    const char* s_protocol;
    const char* s_address;
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
            (struct adm_dataset*) calloc(1, sizeof(struct adm_dataset));

    if(!adm_dataset) {
        LOGGER_ERROR("Could not allocate ADM_dataset_t")
        return NULL;
    }

    if(id) {
        size_t n = strlen(id);
        adm_dataset->d_id = (const char*) calloc(n + 1, sizeof(char));
        strncpy((char*) adm_dataset->d_id, id, n);
    }

    return adm_dataset;
}

ADM_dataset_t
ADM_dataset_copy(ADM_dataset_t dst, const ADM_dataset_t src) {

    if(!src || !dst) {
        return NULL;
    }

    // copy all primitive types
    *dst = *src;

    // duplicate copy any pointer types
    if(src->d_id) {
        size_t n = strlen(src->d_id);
        dst->d_id = (const char*) calloc(n + 1, sizeof(char));
        strncpy((char*) dst->d_id, src->d_id, n);
    }

    return dst;
}

ADM_return_t
ADM_dataset_destroy(ADM_dataset_t dataset) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!dataset) {
        LOGGER_ERROR("Invalid ADM_dataset_t")
        return ADM_EBADARGS;
    }

    if(dataset->d_id) {
        free((void*) dataset->d_id);
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

ADM_dataset_info_t
ADM_dataset_info_create() {

    struct adm_dataset_info* adm_dataset_info =
            (struct adm_dataset_info*) malloc(sizeof(*adm_dataset_info));

    if(!adm_dataset_info) {
        LOGGER_ERROR("Could not allocate ADM_dataset_info_t");
        return NULL;
    }

    return adm_dataset_info;
}

ADM_return_t
ADM_dataset_info_destroy(ADM_dataset_info_t dataset_info) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!dataset_info) {
        LOGGER_ERROR("Invalid ADM_dataset_info_t")
        return ADM_EBADARGS;
    }

    free(dataset_info);
    return ret;
}

ADM_dataset_list_t
ADM_dataset_list_create(ADM_dataset_t datasets[], size_t length) {

    ADM_dataset_list_t p = (ADM_dataset_list_t) malloc(sizeof(*p));

    if(!p) {
        LOGGER_ERROR("Could not allocate ADM_dataset_list_t")
        return NULL;
    }

    const char* error_msg = NULL;

    p->l_length = length;
    p->l_datasets = (struct adm_dataset*) calloc(length, sizeof(adm_dataset));

    if(!p->l_datasets) {
        error_msg = "Could not allocate ADM_dataset_list_t";
        goto cleanup_on_error;
    }

    for(size_t i = 0; i < length; ++i) {
        if(!ADM_dataset_copy(&p->l_datasets[i], datasets[i])) {
            error_msg = "Could not allocate ADM_dataset_list_t";
            goto cleanup_on_error;
        };

        fprintf(stderr, "o: %s -> %s\n", datasets[i]->d_id,
                p->l_datasets[i].d_id);
    }

    return p;

cleanup_on_error:
    if(p->l_datasets) {
        free(p->l_datasets);
    }
    free(p);

    if(error_msg) {
        LOGGER_ERROR(error_msg);
    }

    return NULL;
}

ADM_return_t
ADM_dataset_list_destroy(ADM_dataset_list_t list) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!list) {
        LOGGER_ERROR("Invalid ADM_pfs_context_t")
        return ADM_EBADARGS;
    }

    // We cannot call ADM_dataset_destroy here because adm_datasets
    // are stored as a consecutive array in memory. Thus, we free
    // the dataset ids themselves and then the array.
    if(list->l_datasets) {
        for(size_t i = 0; i < list->l_length; ++i) {
            free((void*) list->l_datasets[i].d_id);
        }
        free(list->l_datasets);
    }

    free(list);
    return ret;
}

ADM_storage_t
ADM_storage_create(const char* id, ADM_storage_type_t type, void* ctx) {

    struct adm_storage* adm_storage =
            (struct adm_storage*) malloc(sizeof(*adm_storage));

    if(!adm_storage) {
        LOGGER_ERROR("Could not allocate ADM_storage_t");
        return NULL;
    }

    if(!id) {
        LOGGER_ERROR("Null storage id")
        return NULL;
    }

    if(!ctx) {
        LOGGER_ERROR("Null storage context")
        return NULL;
    }

    adm_storage->s_id = id;
    adm_storage->s_type = type;

    switch(adm_storage->s_type) {
        case ADM_STORAGE_GEKKOFS:
        case ADM_STORAGE_DATACLAY:
        case ADM_STORAGE_EXPAND:
        case ADM_STORAGE_HERCULES:
            adm_storage->s_adhoc_ctx = *((ADM_adhoc_context_t*) ctx);
            break;

        case ADM_STORAGE_LUSTRE:
        case ADM_STORAGE_GPFS:
            adm_storage->s_pfs_ctx = *((ADM_pfs_context_t*) ctx);
            break;
    }

    return adm_storage;
}

ADM_return_t
ADM_storage_destroy(ADM_storage_t storage) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!storage) {
        LOGGER_ERROR("Invalid ADM_storage_t")
        return ADM_EBADARGS;
    }

    free(storage);
    return ret;
}

ADM_storage_resources_t
ADM_storage_resources_create() {

    struct adm_storage_resources* adm_storage_resources =
            (struct adm_storage_resources*) malloc(
                    sizeof(*adm_storage_resources));

    if(!adm_storage_resources) {
        LOGGER_ERROR("Could not allocate ADM_storage_resources_t");
        return NULL;
    }

    return adm_storage_resources;
}

ADM_return_t
ADM_storage_resources_destroy(ADM_storage_resources_t res) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!res) {
        LOGGER_ERROR("Invalid ADM_storage_resources_t")
        return ADM_EBADARGS;
    }

    free(res);
    return ret;
}

ADM_data_operation_t
ADM_data_operation_create() {

    struct adm_data_operation* adm_data_operation =
            (struct adm_data_operation*) malloc(sizeof(*adm_data_operation));

    if(!adm_data_operation) {
        LOGGER_ERROR("Could not allocate ADM_data_operation_t");
        return NULL;
    }

    return adm_data_operation;
}

ADM_return_t
ADM_data_operation_destroy(ADM_data_operation_t op) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!op) {
        LOGGER_ERROR("Invalid ADM_data_operation_t")
        return ADM_EBADARGS;
    }

    free(op);
    return ret;
}


ADM_adhoc_context_t
ADM_adhoc_context_create(ADM_adhoc_mode_t exec_mode,
                         ADM_adhoc_access_t access_type, uint32_t nodes,
                         uint32_t walltime, bool should_flush) {

    struct adm_adhoc_context* adm_adhoc_context =
            (struct adm_adhoc_context*) malloc(sizeof(*adm_adhoc_context));

    if(!adm_adhoc_context) {
        LOGGER_ERROR("Could not allocate ADM_adhoc_context_t");
        return NULL;
    }

    adm_adhoc_context->c_mode = exec_mode;
    adm_adhoc_context->c_access = access_type;
    adm_adhoc_context->c_nodes = nodes;
    adm_adhoc_context->c_walltime = walltime;
    adm_adhoc_context->c_should_bg_flush = should_flush;

    return adm_adhoc_context;
}

ADM_return_t
ADM_adhoc_context_destroy(ADM_adhoc_context_t ctx) {


    ADM_return_t ret = ADM_SUCCESS;

    if(!ctx) {
        LOGGER_ERROR("Invalid ADM_adhoc_context_t")
        return ADM_EBADARGS;
    }

    free(ctx);
    return ret;
}

ADM_pfs_context_t
ADM_pfs_context_create(const char* mountpoint) {

    struct adm_pfs_context* adm_pfs_context =
            (struct adm_pfs_context*) malloc(sizeof(*adm_pfs_context));

    if(!adm_pfs_context) {
        LOGGER_ERROR("Could not allocate ADM_adhoc_context_t");
        return NULL;
    }

    adm_pfs_context->c_mount = mountpoint;

    return adm_pfs_context;
}

ADM_return_t
ADM_pfs_context_destroy(ADM_pfs_context_t ctx) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!ctx) {
        LOGGER_ERROR("Invalid ADM_pfs_context_t")
        return ADM_EBADARGS;
    }

    free(ctx);
    return ret;
}

ADM_job_requirements_t
ADM_job_requirements_create(ADM_dataset_t inputs[], size_t inputs_len,
                            ADM_dataset_t outputs[], size_t outputs_len,
                            ADM_storage_t storage) {

    struct adm_job_requirements* adm_job_reqs =
            (struct adm_job_requirements*) malloc(
                    sizeof(struct adm_job_requirements));

    if(!adm_job_reqs) {
        LOGGER_ERROR("Could not allocate ADM_job_requirements_t")
        return NULL;
    }

    ADM_dataset_list_t inputs_list = NULL;
    ADM_dataset_list_t outputs_list = NULL;
    const char* error_msg = NULL;

    inputs_list = ADM_dataset_list_create(inputs, inputs_len);

    if(!inputs_list) {
        error_msg = "Could not allocate ADM_job_requirements_t";
        goto cleanup_on_error;
    }

    outputs_list = ADM_dataset_list_create(outputs, outputs_len);

    if(!outputs_list) {
        error_msg = "Could not allocate ADM_job_requirements_t";
        goto cleanup_on_error;
    }

    adm_job_reqs->r_inputs = inputs_list;
    adm_job_reqs->r_outputs = outputs_list;

    if(!storage) {
        adm_job_reqs->r_adhoc_ctx = NULL;
        return adm_job_reqs;
    }

    if(storage->s_type != ADM_STORAGE_GEKKOFS &&
       storage->s_type != ADM_STORAGE_DATACLAY &&
       storage->s_type != ADM_STORAGE_EXPAND &&
       storage->s_type != ADM_STORAGE_HERCULES) {
        error_msg = "Invalid adhoc_storage argument";
        goto cleanup_on_error;
    }
    adm_job_reqs->r_adhoc_ctx = storage->s_adhoc_ctx;

    return adm_job_reqs;

cleanup_on_error:

    if(outputs_list) {
        ADM_dataset_list_destroy(outputs_list);
    }

    if(inputs_list) {
        ADM_dataset_list_destroy(inputs_list);
    }

    if(adm_job_reqs) {
        ADM_job_requirements_destroy(adm_job_reqs);
    }

    if(error_msg) {
        LOGGER_ERROR(error_msg);
    }

    return NULL;
}

ADM_return_t
ADM_job_requirements_destroy(ADM_job_requirements_t reqs) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!reqs) {
        LOGGER_ERROR("Invalid ADM_job_requirements_t")
        return ADM_EBADARGS;
    }

    if(reqs->r_inputs) {
        ADM_dataset_list_destroy(reqs->r_inputs);
    }

    if(reqs->r_outputs) {
        ADM_dataset_list_destroy(reqs->r_outputs);
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

    const auto rv =
            admire::detail::register_job(srv, admire::job_requirements{reqs});

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

    return admire::update_job(srv, job, admire::job_requirements{reqs});
}

ADM_return_t
ADM_remove_job(ADM_server_t server, ADM_job_t job) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::remove_job(srv, job);
}

ADM_return_t
ADM_register_adhoc_storage(ADM_server_t server, ADM_job_t job,
                           ADM_adhoc_context_t ctx,
                           ADM_storage_t* adhoc_storage) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::register_adhoc_storage(srv, job, ctx, adhoc_storage);
}

ADM_return_t
ADM_update_adhoc_storage(ADM_server_t server, ADM_job_t job,
                         ADM_adhoc_context_t ctx, ADM_storage_t adhoc_storage) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::update_adhoc_storage(srv, job, ctx, adhoc_storage);
}

ADM_return_t
ADM_remove_adhoc_storage(ADM_server_t server, ADM_job_t job,
                         ADM_storage_t adhoc_storage) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::remove_adhoc_storage(srv, job, adhoc_storage);
}

ADM_return_t
ADM_deploy_adhoc_storage(ADM_server_t server, ADM_job_t job,
                         ADM_storage_t adhoc_storage) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::deploy_adhoc_storage(srv, job, adhoc_storage);
}

ADM_return_t
ADM_register_pfs_storage(ADM_server_t server, ADM_job_t job,
                         ADM_pfs_context_t ctx, ADM_storage_t* pfs_storage) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::register_pfs_storage(srv, job, ctx, pfs_storage);
}

ADM_return_t
ADM_update_pfs_storage(ADM_server_t server, ADM_job_t job,
                       ADM_pfs_context_t ctx, ADM_storage_t pfs_storage) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::update_pfs_storage(srv, job, ctx, pfs_storage);
}

ADM_return_t
ADM_remove_pfs_storage(ADM_server_t server, ADM_job_t job,
                       ADM_storage_t pfs_storage) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::remove_pfs_storage(srv, job, pfs_storage);
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
ADM_set_io_resources(ADM_server_t server, ADM_job_t job, ADM_storage_t tier,
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
                          ADM_data_operation_t* op, ...) {

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
                            ADM_data_operation_t op,
                            ADM_data_operation_status_t* status) {

    const admire::server srv{server->s_protocol, server->s_address};

    return admire::finalize_data_operation(srv, job, op, status);
}

ADM_return_t
ADM_link_transfer_to_data_operation(ADM_server_t server, ADM_job_t job,
                                    ADM_data_operation_t op, bool should_stream,
                                    ...) {

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
