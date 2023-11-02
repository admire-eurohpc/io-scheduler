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

#include <assert.h>
#include <string.h>
#include <logger/logger.h>
#include "scord/types.h"
#include "types_private.h"

#define maybe_unused __attribute__((unused))

/******************************************************************************/
/* C Type definitions and related functions                                   */
/******************************************************************************/

ADM_server_t
ADM_server_create(const char* protocol, const char* address) {

    struct adm_server* adm_server =
            (struct adm_server*) malloc(sizeof(struct adm_server));

    if(!adm_server) {
        LOGGER_ERROR("Could not allocate ADM_server_t");
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
        LOGGER_ERROR("Invalid ADM_server_t");
        return ADM_EBADARGS;
    }

    free(server);
    return ret;
}

ADM_node_t
ADM_node_create(const char* hostname, ADM_node_type_t type) {

    struct adm_node* adm_node =
            (struct adm_node*) malloc(sizeof(struct adm_node));

    if(!adm_node) {
        LOGGER_ERROR("Could not allocate ADM_node_t");
        return NULL;
    }

    if(hostname) {
        size_t n = strlen(hostname);
        adm_node->n_hostname = (const char*) calloc(n + 1, sizeof(char));
        strcpy((char*) adm_node->n_hostname, hostname);
        adm_node->n_type = type;
    }

    return adm_node;
}

const char*
ADM_node_get_hostname(ADM_node_t node) {
    if(!node) {
        LOGGER_ERROR("Invalid ADM_node_t");
        return NULL;
    }

    return node->n_hostname;
}

ADM_node_type_t
ADM_node_get_type(ADM_node_t node) {
    if(!node) {
        LOGGER_ERROR("Invalid ADM_node_t");
        return ADM_NODE_INVALID;
    }

    return node->n_type;
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
        LOGGER_ERROR("Invalid ADM_node_t");
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
        LOGGER_ERROR("Could not allocate ADM_node_list_t");
        return NULL;
    }

    const char* error_msg = NULL;

    p->l_length = length;
    p->l_nodes = (struct adm_node*) calloc(length, sizeof(struct adm_node));

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

    LOGGER_ERROR(error_msg);

    return NULL;
}

ADM_return_t
ADM_node_list_destroy(ADM_node_list_t list) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!list) {
        LOGGER_ERROR("Invalid ADM_node_list_t");
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
        LOGGER_ERROR("Could not allocate ADM_dataset_t");
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
        LOGGER_ERROR("Invalid ADM_dataset_t");
        return ADM_EBADARGS;
    }

    if(dataset->d_id) {
        free((void*) dataset->d_id);
    }

    free(dataset);
    return ret;
}

ADM_dataset_route_t
ADM_dataset_route_create(ADM_dataset_t source, ADM_dataset_t destination) {

    struct adm_dataset_route* adm_dataset_route =
            (struct adm_dataset_route*) malloc(
                    sizeof(struct adm_dataset_route));

    if(!adm_dataset_route) {
        LOGGER_ERROR("Could not allocate ADM_dataset_route_t");
        return NULL;
    }

    adm_dataset_route->d_src = ADM_dataset_create(source->d_id);

    if(!adm_dataset_route->d_src) {
        LOGGER_ERROR("Could not allocate ADM_dataset_t");
        return NULL;
    }

    adm_dataset_route->d_dst = ADM_dataset_create(destination->d_id);

    if(!adm_dataset_route->d_dst) {
        LOGGER_ERROR("Could not allocate ADM_dataset_t");
        return NULL;
    }

    return adm_dataset_route;
}

ADM_dataset_route_t
ADM_dataset_route_copy(ADM_dataset_route_t dst, const ADM_dataset_route_t src) {

    if(!src || !dst) {
        return NULL;
    }

    // copy all primitive types
    *dst = *src;

    // duplicate copy any pointer types
    if(src->d_src) {
        dst->d_src = ADM_dataset_create(src->d_src->d_id);
    }

    if(src->d_dst) {
        dst->d_dst = ADM_dataset_create(src->d_dst->d_id);
    }

    return dst;
}

ADM_return_t
ADM_dataset_route_destroy(ADM_dataset_route_t route) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!route) {
        LOGGER_ERROR("Invalid ADM_dataset_route_t");
        return ADM_EBADARGS;
    }

    if(route->d_src) {
        ADM_dataset_destroy(route->d_src);
    }

    if(route->d_dst) {
        ADM_dataset_destroy(route->d_dst);
    }

    free(route);
    return ret;
}

ADM_qos_entity_t
ADM_qos_entity_create(ADM_qos_scope_t scope, void* data) {

    struct adm_qos_entity* adm_qos_entity =
            (struct adm_qos_entity*) malloc(sizeof(struct adm_qos_entity));

    if(!adm_qos_entity) {
        LOGGER_ERROR("Could not allocate ADM_qos_entity_t");
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
        LOGGER_ERROR("Invalid ADM_qos_entity_t");
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
        LOGGER_ERROR("Could not allocate ADM_qos_limit_t");
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
        LOGGER_ERROR("Invalid ADM_qos_limit_t");
        return ADM_EBADARGS;
    }

    free(limit);
    return ret;
}

ADM_return_t
ADM_qos_limit_destroy_all(ADM_qos_limit_t limit) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!limit) {
        LOGGER_ERROR("Invalid ADM_qos_limit_t");
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
        LOGGER_ERROR("Invalid ADM_dataset_info_t");
        return ADM_EBADARGS;
    }

    free(dataset_info);
    return ret;
}

ADM_dataset_list_t
ADM_dataset_list_create(ADM_dataset_t datasets[], size_t length) {

    ADM_dataset_list_t p = (ADM_dataset_list_t) malloc(sizeof(*p));

    if(!p) {
        LOGGER_ERROR("Could not allocate ADM_dataset_list_t");
        return NULL;
    }

    const char* error_msg = NULL;

    p->l_length = length;
    p->l_datasets =
            (struct adm_dataset*) calloc(length, sizeof(struct adm_dataset));

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

    LOGGER_ERROR(error_msg);

    return NULL;
}

ADM_return_t
ADM_dataset_list_destroy(ADM_dataset_list_t list) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!list) {
        LOGGER_ERROR("Invalid ADM_pfs_context_t");
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

ADM_dataset_route_list_t
ADM_dataset_route_list_create(ADM_dataset_route_t routes[], size_t length) {

    ADM_dataset_route_list_t p = (ADM_dataset_route_list_t) malloc(sizeof(*p));

    if(!p) {
        LOGGER_ERROR("Could not allocate ADM_dataset_route_list_t");
        return NULL;
    }

    const char* error_msg = NULL;

    p->l_length = length;
    p->l_routes = (struct adm_dataset_route*) calloc(
            length, sizeof(struct adm_dataset_route));

    if(!p->l_routes) {
        error_msg = "Could not allocate ADM_dataset_route_list_t";
        goto cleanup_on_error;
    }

    for(size_t i = 0; i < length; ++i) {
        if(!ADM_dataset_route_copy(&p->l_routes[i], routes[i])) {
            error_msg = "Could not allocate ADM_dataset_route_list_t";
            goto cleanup_on_error;
        };
    }

    return p;

cleanup_on_error:
    if(p->l_routes) {
        free(p->l_routes);
    }
    free(p);

    LOGGER_ERROR(error_msg);

    return NULL;
}

ADM_return_t
ADM_dataset_route_list_destroy(ADM_dataset_route_list_t list) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!list) {
        LOGGER_ERROR("Invalid ADM_dataset_route_list_t");
        return ADM_EBADARGS;
    }

    // We cannot call ADM_dataset_route_destroy here because adm_dataset_routes
    // are stored as a consecutive array in memory. Thus, we free
    // the dataset route ids themselves and then the array.
    if(list->l_routes) {
        for(size_t i = 0; i < list->l_length; ++i) {
            ADM_dataset_route_destroy(&list->l_routes[i]);
        }
        free(list->l_routes);
    }

    free(list);
    return ret;
}

ADM_adhoc_storage_t
ADM_adhoc_storage_create(const char* name, ADM_adhoc_storage_type_t type,
                         uint64_t id, ADM_adhoc_context_t adhoc_ctx,
                         ADM_adhoc_resources_t adhoc_resources) {

    struct adm_adhoc_storage* adm_adhoc_storage =
            (struct adm_adhoc_storage*) malloc(sizeof(*adm_adhoc_storage));
    const char* error_msg = NULL;

    if(!adm_adhoc_storage) {
        LOGGER_ERROR("Could not allocate ADM_adhoc_storage_t");
        return NULL;
    }

    if(!name) {
        LOGGER_ERROR("Null storage name");
        return NULL;
    }

    if(!adhoc_ctx) {
        LOGGER_ERROR("Null storage context");
        return NULL;
    }

    adm_adhoc_storage->s_name =
            (const char*) calloc(strlen(name) + 1, sizeof(char));
    strcpy((char*) adm_adhoc_storage->s_name, name);
    adm_adhoc_storage->s_type = type;
    adm_adhoc_storage->s_id = id;

    adm_adhoc_storage->s_adhoc_ctx =
            (ADM_adhoc_context_t) calloc(1, sizeof(struct adm_adhoc_context));
    if(!adm_adhoc_storage->s_adhoc_ctx) {
        error_msg = "Could not allocate ADM_adhoc_context_t";
        goto cleanup_on_error;
    }

    memcpy(adm_adhoc_storage->s_adhoc_ctx, adhoc_ctx, sizeof(*adhoc_ctx));

    adm_adhoc_storage->s_resources = adhoc_resources;

    return adm_adhoc_storage;

cleanup_on_error:
    LOGGER_ERROR(error_msg);

    maybe_unused ADM_return_t ret =
            ADM_adhoc_storage_destroy(adm_adhoc_storage);
    assert(ret);

    return NULL;
}

ADM_return_t
ADM_adhoc_storage_destroy(ADM_adhoc_storage_t adhoc_storage) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!adhoc_storage) {
        LOGGER_ERROR("Invalid ADM_adhoc_storage_t");
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
    LOGGER_ERROR(error_msg);

    if(adm_adhoc_resources) {
        ADM_adhoc_resources_destroy(adm_adhoc_resources);
    }

    return NULL;
}

ADM_return_t
ADM_adhoc_resources_destroy(ADM_adhoc_resources_t res) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!res) {
        LOGGER_ERROR("Invalid ADM_storage_resources_t");
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
        LOGGER_ERROR("Null storage name");
        return NULL;
    }

    if(!pfs_ctx) {
        LOGGER_ERROR("Null storage context");
        return NULL;
    }

    adm_pfs_storage->s_name =
            (const char*) calloc(strlen(name) + 1, sizeof(char));
    strcpy((char*) adm_pfs_storage->s_name, name);
    adm_pfs_storage->s_type = type;
    adm_pfs_storage->s_id = id;

    adm_pfs_storage->s_pfs_ctx =
            (ADM_pfs_context_t) calloc(1, sizeof(struct adm_pfs_context));
    if(!adm_pfs_storage->s_pfs_ctx) {
        error_msg = "Could not allocate ADM_pfs_context_t";
        goto cleanup_on_error;
    }

    memcpy(adm_pfs_storage->s_pfs_ctx, pfs_ctx, sizeof(*pfs_ctx));

    return adm_pfs_storage;

cleanup_on_error:
    LOGGER_ERROR(error_msg);

    maybe_unused ADM_return_t ret = ADM_pfs_storage_destroy(adm_pfs_storage);
    assert(ret);

    return NULL;
}

ADM_return_t
ADM_pfs_storage_destroy(ADM_pfs_storage_t pfs_storage) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!pfs_storage) {
        LOGGER_ERROR("Invalid ADM_pfs_storage_t");
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
        LOGGER_ERROR("Invalid ADM_data_operation_t");
        return ADM_EBADARGS;
    }

    free(op);
    return ret;
}

ADM_adhoc_context_t
ADM_adhoc_context_create(const char* ctl_address, const char* stager_address,
                         ADM_adhoc_mode_t exec_mode,
                         ADM_adhoc_access_t access_type, uint32_t walltime,
                         bool should_flush) {

    if(!ctl_address) {
        LOGGER_ERROR("The address to the controller cannot be NULL");
        return NULL;
    }

    if(!stager_address) {
        LOGGER_ERROR("The address to the stager cannot be NULL");
        return NULL;
    }

    struct adm_adhoc_context* adm_adhoc_context =
            (struct adm_adhoc_context*) malloc(sizeof(*adm_adhoc_context));

    if(!adm_adhoc_context) {
        LOGGER_ERROR("Could not allocate ADM_adhoc_context_t");
        return NULL;
    }


    size_t n = strlen(ctl_address);
    adm_adhoc_context->c_ctl_address =
            (const char*) calloc(n + 1, sizeof(char));
    strcpy((char*) adm_adhoc_context->c_ctl_address, ctl_address);

    n = strlen(stager_address);
    adm_adhoc_context->c_stager_address =
            (const char*) calloc(n + 1, sizeof(char));
    strcpy((char*) adm_adhoc_context->c_stager_address, stager_address);

    adm_adhoc_context->c_mode = exec_mode;
    adm_adhoc_context->c_access = access_type;
    adm_adhoc_context->c_walltime = walltime;
    adm_adhoc_context->c_should_bg_flush = should_flush;

    return adm_adhoc_context;
}

ADM_return_t
ADM_adhoc_context_destroy(ADM_adhoc_context_t ctx) {


    ADM_return_t ret = ADM_SUCCESS;

    if(!ctx) {
        LOGGER_ERROR("Invalid ADM_adhoc_context_t");
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
        LOGGER_ERROR("Invalid ADM_pfs_context_t");
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
    LOGGER_ERROR(error_msg);

    if(adm_job_resources) {
        ADM_job_resources_destroy(adm_job_resources);
    }

    return NULL;
}

ADM_return_t
ADM_job_resources_destroy(ADM_job_resources_t res) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!res) {
        LOGGER_ERROR("Invalid ADM_job_resources_t");
        return ADM_EBADARGS;
    }

    if(res->r_nodes) {
        ADM_node_list_destroy(res->r_nodes);
    }

    free(res);
    return ret;
}


ADM_job_requirements_t
ADM_job_requirements_create(ADM_dataset_route_t inputs[], size_t inputs_len,
                            ADM_dataset_route_t outputs[], size_t outputs_len,
                            ADM_dataset_route_t expected_outputs[],
                            size_t expected_outputs_len,
                            ADM_adhoc_storage_t adhoc_storage) {

    struct adm_job_requirements* adm_job_reqs =
            (struct adm_job_requirements*) calloc(
                    1, sizeof(struct adm_job_requirements));

    if(!adm_job_reqs) {
        LOGGER_ERROR("Could not allocate ADM_job_requirements_t");
        return NULL;
    }

    ADM_dataset_route_list_t inputs_list = NULL;
    ADM_dataset_route_list_t outputs_list = NULL;
    ADM_dataset_route_list_t expected_outputs_list = NULL;
    const char* error_msg = NULL;

    inputs_list = ADM_dataset_route_list_create(inputs, inputs_len);

    if(!inputs_list) {
        error_msg = "Could not allocate ADM_job_requirements_t";
        goto cleanup_on_error;
    }

    outputs_list = ADM_dataset_route_list_create(outputs, outputs_len);

    if(!outputs_list) {
        error_msg = "Could not allocate ADM_job_requirements_t";
        goto cleanup_on_error;
    }

    expected_outputs_list = ADM_dataset_route_list_create(expected_outputs,
                                                          expected_outputs_len);

    if(!expected_outputs_list) {
        error_msg = "Could not allocate ADM_job_requirements_t";
        goto cleanup_on_error;
    }

    adm_job_reqs->r_inputs = inputs_list;
    adm_job_reqs->r_outputs = outputs_list;
    adm_job_reqs->r_expected_outputs = expected_outputs_list;

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
            adhoc_storage->s_adhoc_ctx, adhoc_storage->s_resources);

    return adm_job_reqs;

cleanup_on_error:
    LOGGER_ERROR(error_msg);
    ADM_job_requirements_destroy(adm_job_reqs);
    return NULL;
}

ADM_return_t
ADM_job_requirements_destroy(ADM_job_requirements_t reqs) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!reqs) {
        LOGGER_ERROR("Invalid ADM_job_requirements_t");
        return ADM_EBADARGS;
    }

    if(reqs->r_inputs) {
        ADM_dataset_route_list_destroy(reqs->r_inputs);
    }

    if(reqs->r_outputs) {
        ADM_dataset_route_list_destroy(reqs->r_outputs);
    }

    if(reqs->r_expected_outputs) {
        ADM_dataset_route_list_destroy(reqs->r_expected_outputs);
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
        LOGGER_ERROR("Could not allocate ADM_job_t");
        return NULL;
    }

    adm_job->j_id = id;
    adm_job->j_slurm_id = slurm_id;

    return adm_job;
}

ADM_return_t
ADM_job_destroy(ADM_job_t job) {
    ADM_return_t ret = ADM_SUCCESS;

    if(!job) {
        LOGGER_ERROR("Invalid ADM_job_t");
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
        LOGGER_ERROR("Could not allocate ADM_transfer_t");
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
        LOGGER_ERROR("Invalid ADM_transfer_t");
        return ADM_EBADARGS;
    }

    free(tx);
    return ret;
}

ADM_qos_limit_list_t
ADM_qos_limit_list_create(ADM_qos_limit_t limits[], size_t length) {

    ADM_qos_limit_list_t p = (ADM_qos_limit_list_t) malloc(sizeof(*p));

    if(!p) {
        LOGGER_ERROR("Could not allocate ADM_qos_limit_list_t");
        return NULL;
    }

    const char* error_msg = NULL;

    p->l_length = length;
    p->l_limits = (struct adm_qos_limit*) calloc(length,
                                                 sizeof(struct adm_qos_limit));

    if(!p->l_limits) {
        error_msg = "Could not allocate ADM_qos_limit_list_t";
        goto cleanup_on_error;
    }

    for(size_t i = 0; i < length; ++i) {
        memcpy(&p->l_limits[i], limits[i], sizeof(struct adm_qos_limit));
    }

    return p;

cleanup_on_error:
    if(p->l_limits) {
        free(p->l_limits);
    }
    free(p);

    LOGGER_ERROR(error_msg);

    return NULL;
}

ADM_return_t
ADM_qos_limit_list_destroy(ADM_qos_limit_list_t list) {

    ADM_return_t ret = ADM_SUCCESS;

    if(!list) {
        LOGGER_ERROR("Invalid ADM_qos_limit_list_t");
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
