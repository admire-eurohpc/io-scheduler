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
#include <network/proto/rpc_types.h>
#include <utility>
#include <utils/c_ptr.hpp>
#include <cstdarg>
#include "admire_types.hpp"

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
            adm_storage->s_adhoc_ctx = *(adm_adhoc_context*) ctx;
            break;

        case ADM_STORAGE_LUSTRE:
        case ADM_STORAGE_GPFS:
            adm_storage->s_pfs_ctx = *(adm_pfs_context*) ctx;
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
    adm_job_reqs->r_storage = storage;

    if(!storage) {
        return adm_job_reqs;
    }

    if(storage->s_type != ADM_STORAGE_GEKKOFS &&
       storage->s_type != ADM_STORAGE_DATACLAY &&
       storage->s_type != ADM_STORAGE_EXPAND &&
       storage->s_type != ADM_STORAGE_HERCULES) {
        error_msg = "Invalid adhoc_storage argument";
        goto cleanup_on_error;
    }

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

    if(reqs->r_storage) {
        ADM_storage_destroy(reqs->r_storage);
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
/* C++ Type definitions and related functions                                 */
/******************************************************************************/

namespace admire {

server::server(std::string protocol, std::string address)
    : m_protocol(std::move(protocol)), m_address(std::move(address)) {}

server::server(const ADM_server_t& srv)
    : m_protocol(srv->s_protocol), m_address(srv->s_address) {}

job::job(job_id id) : m_id(id) {}

job::job(ADM_job_t job) : m_id(job->j_id) {}

ADM_job_t
job::to_ctype() const {
    return ADM_job_create(m_id);
}

storage::storage(storage::type type, std::string id)
    : m_id(std::move(id)), m_type(type) {}

adhoc_storage::adhoc_storage(storage::type type, std::string id,
                             execution_mode exec_mode, access_type access_type,
                             std::uint32_t nodes, std::uint32_t walltime,
                             bool should_flush)
    : storage(type, std::move(id)), m_ctx{exec_mode, access_type, nodes,
                                          walltime, should_flush} {}

adhoc_storage::adhoc_storage(storage::type type, std::string id,
                             ADM_adhoc_context_t ctx)
    : storage(type, std::move(id)), m_ctx(ctx) {}

ADM_storage_t
adhoc_storage::to_rpc_type() const {
    return ADM_storage_create(m_id.c_str(),
                              static_cast<ADM_storage_type_t>(m_type),
                              m_ctx.to_rpc_type());
}


adhoc_storage::context::context(adhoc_storage::execution_mode exec_mode,
                                adhoc_storage::access_type access_type,
                                std::uint32_t nodes, std::uint32_t walltime,
                                bool should_flush)
    : m_exec_mode(exec_mode), m_access_type(access_type), m_nodes(nodes),
      m_walltime(walltime), m_should_flush(should_flush) {}

adhoc_storage::context::context(ADM_adhoc_context_t ctx)
    : context(static_cast<execution_mode>(ctx->c_mode),
              static_cast<access_type>(ctx->c_access), ctx->c_nodes,
              ctx->c_walltime, ctx->c_should_bg_flush) {}

ADM_adhoc_context_t
adhoc_storage::context::to_rpc_type() const {
    return ADM_adhoc_context_create(
            static_cast<ADM_adhoc_mode_t>(m_exec_mode),
            static_cast<ADM_adhoc_access_t>(m_access_type), m_nodes, m_walltime,
            m_should_flush);
}


pfs_storage::pfs_storage(storage::type type, std::string id,
                         std::filesystem::path mount_point)
    : storage(type, std::move(id)), m_ctx(std::move(mount_point)) {}

pfs_storage::pfs_storage(storage::type type, std::string id,
                         ADM_pfs_context_t ctx)
    : storage(type, std::move(id)), m_ctx(ctx) {}

ADM_storage_t
pfs_storage::to_rpc_type() const {
    return ADM_storage_create(m_id.c_str(),
                              static_cast<ADM_storage_type_t>(m_type),
                              m_ctx.to_rpc_type());
}


pfs_storage::context::context(std::filesystem::path mount_point)
    : m_mount_point(std::move(mount_point)) {}

pfs_storage::context::context(ADM_pfs_context_t ctx) : context(ctx->c_mount) {}

ADM_pfs_context_t
pfs_storage::context::to_rpc_type() const {
    return ADM_pfs_context_create(m_mount_point.c_str());
}


job_requirements::job_requirements(std::vector<admire::dataset> inputs,
                                   std::vector<admire::dataset> outputs)
    : m_inputs(std::move(inputs)), m_outputs(std::move(outputs)) {}

job_requirements::job_requirements(std::vector<admire::dataset> inputs,
                                   std::vector<admire::dataset> outputs,
                                   std::unique_ptr<storage> storage)
    : m_inputs(std::move(inputs)), m_outputs(std::move(outputs)),
      m_storage(std::move(storage)) {}

job_requirements::job_requirements(ADM_job_requirements_t reqs) {
    m_inputs.reserve(reqs->r_inputs->l_length);

    for(size_t i = 0; i < reqs->r_inputs->l_length; ++i) {
        m_inputs.emplace_back(reqs->r_inputs->l_datasets[i].d_id);
    }

    m_outputs.reserve(reqs->r_outputs->l_length);

    for(size_t i = 0; i < reqs->r_outputs->l_length; ++i) {
        m_outputs.emplace_back(reqs->r_outputs->l_datasets[i].d_id);
    }

    if(reqs->r_storage) {

        switch(reqs->r_storage->s_type) {

            case ADM_STORAGE_GEKKOFS:
            case ADM_STORAGE_DATACLAY:
            case ADM_STORAGE_EXPAND:
            case ADM_STORAGE_HERCULES:
                m_storage = std::make_unique<adhoc_storage>(
                        static_cast<storage::type>(reqs->r_storage->s_type),
                        reqs->r_storage->s_id, &reqs->r_storage->s_adhoc_ctx);
                break;
            case ADM_STORAGE_LUSTRE:
            case ADM_STORAGE_GPFS:
                m_storage = std::make_unique<pfs_storage>(
                        static_cast<storage::type>(reqs->r_storage->s_type),
                        reqs->r_storage->s_id, &reqs->r_storage->s_pfs_ctx);
                break;
        }
    }
}

ADM_job_requirements_t
job_requirements::to_rpc_type() const {
    using scord::utils::c_ptr;
    using scord::utils::c_ptr_vector;
    using dataset_vector = c_ptr_vector<adm_dataset, ADM_dataset_destroy>;

    dataset_vector inputs;
    inputs.reserve(m_inputs.size());

    for(const auto& in : m_inputs) {
        inputs.emplace_back(ADM_dataset_create(in.m_id.c_str()));
    }

    dataset_vector outputs;
    outputs.reserve(m_outputs.size());

    for(const auto& out : m_outputs) {
        outputs.emplace_back(ADM_dataset_create(out.m_id.c_str()));
    }

    return ADM_job_requirements_create(
            inputs.data(), inputs.size(), outputs.data(), outputs.size(),
            m_storage ? m_storage->to_rpc_type() : nullptr);
}


} // namespace admire
