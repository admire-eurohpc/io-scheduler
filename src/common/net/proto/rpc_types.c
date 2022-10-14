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

#include "rpc_types.h"

hg_return_t (*hg_proc_ADM_qos_scope_t)(hg_proc_t, void*) = hg_proc_hg_uint32_t;
hg_return_t (*hg_proc_ADM_qos_class_t)(hg_proc_t, void*) = hg_proc_hg_uint32_t;

hg_return_t
hg_proc_ADM_node_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_node_t* node = (ADM_node_t*) data;
    ADM_node_t tmp = NULL;
    hg_size_t node_length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            // find out the length of the adm_node object we need to send
            node_length = *node ? sizeof(adm_node) : 0;
            ret = hg_proc_hg_size_t(proc, &node_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!node_length) {
                return HG_SUCCESS;
            }

            // if we actually need to send an adm_node object,
            // write it to the mercury buffer
            tmp = *node;

            ret = hg_proc_adm_node(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            break;

        case HG_DECODE:
            // find out the length of the adm_node object
            ret = hg_proc_hg_size_t(proc, &node_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!node_length) {
                *node = NULL;
                break;
            }

            // if the received adm_node object was not NULL, read each of
            // its fields from the mercury buffer
            tmp = (adm_node*) calloc(1, sizeof(adm_node));

            ret = hg_proc_adm_node(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            // return the newly-created ctx
            *node = tmp;
            break;

        case HG_FREE:
            tmp = *node;
            free(tmp);
            break;
    }

    return ret;
}

hg_return_t
hg_proc_ADM_dataset_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_dataset_t* dataset = (ADM_dataset_t*) data;
    ADM_dataset_t tmp = NULL;
    hg_size_t dataset_length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            // find out the length of the adm_dataset object we need to send
            dataset_length = *dataset ? sizeof(adm_dataset) : 0;
            ret = hg_proc_hg_size_t(proc, &dataset_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!dataset_length) {
                return HG_SUCCESS;
            }

            // if we actually need to send an adm_dataset object,
            // write it to the mercury buffer
            tmp = *dataset;

            ret = hg_proc_adm_dataset(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            break;

        case HG_DECODE:
            // find out the length of the adm_dataset object
            ret = hg_proc_hg_size_t(proc, &dataset_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!dataset_length) {
                *dataset = NULL;
                break;
            }

            // if the received adm_dataset object was not NULL, read each of
            // its fields from the mercury buffer
            tmp = (adm_dataset*) calloc(1, sizeof(adm_dataset));

            ret = hg_proc_adm_dataset(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            // return the newly-created ctx
            *dataset = tmp;
            break;

        case HG_FREE:
            tmp = *dataset;
            free(tmp);
            break;
    }

    return ret;
}

hg_return_t
hg_proc_ADM_job_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_job_t* job = (ADM_job_t*) data;
    ADM_job_t tmp = NULL;
    hg_size_t job_length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            // find out the length of the adm_storage object we need to send
            job_length = *job ? sizeof(adm_job) : 0;
            ret = hg_proc_hg_size_t(proc, &job_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!job_length) {
                return HG_SUCCESS;
            }

            // if we actually need to send an adm_job object,
            // write it to the mercury buffer
            tmp = *job;

            ret = hg_proc_adm_job(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            break;

        case HG_DECODE:
            // find out the length of the adm_storage object
            ret = hg_proc_hg_size_t(proc, &job_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!job_length) {
                *job = NULL;
                break;
            }

            // if the received adm_job object was not NULL, read each of
            // its fields from the mercury buffer
            tmp = (adm_job*) calloc(1, sizeof(adm_job));

            ret = hg_proc_adm_job(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            // return the newly-created ctx
            *job = tmp;
            break;

        case HG_FREE:
            tmp = *job;
            free(tmp);
            break;
    }

    return ret;
}

hg_return_t
hg_proc_ADM_transfer_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_transfer_t* transfer = (ADM_transfer_t*) data;
    ADM_transfer_t tmp = NULL;
    hg_size_t transfer_length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            // find out the length of the adm_transfer object we need to send
            transfer_length = *transfer ? sizeof(adm_transfer) : 0;
            ret = hg_proc_hg_size_t(proc, &transfer_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!transfer_length) {
                return HG_SUCCESS;
            }

            // if we actually need to send an adm_transfer object,
            // write it to the mercury buffer
            tmp = *transfer;

            ret = hg_proc_adm_transfer(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            break;

        case HG_DECODE:
            // find out the length of the adm_transfer object
            ret = hg_proc_hg_size_t(proc, &transfer_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!transfer_length) {
                *transfer = NULL;
                break;
            }

            // if the received adm_transfer object was not NULL, read each of
            // its fields from the mercury buffer
            tmp = (adm_transfer*) calloc(1, sizeof(adm_transfer));

            ret = hg_proc_adm_transfer(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            // return the newly-created ctx
            *transfer = tmp;
            break;

        case HG_FREE:
            tmp = *transfer;
            free(tmp);
            break;
    }

    return ret;
}

hg_return_t
hg_proc_ADM_dataset_list_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_dataset_list_t* list = (ADM_dataset_list_t*) data;
    ADM_dataset_list_t tmp = NULL;

    hg_size_t length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            tmp = *list;
            // write the length of the list
            length = tmp->l_length;
            ret = hg_proc_hg_size_t(proc, &tmp->l_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            // write the list
            for(size_t i = 0; i < length; ++i) {
                ret = hg_proc_adm_dataset(proc, &tmp->l_datasets[i]);

                if(ret != HG_SUCCESS) {
                    break;
                }
            }
            break;

        case HG_DECODE: {

            // find out the length of the list
            ret = hg_proc_hg_size_t(proc, &length);

            if(ret != HG_SUCCESS) {
                break;
            }

            // loop and create list elements
            tmp = (ADM_dataset_list_t) calloc(1,
                                              sizeof(struct adm_dataset_list));
            tmp->l_length = length;
            tmp->l_datasets =
                    (adm_dataset*) calloc(length, sizeof(adm_dataset));

            for(size_t i = 0; i < length; ++i) {
                ret = hg_proc_adm_dataset(proc, &tmp->l_datasets[i]);

                if(ret != HG_SUCCESS) {
                    break;
                }
            }

            // return the newly-created list
            *list = tmp;

            break;
        }

        case HG_FREE:
            tmp = *list;
            free(tmp->l_datasets);
            free(tmp);
            ret = HG_SUCCESS;
            break;
    }

    return ret;
}


hg_return_t
hg_proc_ADM_storage_t(hg_proc_t proc, void* data) {
    (void) proc;
    (void) data;

    hg_return_t ret = HG_SUCCESS;
    ADM_storage_t* storage = (ADM_storage_t*) data;
    ADM_storage_t tmp = NULL;
    hg_size_t storage_length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            // find out the length of the adm_storage object we need to send
            storage_length = *storage ? sizeof(adm_storage) : 0;
            ret = hg_proc_hg_size_t(proc, &storage_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!storage_length) {
                return HG_SUCCESS;
            }

            // if we actually need to send an adm_storage object,
            // write each of its fields to the mercury buffer
            tmp = *storage;

            // 1. the storage type
            ret = hg_proc_hg_uint32_t(proc, &tmp->s_type);

            if(ret != HG_SUCCESS) {
                break;
            }

            // 2. the storage id
            ret = hg_proc_hg_const_string_t(proc, &tmp->s_name);

            if(ret != HG_SUCCESS) {
                break;
            }

            // 3. the server-assigned id
            ret = hg_proc_hg_int64_t(proc, &tmp->s_id);

            if(ret != HG_SUCCESS) {
                break;
            }

            // 4. the appropriate storage context
            switch(tmp->s_type) {
                case ADM_STORAGE_GEKKOFS:
                case ADM_STORAGE_DATACLAY:
                case ADM_STORAGE_EXPAND:
                case ADM_STORAGE_HERCULES:
                    ret = hg_proc_ADM_adhoc_context_t(proc, &tmp->s_adhoc_ctx);
                    break;
                case ADM_STORAGE_LUSTRE:
                case ADM_STORAGE_GPFS:
                    ret = hg_proc_ADM_pfs_context_t(proc, &tmp->s_pfs_ctx);
                    break;
            }

            break;

        case HG_DECODE:
            // find out the length of the adm_storage object
            ret = hg_proc_hg_size_t(proc, &storage_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!storage_length) {
                *storage = NULL;
                break;
            }

            // if the received adm_storage object was not NULL, read each of
            // its fields from the mercury buffer
            tmp = (adm_storage*) calloc(1, sizeof(adm_storage));

            // 1. the storage type
            ret = hg_proc_uint32_t(proc, &tmp->s_type);

            if(ret != HG_SUCCESS) {
                break;
            }

            // 2. the storage id
            ret = hg_proc_hg_const_string_t(proc, &tmp->s_name);

            if(ret != HG_SUCCESS) {
                break;
            }

            // 3. the server-assigned id
            ret = hg_proc_hg_int64_t(proc, &tmp->s_id);

            if(ret != HG_SUCCESS) {
                break;
            }

            // 4. the appropriate storage context
            switch(tmp->s_type) {
                case ADM_STORAGE_GEKKOFS:
                case ADM_STORAGE_DATACLAY:
                case ADM_STORAGE_EXPAND:
                case ADM_STORAGE_HERCULES:
                    ret = hg_proc_ADM_adhoc_context_t(proc, &tmp->s_adhoc_ctx);
                    break;
                case ADM_STORAGE_LUSTRE:
                case ADM_STORAGE_GPFS:
                    ret = hg_proc_ADM_pfs_context_t(proc, &tmp->s_adhoc_ctx);
                    break;
            }

            // return the newly-created ctx
            *storage = tmp;
            break;

        case HG_FREE:
            tmp = *storage;
            free(tmp);
            break;
    }

    return ret;
}

hg_return_t
hg_proc_ADM_adhoc_context_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_adhoc_context_t* ctx = (ADM_adhoc_context_t*) data;
    ADM_adhoc_context_t tmp = NULL;
    hg_size_t ctx_length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            // find out the length of the context
            ctx_length = *ctx ? sizeof(adm_adhoc_context) : 0;
            ret = hg_proc_hg_size_t(proc, &ctx_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!ctx_length) {
                return HG_SUCCESS;
            }

            // if not NULL, write the context
            tmp = *ctx;
            ret = hg_proc_adm_adhoc_context(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            break;

        case HG_DECODE: {

            // find out the length of the context
            ret = hg_proc_hg_size_t(proc, &ctx_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!ctx_length) {
                *ctx = NULL;
                break;
            }

            // if not NULL, read the context
            tmp = (ADM_adhoc_context_t) calloc(
                    1, sizeof(struct adm_adhoc_context));
            ret = hg_proc_adm_adhoc_context(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            // return the newly-created ctx
            *ctx = tmp;
            break;
        }

        case HG_FREE:
            tmp = *ctx;
            free(tmp);
            ret = HG_SUCCESS;
            break;
    }

    return ret;
}

hg_return_t
hg_proc_ADM_pfs_context_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_pfs_context_t* ctx = (ADM_pfs_context_t*) data;
    ADM_pfs_context_t tmp = NULL;
    hg_size_t ctx_length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            // find out the length of the context
            ctx_length = *ctx ? sizeof(adm_pfs_context) : 0;
            ret = hg_proc_hg_size_t(proc, &ctx_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!ctx_length) {
                return HG_SUCCESS;
            }

            // if not NULL, write the context
            tmp = *ctx;
            ret = hg_proc_adm_pfs_context(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            break;

        case HG_DECODE: {

            // find out the length of the context
            ret = hg_proc_hg_size_t(proc, &ctx_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!ctx_length) {
                *ctx = NULL;
                break;
            }

            // if not NULL, read the context
            tmp = (ADM_pfs_context_t) calloc(1, sizeof(struct adm_pfs_context));
            ret = hg_proc_adm_pfs_context(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            // return the newly-created ctx
            *ctx = tmp;
            break;
        }

        case HG_FREE:
            tmp = *ctx;
            free(tmp);
            ret = HG_SUCCESS;
            break;
    }

    return ret;
}


hg_return_t
hg_proc_ADM_qos_limit_list_t(hg_proc_t proc, void* data) {
    hg_return_t ret = HG_SUCCESS;
    ADM_qos_limit_list_t* list = (ADM_qos_limit_list_t*) data;
    ADM_qos_limit_list_t tmp = NULL;

    hg_size_t length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            tmp = *list;
            // write the length of the list
            length = tmp->l_length;
            ret = hg_proc_hg_size_t(proc, &tmp->l_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            // write the list
            for(size_t i = 0; i < length; ++i) {
                ret = hg_proc_adm_qos_limit(proc, &tmp->l_limits[i]);

                if(ret != HG_SUCCESS) {
                    break;
                }
            }
            break;

        case HG_DECODE: {

            // find out the length of the list
            ret = hg_proc_hg_size_t(proc, &length);

            if(ret != HG_SUCCESS) {
                break;
            }

            // loop and create list elements
            tmp = (ADM_qos_limit_list_t) calloc(
                    1, sizeof(struct adm_qos_limit_list));
            tmp->l_length = length;
            tmp->l_limits =
                    (adm_qos_limit*) calloc(length, sizeof(adm_qos_limit));

            for(size_t i = 0; i < length; ++i) {
                ret = hg_proc_adm_qos_limit(proc, &tmp->l_limits[i]);

                if(ret != HG_SUCCESS) {
                    break;
                }
            }

            // return the newly-created list
            *list = tmp;

            break;
        }

        case HG_FREE:
            tmp = *list;
            free(tmp->l_limits);
            free(tmp);
            ret = HG_SUCCESS;
            break;
    }

    return ret;
}

hg_return_t
hg_proc_ADM_qos_entity_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_qos_entity_t* qos_entity = (ADM_qos_entity_t*) data;
    ADM_qos_entity_t tmp = NULL;
    hg_size_t qos_entity_length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            // find out the length of the adm_qos_entity object we need to send
            qos_entity_length = *qos_entity ? sizeof(adm_qos_entity) : 0;
            ret = hg_proc_hg_size_t(proc, &qos_entity_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!qos_entity_length) {
                return HG_SUCCESS;
            }

            // if we actually need to send an adm_qos_entity object,
            // write each of its fields to the mercury buffer

            // 1. the QoS scope
            ret = hg_proc_ADM_qos_scope_t(proc, &tmp->e_scope);

            if(ret != HG_SUCCESS) {
                break;
            }

            // 2. the appropriate related data depending on the scope (i.e. an
            // ADM_node_t, ADM_job_t, ADM_dataset_t, or ADM_transfer_t)
            switch(tmp->e_scope) {

                case ADM_QOS_SCOPE_DATASET:
                    ret = hg_proc_ADM_dataset_t(proc, &tmp->e_dataset);
                    break;
                case ADM_QOS_SCOPE_NODE:
                    ret = hg_proc_ADM_node_t(proc, &tmp->e_node);
                    break;
                case ADM_QOS_SCOPE_JOB:
                    ret = hg_proc_ADM_job_t(proc, &tmp->e_job);
                    break;
                case ADM_QOS_SCOPE_TRANSFER:
                    ret = hg_proc_ADM_transfer_t(proc, &tmp->e_transfer);
                    break;
            }

            break;

        case HG_DECODE:
            // find out the length of the adm_qos_entity object
            ret = hg_proc_hg_size_t(proc, &qos_entity_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!qos_entity_length) {
                *qos_entity = NULL;
                break;
            }

            // if the received adm_qos_entity object was not NULL, read each
            // of its fields from the mercury buffer
            tmp = (adm_qos_entity*) calloc(1, sizeof(adm_qos_entity));

            // 1. the QoS scope
            ret = hg_proc_ADM_qos_scope_t(proc, &tmp->e_scope);

            if(ret != HG_SUCCESS) {
                break;
            }

            // 2. the appropriate related data depending on the scope (i.e. an
            // ADM_node_t, ADM_job_t, ADM_dataset_t, or ADM_transfer_t)
            switch(tmp->e_scope) {

                case ADM_QOS_SCOPE_DATASET:
                    ret = hg_proc_ADM_dataset_t(proc, &tmp->e_dataset);
                    break;
                case ADM_QOS_SCOPE_NODE:
                    ret = hg_proc_ADM_node_t(proc, &tmp->e_node);
                    break;
                case ADM_QOS_SCOPE_JOB:
                    ret = hg_proc_ADM_job_t(proc, &tmp->e_job);
                    break;
                case ADM_QOS_SCOPE_TRANSFER:
                    ret = hg_proc_ADM_transfer_t(proc, &tmp->e_transfer);
                    break;
            }

            // return the newly-created entity
            *qos_entity = tmp;
            break;

        case HG_FREE:
            tmp = *qos_entity;
            free(tmp);
            break;
    }

    return ret;
}

hg_return_t
hg_proc_ADM_node_list_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_node_list_t* list = (ADM_node_list_t*) data;
    ADM_node_list_t tmp = NULL;

    hg_size_t length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            tmp = *list;
            // write the length of the list
            length = tmp->l_length;
            ret = hg_proc_hg_size_t(proc, &tmp->l_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            // write the list
            for(size_t i = 0; i < length; ++i) {
                ret = hg_proc_adm_node(proc, &tmp->l_nodes[i]);

                if(ret != HG_SUCCESS) {
                    break;
                }
            }
            break;

        case HG_DECODE: {

            // find out the length of the list
            ret = hg_proc_hg_size_t(proc, &length);

            if(ret != HG_SUCCESS) {
                break;
            }

            // loop and create list elements
            tmp = (ADM_node_list_t) calloc(1, sizeof(struct adm_node_list));
            tmp->l_length = length;
            tmp->l_nodes = (adm_node*) calloc(length, sizeof(adm_node));

            for(size_t i = 0; i < length; ++i) {
                ret = hg_proc_adm_node(proc, &tmp->l_nodes[i]);

                if(ret != HG_SUCCESS) {
                    break;
                }
            }

            // return the newly-created list
            *list = tmp;

            break;
        }

        case HG_FREE:
            tmp = *list;
            free(tmp->l_nodes);
            free(tmp);
            ret = HG_SUCCESS;
            break;
    }

    return ret;
}

hg_return_t
hg_proc_ADM_adhoc_resources_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_adhoc_resources_t* res = (ADM_adhoc_resources_t*) data;
    ADM_adhoc_resources_t tmp = NULL;
    hg_size_t res_length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            // find out the length of the adm_adhoc_resources object we need to
            // send
            res_length = *res ? sizeof(adm_adhoc_resources) : 0;
            ret = hg_proc_hg_size_t(proc, &res_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!res_length) {
                return HG_SUCCESS;
            }

            // if we actually need to send an adm_transfer object,
            // write it to the mercury buffer
            tmp = *res;

            ret = hg_proc_adm_adhoc_resources(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            break;

        case HG_DECODE:
            // find out the length of the adm_transfer object
            ret = hg_proc_hg_size_t(proc, &res_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!res_length) {
                *res = NULL;
                break;
            }

            // if the received adm_adhoc_resources object was not NULL, read
            // each of its fields from the mercury buffer
            tmp = (adm_adhoc_resources*) calloc(1, sizeof(adm_adhoc_resources));

            ret = hg_proc_adm_adhoc_resources(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            // return the newly-created ctx
            *res = tmp;
            break;

        case HG_FREE:
            tmp = *res;
            free(tmp);
            break;
    }

    return ret;
}

hg_return_t
hg_proc_ADM_job_resources_t(hg_proc_t proc, void* data) {

    hg_return_t ret = HG_SUCCESS;
    ADM_job_resources_t* res = (ADM_job_resources_t*) data;
    ADM_job_resources_t tmp = NULL;
    hg_size_t res_length = 0;

    switch(hg_proc_get_op(proc)) {

        case HG_ENCODE:
            // find out the length of the adm_job_resources object we need to
            // send
            res_length = *res ? sizeof(adm_job_resources) : 0;
            ret = hg_proc_hg_size_t(proc, &res_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!res_length) {
                return HG_SUCCESS;
            }

            // if we actually need to send an adm_transfer object,
            // write it to the mercury buffer
            tmp = *res;

            ret = hg_proc_adm_job_resources(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            break;

        case HG_DECODE:
            // find out the length of the adm_transfer object
            ret = hg_proc_hg_size_t(proc, &res_length);

            if(ret != HG_SUCCESS) {
                break;
            }

            if(!res_length) {
                *res = NULL;
                break;
            }

            // if the received adm_job_resources object was not NULL, read
            // each of its fields from the mercury buffer
            tmp = (adm_job_resources*) calloc(1, sizeof(adm_job_resources));

            ret = hg_proc_adm_job_resources(proc, tmp);

            if(ret != HG_SUCCESS) {
                break;
            }

            // return the newly-created ctx
            *res = tmp;
            break;

        case HG_FREE:
            tmp = *res;
            free(tmp);
            break;
    }

    return ret;
}
