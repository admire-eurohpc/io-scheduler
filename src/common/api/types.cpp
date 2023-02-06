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

#include <logger/logger.hpp>
#include <net/proto/rpc_types.h>
#include <net/serialization.hpp>
#include <utility>
#include <utils/ctype_ptr.hpp>
#include <cstdarg>
#include <api/convert.hpp>
#include <variant>
#include <optional>
#include "admire_types.hpp"
#include "internal_types.hpp"

/******************************************************************************/
/* C Type definitions and related functions                                   */
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

    if(hostname) {
        size_t n = strlen(hostname);
        adm_node->n_hostname = (const char*) calloc(n + 1, sizeof(char));
        strcpy((char*) adm_node->n_hostname, hostname);
    }

    return adm_node;
}

ADM_node_t
ADM_node_copy(ADM_node_t dst, const ADM_node_t src) {

    if(!src || !dst) {
        return NULL;
    }

    // copy all primitive types
    *dst = *src;

    // duplicate copy any pointer types
    if(src->n_hostname) {
        size_t n = strlen(src->n_hostname);
        dst->n_hostname = (const char*) calloc(n + 1, sizeof(char));
        strncpy((char*) dst->n_hostname, src->n_hostname, n);
    }

    return dst;
}

ADM_return_t
ADM_node_destroy(ADM_node_t node) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!node) {
        LOGGER_ERROR("Invalid ADM_node_t")
        return ADM_EBADARGS;
    }

    if(node->n_hostname) {
        free((void*) node->n_hostname);
    }

    free(node);
    return ret;
}

ADM_node_list_t
ADM_node_list_create(ADM_node_t nodes[], size_t length) {

    ADM_node_list_t p = (ADM_node_list_t) malloc(sizeof(*p));

    if(!p) {
        LOGGER_ERROR("Could not allocate ADM_node_list_t")
        return NULL;
    }

    const char* error_msg = NULL;

    p->l_length = length;
    p->l_nodes = (struct adm_node*) calloc(length, sizeof(adm_node));

    if(!p->l_nodes) {
        error_msg = "Could not allocate ADM_node_list_t";
        goto cleanup_on_error;
    }

    for(size_t i = 0; i < length; ++i) {

        if(!ADM_node_copy(&p->l_nodes[i], nodes[i])) {
            error_msg = "Could not allocate ADM_node_list_t";
            goto cleanup_on_error;
        };
    }

    return p;

cleanup_on_error:
    if(p->l_nodes) {
        free(p->l_nodes);
    }
    free(p);

    if(error_msg) {
        LOGGER_ERROR(error_msg);
    }

    return NULL;
}

ADM_return_t
ADM_node_list_destroy(ADM_node_list_t list) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!list) {
        LOGGER_ERROR("Invalid ADM_node_list_t")
        return ADM_EBADARGS;
    }

    // We cannot call ADM_node_destroy here because adm_nodes
    // are stored as a consecutive array in memory. Thus, we free
    // the node ids themselves and then the array.
    if(list->l_nodes) {
        for(size_t i = 0; i < list->l_length; ++i) {
            free((void*) list->l_nodes[i].n_hostname);
        }
        free(list->l_nodes);
    }

    free(list);
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
        strcpy((char*) adm_dataset->d_id, id);
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
ADM_qos_entity_create(ADM_qos_scope_t scope, void* data) {

    struct adm_qos_entity* adm_qos_entity =
            (struct adm_qos_entity*) malloc(sizeof(struct adm_qos_entity));

    if(!adm_qos_entity) {
        LOGGER_ERROR("Could not allocate ADM_qos_entity_t")
        return NULL;
    }

    adm_qos_entity->e_scope = scope;

    switch(scope) {
        case ADM_QOS_SCOPE_NODE:
            adm_qos_entity->e_node = (ADM_node_t) data;
            break;
        case ADM_QOS_SCOPE_JOB:
            adm_qos_entity->e_job = (ADM_job_t) data;
            break;
        case ADM_QOS_SCOPE_DATASET:
            adm_qos_entity->e_dataset = (ADM_dataset_t) data;
            break;
        case ADM_QOS_SCOPE_TRANSFER:
            adm_qos_entity->e_transfer = (ADM_transfer_t) data;
    }

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

ADM_return_t
ADM_qos_limit_destroy_all(ADM_qos_limit_t limit) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!limit) {
        LOGGER_ERROR("Invalid ADM_qos_limit_t")
        return ADM_EBADARGS;
    }

    if(limit->l_entity) {
        ADM_qos_entity_destroy(limit->l_entity);
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

ADM_adhoc_storage_t
ADM_adhoc_storage_create(const char* name, ADM_adhoc_storage_type_t type,
                         uint64_t id, ADM_adhoc_context_t adhoc_ctx) {

    struct adm_adhoc_storage* adm_adhoc_storage =
            (struct adm_adhoc_storage*) malloc(sizeof(*adm_adhoc_storage));
    const char* error_msg = NULL;

    if(!adm_adhoc_storage) {
        LOGGER_ERROR("Could not allocate ADM_adhoc_storage_t");
        return NULL;
    }

    if(!name) {
        LOGGER_ERROR("Null storage name")
        return NULL;
    }

    if(!adhoc_ctx) {
        LOGGER_ERROR("Null storage context")
        return NULL;
    }

    adm_adhoc_storage->s_name =
            (const char*) calloc(strlen(name) + 1, sizeof(char));
    strcpy((char*) adm_adhoc_storage->s_name, name);
    adm_adhoc_storage->s_type = type;
    adm_adhoc_storage->s_id = id;

    adm_adhoc_storage->s_adhoc_ctx =
            (ADM_adhoc_context_t) calloc(1, sizeof(adm_adhoc_context));
    if(!adm_adhoc_storage->s_adhoc_ctx) {
        error_msg = "Could not allocate ADM_adhoc_context_t";
        goto cleanup_on_error;
    }

    memcpy(adm_adhoc_storage->s_adhoc_ctx, adhoc_ctx, sizeof(*adhoc_ctx));

    return adm_adhoc_storage;

cleanup_on_error:
    if(error_msg) {
        LOGGER_ERROR(error_msg);
    }

    [[maybe_unused]] ADM_return_t ret =
            ADM_adhoc_storage_destroy(adm_adhoc_storage);
    assert(ret);

    return NULL;
}

ADM_return_t
ADM_adhoc_storage_destroy(ADM_adhoc_storage_t adhoc_storage) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!adhoc_storage) {
        LOGGER_ERROR("Invalid ADM_adhoc_storage_t")
        return ADM_EBADARGS;
    }

    if(adhoc_storage->s_name) {
        free((void*) adhoc_storage->s_name);
    }

    if(adhoc_storage->s_adhoc_ctx) {
        free(adhoc_storage->s_adhoc_ctx);
    }

    free(adhoc_storage);
    return ret;
}

ADM_adhoc_resources_t
ADM_adhoc_resources_create(ADM_node_t nodes[], size_t nodes_len) {

    struct adm_adhoc_resources* adm_adhoc_resources =
            (struct adm_adhoc_resources*) malloc(sizeof(*adm_adhoc_resources));

    const char* error_msg = NULL;
    ADM_node_list_t nodes_list = NULL;

    if(!adm_adhoc_resources) {
        error_msg = "Could not allocate ADM_adhoc_resources_t";
        goto cleanup_on_error;
    }

    nodes_list = ADM_node_list_create(nodes, nodes_len);

    if(!nodes_list) {
        error_msg = "Could not allocate ADM_adhoc_resources_t";
        goto cleanup_on_error;
    }

    adm_adhoc_resources->r_nodes = nodes_list;

    return adm_adhoc_resources;

cleanup_on_error:
    if(error_msg) {
        LOGGER_ERROR(error_msg);
    }

    if(adm_adhoc_resources) {
        ADM_adhoc_resources_destroy(adm_adhoc_resources);
    }

    return NULL;
}

ADM_return_t
ADM_adhoc_resources_destroy(ADM_adhoc_resources_t res) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!res) {
        LOGGER_ERROR("Invalid ADM_storage_resources_t")
        return ADM_EBADARGS;
    }

    if(res->r_nodes) {
        ADM_node_list_destroy(res->r_nodes);
    }

    free(res);
    return ret;
}

ADM_pfs_storage_t
ADM_pfs_storage_create(const char* name, ADM_pfs_storage_type_t type,
                       uint64_t id, ADM_pfs_context_t pfs_ctx) {

    struct adm_pfs_storage* adm_pfs_storage =
            (struct adm_pfs_storage*) malloc(sizeof(*adm_pfs_storage));
    const char* error_msg = NULL;

    if(!adm_pfs_storage) {
        LOGGER_ERROR("Could not allocate ADM_pfs_storage_t");
        return NULL;
    }

    if(!name) {
        LOGGER_ERROR("Null storage name")
        return NULL;
    }

    if(!pfs_ctx) {
        LOGGER_ERROR("Null storage context")
        return NULL;
    }

    adm_pfs_storage->s_name =
            (const char*) calloc(strlen(name) + 1, sizeof(char));
    strcpy((char*) adm_pfs_storage->s_name, name);
    adm_pfs_storage->s_type = type;
    adm_pfs_storage->s_id = id;

    adm_pfs_storage->s_pfs_ctx =
            (ADM_pfs_context_t) calloc(1, sizeof(adm_pfs_context));
    if(!adm_pfs_storage->s_pfs_ctx) {
        error_msg = "Could not allocate ADM_pfs_context_t";
        goto cleanup_on_error;
    }

    memcpy(adm_pfs_storage->s_pfs_ctx, pfs_ctx, sizeof(*pfs_ctx));

    return adm_pfs_storage;

cleanup_on_error:
    if(error_msg) {
        LOGGER_ERROR(error_msg);
    }

    [[maybe_unused]] ADM_return_t ret =
            ADM_pfs_storage_destroy(adm_pfs_storage);
    assert(ret);

    return NULL;
}

ADM_return_t
ADM_pfs_storage_destroy(ADM_pfs_storage_t pfs_storage) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!pfs_storage) {
        LOGGER_ERROR("Invalid ADM_pfs_storage_t")
        return ADM_EBADARGS;
    }

    if(pfs_storage->s_name) {
        free((void*) pfs_storage->s_name);
    }

    if(pfs_storage->s_pfs_ctx) {
        free(pfs_storage->s_pfs_ctx);
    }

    free(pfs_storage);
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
                         ADM_adhoc_access_t access_type,
                         ADM_adhoc_resources_t adhoc_resources,
                         uint32_t walltime, bool should_flush) {

    struct adm_adhoc_context* adm_adhoc_context =
            (struct adm_adhoc_context*) malloc(sizeof(*adm_adhoc_context));

    if(!adm_adhoc_context) {
        LOGGER_ERROR("Could not allocate ADM_adhoc_context_t");
        return NULL;
    }

    adm_adhoc_context->c_mode = exec_mode;
    adm_adhoc_context->c_access = access_type;
    adm_adhoc_context->c_resources = adhoc_resources;
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

    if(mountpoint) {
        size_t n = strlen(mountpoint);
        adm_pfs_context->c_mount = (const char*) calloc(n + 1, sizeof(char));
        strcpy((char*) adm_pfs_context->c_mount, mountpoint);
    }

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

ADM_job_resources_t
ADM_job_resources_create(ADM_node_t nodes[], size_t nodes_len) {

    struct adm_job_resources* adm_job_resources =
            (struct adm_job_resources*) malloc(sizeof(*adm_job_resources));

    const char* error_msg = NULL;
    ADM_node_list_t nodes_list = NULL;

    if(!adm_job_resources) {
        error_msg = "Could not allocate ADM_job_resources_t";
        goto cleanup_on_error;
    }

    nodes_list = ADM_node_list_create(nodes, nodes_len);

    if(!nodes_list) {
        error_msg = "Could not allocate ADM_job_resources_t";
        goto cleanup_on_error;
    }

    adm_job_resources->r_nodes = nodes_list;

    return adm_job_resources;

cleanup_on_error:
    if(error_msg) {
        LOGGER_ERROR(error_msg);
    }

    if(adm_job_resources) {
        ADM_job_resources_destroy(adm_job_resources);
    }

    return NULL;
}

ADM_return_t
ADM_job_resources_destroy(ADM_job_resources_t res) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!res) {
        LOGGER_ERROR("Invalid ADM_job_resources_t")
        return ADM_EBADARGS;
    }

    if(res->r_nodes) {
        ADM_node_list_destroy(res->r_nodes);
    }

    free(res);
    return ret;
}


ADM_job_requirements_t
ADM_job_requirements_create(ADM_dataset_t inputs[], size_t inputs_len,
                            ADM_dataset_t outputs[], size_t outputs_len,
                            ADM_adhoc_storage_t adhoc_storage) {

    struct adm_job_requirements* adm_job_reqs =
            (struct adm_job_requirements*) calloc(
                    1, sizeof(struct adm_job_requirements));

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

    if(!adhoc_storage) {
        return adm_job_reqs;
    }

    if(adhoc_storage->s_type != ADM_ADHOC_STORAGE_GEKKOFS &&
       adhoc_storage->s_type != ADM_ADHOC_STORAGE_DATACLAY &&
       adhoc_storage->s_type != ADM_ADHOC_STORAGE_EXPAND &&
       adhoc_storage->s_type != ADM_ADHOC_STORAGE_HERCULES) {
        error_msg = "Invalid adhoc_storage type";
        goto cleanup_on_error;
    }

    adm_job_reqs->r_adhoc_storage = ADM_adhoc_storage_create(
            adhoc_storage->s_name, adhoc_storage->s_type, adhoc_storage->s_id,
            adhoc_storage->s_adhoc_ctx);

    return adm_job_reqs;

cleanup_on_error:

    if(error_msg) {
        LOGGER_ERROR(error_msg);
    }

    if(adm_job_reqs) {
        ADM_job_requirements_destroy(adm_job_reqs);
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

    if(reqs->r_adhoc_storage) {
        ADM_adhoc_storage_destroy(reqs->r_adhoc_storage);
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
 * @param [in] slurm_id The SLURM_JOB_ID for this job
 * @return A valid JOB HANDLE or NULL in case of failure.
 */
ADM_job_t
ADM_job_create(uint64_t id, uint64_t slurm_id) {

    struct adm_job* adm_job = (struct adm_job*) malloc(sizeof(struct adm_job));

    if(!adm_job) {
        LOGGER_ERROR("Could not allocate ADM_job_t")
        return NULL;
    }

    adm_job->j_id = id;
    adm_job->j_slurm_id = slurm_id;

    return adm_job;
}

/**
 * Destroy a ADM_job_t created by ADM_job_create().
 *
 * @remark This function is not actually part of the public API, but it is
 * useful to have for internal purposes
 *
 * @param[in] reqs The ADM_job_t to destroy.
 * @return ADM_SUCCESS or corresponding error code.
 */
ADM_return_t
ADM_job_destroy(ADM_job_t job) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!job) {
        LOGGER_ERROR("Invalid ADM_job_t")
        return ADM_EBADARGS;
    }

    free(job);
    return ret;
}

/**
 * Initialize a transfer handle that can be used by clients to refer to a
 * transfer.
 *
 * @remark This function is not actually part of the public API, but it is
 * useful to have for internal purposes
 *
 * @param [in] id The identifier for this transfer
 * @return A valid TRANSFER HANDLE or NULL in case of failure.
 */
ADM_transfer_t
ADM_transfer_create(uint64_t id) {

    struct adm_transfer* adm_transfer =
            (struct adm_transfer*) malloc(sizeof(struct adm_transfer));

    if(!adm_transfer) {
        LOGGER_ERROR("Could not allocate ADM_transfer_t")
        return NULL;
    }

    adm_transfer->t_id = id;

    return adm_transfer;
}

/**
 * Destroy a ADM_transfer_t created by ADM_transfer_create().
 *
 * @remark This function is not actually part of the public API, but it is
 * useful to have for internal purposes
 *
 * @param[in] tx The ADM_transfer_t to destroy.
 * @return ADM_SUCCESS or corresponding error code.
 */
ADM_return_t
ADM_transfer_destroy(ADM_transfer_t tx) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!tx) {
        LOGGER_ERROR("Invalid ADM_transfer_t")
        return ADM_EBADARGS;
    }

    free(tx);
    return ret;
}

ADM_qos_limit_list_t
ADM_qos_limit_list_create(ADM_qos_limit_t limits[], size_t length) {

    ADM_qos_limit_list_t p = (ADM_qos_limit_list_t) malloc(sizeof(*p));

    if(!p) {
        LOGGER_ERROR("Could not allocate ADM_qos_limit_list_t")
        return NULL;
    }

    const char* error_msg = NULL;

    p->l_length = length;
    p->l_limits = (struct adm_qos_limit*) calloc(length, sizeof(adm_qos_limit));

    if(!p->l_limits) {
        error_msg = "Could not allocate ADM_qos_limit_list_t";
        goto cleanup_on_error;
    }

    for(size_t i = 0; i < length; ++i) {
        memcpy(&p->l_limits[i], limits[i], sizeof(adm_qos_limit));
    }

    return p;

cleanup_on_error:
    if(p->l_limits) {
        free(p->l_limits);
    }
    free(p);

    if(error_msg) {
        LOGGER_ERROR(error_msg);
    }

    return NULL;
}

ADM_return_t
ADM_qos_limit_list_destroy(ADM_qos_limit_list_t list) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!list) {
        LOGGER_ERROR("Invalid ADM_qos_limit_list_t")
        return ADM_EBADARGS;
    }

    // We cannot call ADM_qos_limit_destroy here because adm_limits
    // are stored as a consecutive array in memory. Thus, we free
    // the entities themselves and then the array.
    if(list->l_limits) {
        for(size_t i = 0; i < list->l_length; ++i) {

            ADM_qos_entity_t entity = list->l_limits[i].l_entity;

            if(entity) {
                ADM_qos_entity_destroy(entity);
            }
        }
        free(list->l_limits);
    }

    free(list);
    return ret;
}


/******************************************************************************/
/* C++ Type definitions and related functions                                 */
/******************************************************************************/

extern "C" {
const char*
ADM_strerror(ADM_return_t errnum);
};

namespace admire {

std::string_view
error_code::message() const {
    return ::ADM_strerror(m_value);
}

class server::impl {

public:
    impl(std::string protocol, std::string address)
        : m_protocol(std::move(protocol)), m_address(std::move(address)) {}

    std::string
    protocol() const {
        return m_protocol;
    }

    std::string
    address() const {
        return m_address;
    }

private:
    std::string m_protocol;
    std::string m_address;
};

server::server(std::string protocol, std::string address)
    : m_pimpl(std::make_unique<server::impl>(std::move(protocol),
                                             std::move(address))) {}

server::server(const ADM_server_t& srv)
    : server::server(srv->s_protocol, srv->s_address) {}

server::server(server&&) noexcept = default;

server&
server::operator=(server&&) noexcept = default;

server::~server() = default;

std::string
server::protocol() const {
    return m_pimpl->protocol();
}

std::string
server::address() const {
    return m_pimpl->address();
}

class node::impl {

public:
    impl() = default;
    explicit impl(std::string hostname) : m_hostname(std::move(hostname)) {}

    std::string
    hostname() const {
        return m_hostname;
    }

    template <class Archive>
    void
    load(Archive& ar) {
        ar(SCORD_SERIALIZATION_NVP(m_hostname));
    }

    template <class Archive>
    void
    save(Archive& ar) const {
        ar(SCORD_SERIALIZATION_NVP(m_hostname));
    }

private:
    std::string m_hostname;
};

node::node() = default;

node::node(std::string hostname)
    : m_pimpl(std::make_unique<node::impl>(std::move(hostname))) {}

node::node(const ADM_node_t& node) : node::node(node->n_hostname) {}

node::node(const node& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

node::node(node&&) noexcept = default;

node&
node::operator=(const node& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

node&
node::operator=(node&&) noexcept = default;

node::~node() = default;

std::string
node::hostname() const {
    return m_pimpl->hostname();
}

// since the PIMPL class is fully defined at this point, we can now
// define the serialization function
template <class Archive>
inline void
node::serialize(Archive& ar) {
    ar(SCORD_SERIALIZATION_NVP(m_pimpl));
}

//  we must also explicitly instantiate our template functions for
//  serialization in the desired archives
template void
node::impl::save<scord::network::serialization::output_archive>(
        scord::network::serialization::output_archive&) const;

template void
node::impl::load<scord::network::serialization::input_archive>(
        scord::network::serialization::input_archive&);

template void
node::serialize<scord::network::serialization::output_archive>(
        scord::network::serialization::output_archive&);

template void
node::serialize<scord::network::serialization::input_archive>(
        scord::network::serialization::input_archive&);

class job::impl {

public:
    impl(job_id id, slurm_job_id slurm_job_id)
        : m_id(id), m_slurm_job_id(slurm_job_id) {}
    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;

    job_id
    id() const {
        return m_id;
    }

    slurm_job_id
    slurm_id() const {
        return m_slurm_job_id;
    }

private:
    job_id m_id;
    slurm_job_id m_slurm_job_id;
};

job::resources::resources(std::vector<admire::node> nodes)
    : m_nodes(std::move(nodes)) {}

job::resources::resources(ADM_job_resources_t res) {
    assert(res->r_nodes);
    m_nodes.reserve(res->r_nodes->l_length);

    for(size_t i = 0; i < res->r_nodes->l_length; ++i) {
        m_nodes.emplace_back(res->r_nodes->l_nodes[i].n_hostname);
    }
}

std::vector<admire::node>
job::resources::nodes() const {
    return m_nodes;
}

job::job(job_id id, slurm_job_id slurm_job_id)
    : m_pimpl(std::make_unique<job::impl>(id, slurm_job_id)) {}

job::job(ADM_job_t job) : job::job(job->j_id, job->j_slurm_id) {}

job::job(job&&) noexcept = default;

job&
job::operator=(job&&) noexcept = default;

job::job(const job& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

job&
job::operator=(const job& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

job::~job() = default;

job_id
job::id() const {
    return m_pimpl->id();
}

job_id
job::slurm_id() const {
    return m_pimpl->slurm_id();
}

class transfer::impl {

public:
    explicit impl(transfer_id id) : m_id(id) {}

    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;

    transfer_id
    id() const {
        return m_id;
    }

private:
    transfer_id m_id;
};

transfer::transfer(transfer_id id)
    : m_pimpl(std::make_unique<transfer::impl>(id)) {}

transfer::transfer(ADM_transfer_t transfer)
    : transfer::transfer(transfer->t_id) {}

transfer::transfer(transfer&&) noexcept = default;

transfer&
transfer::operator=(transfer&&) noexcept = default;

transfer::transfer(const transfer& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

transfer&
transfer::operator=(const transfer& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

transfer::~transfer() = default;

transfer_id
transfer::id() const {
    return m_pimpl->id();
}

class dataset::impl {
public:
    explicit impl(std::string id) : m_id(std::move(id)) {}

    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;
    ~impl() = default;

    std::string
    id() const {
        return m_id;
    }

private:
    std::string m_id;
};

dataset::dataset(std::string id)
    : m_pimpl(std::make_unique<dataset::impl>(std::move(id))) {}

dataset::dataset(ADM_dataset_t dataset) : dataset::dataset(dataset->d_id) {}

dataset::dataset(const dataset& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

dataset::dataset(dataset&&) noexcept = default;

dataset&
dataset::operator=(const dataset& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

dataset&
dataset::operator=(dataset&&) noexcept = default;

dataset::~dataset() = default;

std::string
dataset::id() const {
    return m_pimpl->id();
}

adhoc_storage::resources::resources(std::vector<admire::node> nodes)
    : m_nodes(std::move(nodes)) {}

adhoc_storage::resources::resources(ADM_adhoc_resources_t res) {
    assert(res->r_nodes);
    m_nodes.reserve(res->r_nodes->l_length);

    for(size_t i = 0; i < res->r_nodes->l_length; ++i) {
        m_nodes.emplace_back(res->r_nodes->l_nodes[i].n_hostname);
    }
}

std::vector<admire::node>
adhoc_storage::resources::nodes() const {
    return m_nodes;
}

adhoc_storage::ctx::ctx(adhoc_storage::execution_mode exec_mode,
                        adhoc_storage::access_type access_type,
                        adhoc_storage::resources resources,
                        std::uint32_t walltime, bool should_flush)
    : m_exec_mode(exec_mode), m_access_type(access_type),
      m_resources(std::move(resources)), m_walltime(walltime),
      m_should_flush(should_flush) {}

adhoc_storage::ctx::ctx(ADM_adhoc_context_t ctx)
    : adhoc_storage::ctx(static_cast<execution_mode>(ctx->c_mode),
                         static_cast<enum access_type>(ctx->c_access),
                         adhoc_storage::resources{ctx->c_resources},
                         ctx->c_walltime, ctx->c_should_bg_flush) {}

adhoc_storage::execution_mode
adhoc_storage::ctx::exec_mode() const {
    return m_exec_mode;
}

adhoc_storage::access_type
adhoc_storage::ctx::access_type() const {
    return m_access_type;
}

adhoc_storage::resources
adhoc_storage::ctx::resources() const {
    return m_resources;
}

std::uint32_t
adhoc_storage::ctx::walltime() const {
    return m_walltime;
}

bool
adhoc_storage::ctx::should_flush() const {
    return m_should_flush;
}

class adhoc_storage::impl {

public:
    explicit impl(enum adhoc_storage::type type, std::string name,
                  std::uint64_t id, adhoc_storage::ctx ctx)
        : m_type(type), m_name(std::move(name)), m_id(id),
          m_ctx(std::move(ctx)) {}
    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;
    ~impl() = default;

    std::string
    name() const {
        return m_name;
    }

    enum type
    type() const {
        return m_type;
    }

    std::uint64_t
    id() const {
        return m_id;
    }

    adhoc_storage::ctx
    context() const {
        return m_ctx;
    }

    void
    update(adhoc_storage::ctx new_ctx) {
        m_ctx = std::move(new_ctx);
    }

private:
    enum type m_type;
    std::string m_name;
    std::uint64_t m_id;
    adhoc_storage::ctx m_ctx;
};

adhoc_storage::adhoc_storage(enum adhoc_storage::type type, std::string name,
                             std::uint64_t id, execution_mode exec_mode,
                             access_type access_type,
                             adhoc_storage::resources res,
                             std::uint32_t walltime, bool should_flush)
    : m_pimpl(std::make_unique<impl>(
              type, std::move(name), id,
              adhoc_storage::ctx{exec_mode, access_type, std::move(res),
                                 walltime, should_flush})) {}

adhoc_storage::adhoc_storage(ADM_adhoc_storage_t st)
    : m_pimpl(std::make_unique<impl>(
              static_cast<enum adhoc_storage::type>(st->s_type), st->s_name,
              st->s_id, adhoc_storage::ctx{st->s_adhoc_ctx})) {}

adhoc_storage::adhoc_storage(enum adhoc_storage::type type, std::string name,
                             std::uint64_t id, const adhoc_storage::ctx& ctx)
    : m_pimpl(std::make_unique<impl>(type, std::move(name), id, ctx)) {}

adhoc_storage::adhoc_storage(const adhoc_storage& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

adhoc_storage::adhoc_storage(adhoc_storage&&) noexcept = default;

adhoc_storage&
adhoc_storage::operator=(const adhoc_storage& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

adhoc_storage&
adhoc_storage::operator=(adhoc_storage&&) noexcept = default;

std::string
adhoc_storage::name() const {
    return m_pimpl->name();
}

enum adhoc_storage::type
adhoc_storage::type() const {
    return m_pimpl->type();
}

std::uint64_t
adhoc_storage::id() const {
    return m_pimpl->id();
}

adhoc_storage::ctx
adhoc_storage::context() const {
    return m_pimpl->context();
}

void
adhoc_storage::update(admire::adhoc_storage::ctx new_ctx) {
    return m_pimpl->update(std::move(new_ctx));
}

adhoc_storage::~adhoc_storage() = default;

pfs_storage::ctx::ctx(std::filesystem::path mount_point)
    : m_mount_point(std::move(mount_point)) {}

pfs_storage::ctx::ctx(ADM_pfs_context_t ctx) : pfs_storage::ctx(ctx->c_mount) {}

std::filesystem::path
pfs_storage::ctx::mount_point() const {
    return m_mount_point;
}

class pfs_storage::impl {

public:
    explicit impl(enum pfs_storage::type type, std::string name,
                  std::uint64_t id, pfs_storage::ctx ctx)
        : m_type(type), m_name(std::move(name)), m_id(id),
          m_ctx(std::move(ctx)) {}
    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;
    ~impl() = default;

    enum type
    type() const {
        return m_type;
    };

    std::string
    name() const {
        return m_name;
    }

    std::uint64_t
    id() const {
        return m_id;
    };

    pfs_storage::ctx
    context() const {
        return m_ctx;
    }

    void
    update(pfs_storage::ctx new_ctx) {
        m_ctx = std::move(new_ctx);
    }

private:
    enum type m_type;
    std::string m_name;
    std::uint64_t m_id;
    pfs_storage::ctx m_ctx;
};

pfs_storage::pfs_storage(enum pfs_storage::type type, std::string name,
                         std::uint64_t id, std::filesystem::path mount_point)
    : m_pimpl(std::make_unique<impl>(
              type, std::move(name), id,
              pfs_storage::ctx{std::move(mount_point)})) {}

pfs_storage::pfs_storage(enum pfs_storage::type type, std::string name,
                         std::uint64_t id, const pfs_storage::ctx& pfs_ctx)
    : m_pimpl(std::make_unique<impl>(type, std::move(name), id, pfs_ctx)) {}

pfs_storage::pfs_storage(ADM_pfs_storage_t st)
    : m_pimpl(std::make_unique<impl>(
              static_cast<enum pfs_storage::type>(st->s_type), st->s_name,
              st->s_id, pfs_storage::ctx{st->s_pfs_ctx})) {}

pfs_storage::pfs_storage(const pfs_storage& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

pfs_storage::pfs_storage(pfs_storage&&) noexcept = default;

pfs_storage&
pfs_storage::operator=(const pfs_storage& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

pfs_storage&
pfs_storage::operator=(pfs_storage&&) noexcept = default;

pfs_storage::~pfs_storage() = default;

std::string
pfs_storage::name() const {
    return m_pimpl->name();
}

enum pfs_storage::type
pfs_storage::type() const {
    return m_pimpl->type();
}

std::uint64_t
pfs_storage::id() const {
    return m_pimpl->id();
}

pfs_storage::ctx
pfs_storage::context() const {
    return m_pimpl->context();
}

void
pfs_storage::update(admire::pfs_storage::ctx new_ctx) {
    return m_pimpl->update(std::move(new_ctx));
}

class job_requirements::impl {

public:
    impl(std::vector<admire::dataset> inputs,
         std::vector<admire::dataset> outputs)
        : m_inputs(std::move(inputs)), m_outputs(std::move(outputs)) {}

    impl(std::vector<admire::dataset> inputs,
         std::vector<admire::dataset> outputs,
         admire::adhoc_storage adhoc_storage)
        : m_inputs(std::move(inputs)), m_outputs(std::move(outputs)),
          m_adhoc_storage(std::move(adhoc_storage)) {}


    explicit impl(ADM_job_requirements_t reqs) {
        m_inputs.reserve(reqs->r_inputs->l_length);

        for(size_t i = 0; i < reqs->r_inputs->l_length; ++i) {
            m_inputs.emplace_back(reqs->r_inputs->l_datasets[i].d_id);
        }

        m_outputs.reserve(reqs->r_outputs->l_length);

        for(size_t i = 0; i < reqs->r_outputs->l_length; ++i) {
            m_outputs.emplace_back(reqs->r_outputs->l_datasets[i].d_id);
        }

        if(reqs->r_adhoc_storage) {
            m_adhoc_storage = admire::adhoc_storage(reqs->r_adhoc_storage);
        }
    }

    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;

    std::vector<admire::dataset>
    inputs() const {
        return m_inputs;
    }

    std::vector<admire::dataset>
    outputs() const {
        return m_outputs;
    }

    std::optional<admire::adhoc_storage>
    adhoc_storage() const {
        return m_adhoc_storage;
    }

private:
    std::vector<admire::dataset> m_inputs;
    std::vector<admire::dataset> m_outputs;
    std::optional<admire::adhoc_storage> m_adhoc_storage;
};


job_requirements::job_requirements(std::vector<admire::dataset> inputs,
                                   std::vector<admire::dataset> outputs)
    : m_pimpl(std::make_unique<impl>(std::move(inputs), std::move(outputs))) {}

job_requirements::job_requirements(std::vector<admire::dataset> inputs,
                                   std::vector<admire::dataset> outputs,
                                   admire::adhoc_storage adhoc_storage)
    : m_pimpl(std::make_unique<impl>(std::move(inputs), std::move(outputs),
                                     std::move(adhoc_storage))) {}

job_requirements::job_requirements(ADM_job_requirements_t reqs)
    : m_pimpl(std::make_unique<impl>(reqs)) {}

job_requirements::job_requirements(const job_requirements& other) noexcept
    : m_pimpl(std::make_unique<job_requirements::impl>(*other.m_pimpl)) {}

job_requirements::job_requirements(job_requirements&&) noexcept = default;

job_requirements&
job_requirements::operator=(const job_requirements& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

job_requirements&
job_requirements::operator=(job_requirements&&) noexcept = default;

job_requirements::~job_requirements() = default;

std::vector<admire::dataset>
job_requirements::inputs() const {
    return m_pimpl->inputs();
}

std::vector<admire::dataset>
job_requirements::outputs() const {
    return m_pimpl->outputs();
}

std::optional<admire::adhoc_storage>
job_requirements::adhoc_storage() const {
    return m_pimpl->adhoc_storage();
}

namespace qos {

class entity::impl {
public:
    template <typename T>
    impl(const admire::qos::scope& s, T&& data) : m_scope(s), m_data(data) {}

    explicit impl(ADM_qos_entity_t entity)
        : m_scope(static_cast<qos::scope>(entity->e_scope)),
          m_data(init_helper(entity)) {}

    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;

    admire::qos::scope
    scope() const {
        return m_scope;
    }

    template <typename T>
    T
    data() const {
        return std::get<T>(m_data);
    }

private:
    static std::variant<dataset, node, job, transfer>
    init_helper(ADM_qos_entity_t entity) {
        switch(entity->e_scope) {
            case ADM_QOS_SCOPE_DATASET:
                return admire::dataset(entity->e_dataset);
            case ADM_QOS_SCOPE_NODE:
                return admire::node(entity->e_node);
            case ADM_QOS_SCOPE_JOB:
                return admire::job(entity->e_job);
            case ADM_QOS_SCOPE_TRANSFER:
                return admire::transfer(entity->e_transfer);
            default:
                throw std::runtime_error(fmt::format(
                        "Unexpected scope value: {}", entity->e_scope));
        }
    }


private:
    admire::qos::scope m_scope;
    std::variant<dataset, node, job, transfer> m_data;
};

template <typename T>
entity::entity(admire::qos::scope s, T&& data)
    : m_pimpl(std::make_unique<entity::impl>(s, std::forward<T>(data))) {}

entity::entity(ADM_qos_entity_t entity)
    : m_pimpl(std::make_unique<entity::impl>(entity)) {}

entity::entity(const entity& other) noexcept
    : m_pimpl(std::make_unique<entity::impl>(*other.m_pimpl)) {}

entity::entity(entity&&) noexcept = default;

entity&
entity::operator=(const entity& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

entity&
entity::operator=(entity&&) noexcept = default;

entity::~entity() = default;

admire::qos::scope
entity::scope() const {
    return m_pimpl->scope();
}

template <>
admire::node
entity::data<admire::node>() const {
    return m_pimpl->data<admire::node>();
}

template <>
admire::job
entity::data<admire::job>() const {
    return m_pimpl->data<admire::job>();
}

template <>
admire::dataset
entity::data<admire::dataset>() const {
    return m_pimpl->data<admire::dataset>();
}

template <>
admire::transfer
entity::data<admire::transfer>() const {
    return m_pimpl->data<admire::transfer>();
}


class limit::impl {

public:
    impl(admire::qos::subclass cls, uint64_t value, admire::qos::entity e)
        : m_subclass(cls), m_value(value), m_entity(std::move(e)) {}

    impl(admire::qos::subclass cls, uint64_t value)
        : m_subclass(cls), m_value(value) {}

    explicit impl(ADM_qos_limit_t l)
        : m_subclass(static_cast<qos::subclass>(l->l_class)),
          m_value(l->l_value),
          m_entity(l->l_entity ? std::optional(admire::qos::entity(l->l_entity))
                               : std::nullopt) {}

    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;

    std::optional<admire::qos::entity>
    entity() const {
        return m_entity;
    }

    admire::qos::subclass
    subclass() const {
        return m_subclass;
    }

    uint64_t
    value() const {
        return m_value;
    }

private:
    admire::qos::subclass m_subclass;
    uint64_t m_value;
    std::optional<admire::qos::entity> m_entity;
};

limit::limit(admire::qos::subclass cls, uint64_t value)
    : m_pimpl(std::make_unique<limit::impl>(cls, value)) {}

limit::limit(admire::qos::subclass cls, uint64_t value,
             const admire::qos::entity& e)
    : m_pimpl(std::make_unique<limit::impl>(cls, value, e)) {}

limit::limit(ADM_qos_limit_t l) : m_pimpl(std::make_unique<limit::impl>(l)) {}

limit::limit(const limit& other) noexcept
    : m_pimpl(std::make_unique<limit::impl>(*other.m_pimpl)) {}

limit::limit(limit&&) noexcept = default;

limit&
limit::operator=(const limit& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

limit&
limit::operator=(limit&&) noexcept = default;

limit::~limit() = default;

std::optional<admire::qos::entity>
limit::entity() const {
    return m_pimpl->entity();
}

admire::qos::subclass
limit::subclass() const {
    return m_pimpl->subclass();
}

uint64_t
limit::value() const {
    return m_pimpl->value();
}

} // namespace qos

} // namespace admire
