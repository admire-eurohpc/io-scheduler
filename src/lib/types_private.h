/******************************************************************************
 * Copyright 2021-2023, Barcelona Supercomputing Center (BSC), Spain
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

#ifndef SCORD_TYPES_PRIVATE_H
#define SCORD_TYPES_PRIVATE_H

#include <scord/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct adm_server {
    const char* s_protocol;
    const char* s_address;
};

struct adm_node {
    const char* n_hostname;
    ADM_node_type_t n_type;
};

struct adm_dataset {
    const char* d_id;
};

struct adm_job {
    uint64_t j_id;
    uint64_t j_slurm_id;
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
    uint64_t t_id;
};

struct adm_dataset_info {
    // TODO: undefined for now
    int32_t placeholder;
};

struct adm_adhoc_context {
    /** The adhoc storage system execution mode */
    ADM_adhoc_mode_t c_mode;
    /** The adhoc storage system access type */
    ADM_adhoc_access_t c_access;
    /** The adhoc storage system walltime */
    uint32_t c_walltime;
    /** Whether the adhoc storage system should flush data in the background */
    bool c_should_bg_flush;
};

struct adm_pfs_context {
    /** The PFS mount point */
    const char* c_mount;
};

struct adm_adhoc_storage {
    const char* s_name;
    ADM_adhoc_storage_type_t s_type;
    uint64_t s_id;
    ADM_adhoc_context_t s_adhoc_ctx;
    /** The resources assigned for the adhoc storage system */
    ADM_adhoc_resources_t s_resources;
};

struct adm_pfs_storage {
    const char* s_name;
    ADM_pfs_storage_type_t s_type;
    uint64_t s_id;
    ADM_pfs_context_t s_pfs_ctx;
};

struct adm_adhoc_resources {
    ADM_node_list_t r_nodes;
};

struct adm_data_operation {
    // TODO: undefined for now
    int32_t placeholder;
};

struct adm_job_requirements {
    /** An array of input datasets */
    ADM_dataset_list_t r_inputs;
    /** An array of output datasets */
    ADM_dataset_list_t r_outputs;
    /** An optional definition for a specific storage instance */
    ADM_adhoc_storage_t r_adhoc_storage;
};

struct adm_job_resources {
    ADM_node_list_t r_nodes;
};

/* Lists */

struct adm_dataset_list {
    /** An array of datasets */
    struct adm_dataset* l_datasets;
    /** The length of the array */
    size_t l_length;
};

struct adm_qos_limit_list {
    /** An array of QoS limits */
    struct adm_qos_limit* l_limits;
    /** The length of the array */
    size_t l_length;
};

struct adm_node_list {
    /** An array of nodes */
    struct adm_node* l_nodes;
    /** The length of the array */
    size_t l_length;
};

ADM_job_t
ADM_job_create(uint64_t id, uint64_t slurm_id);

ADM_transfer_t
ADM_transfer_create(uint64_t id);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCORD_TYPES_PRIVATE_H
