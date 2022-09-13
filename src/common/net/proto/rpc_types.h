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

#ifndef SCORD_PROTO_TYPES_HPP
#define SCORD_PROTO_TYPES_HPP

#include <stdlib.h> // NOLINT
#include <mercury_macros.h>
#include <mercury_proc_string.h>
#include <api/admire_types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * N.B. MERCURY_GEN_STRUCT_PROC requires a `typedef` as its first argument, but
 * admire_types.h also requires types to be defined as `struct T`s. Defining RPC
 * types as `typedef struct T { ... } T;` solves both problems
 */

typedef struct adm_node {
    const char* n_hostname;
} adm_node;

hg_return_t
hg_proc_ADM_node_t(hg_proc_t proc, void* data);

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_node, // NOLINT
        ((hg_const_string_t) (n_hostname))
);
// clang-format on

typedef struct adm_dataset {
    const char* d_id;
} adm_dataset;

hg_return_t
hg_proc_ADM_dataset_t(hg_proc_t proc, void* data);

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_dataset, // NOLINT
        ((hg_const_string_t) (d_id))
);
// clang-format on

typedef struct adm_job {
    uint64_t j_id;
} adm_job;

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_job, // NOLINT
        ((hg_uint64_t) (j_id))
);
// clang-format on

hg_return_t
hg_proc_ADM_job_t(hg_proc_t proc, void* data);

typedef struct adm_qos_entity {
    ADM_qos_scope_t e_scope;
    union {
        ADM_node_t e_node;
        ADM_job_t e_job;
        ADM_dataset_t e_dataset;
        ADM_transfer_t e_transfer;
    };
} adm_qos_entity;

extern hg_return_t (*hg_proc_ADM_qos_scope_t)(hg_proc_t, void*);

hg_return_t
hg_proc_ADM_qos_entity_t(hg_proc_t proc, void* data);

typedef struct adm_qos_limit {
    ADM_qos_entity_t l_entity;
    ADM_qos_class_t l_class;
    hg_uint64_t l_value;
} adm_qos_limit;

extern hg_return_t (*hg_proc_ADM_qos_class_t)(hg_proc_t, void*);

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_qos_limit, // NOLINT
        ((ADM_qos_entity_t) (l_entity))
        ((ADM_qos_class_t) (l_class))
        ((hg_uint64_t) (l_value))
)
// clang-format on

typedef struct adm_transfer {
    uint64_t t_id;
} adm_transfer;

hg_return_t
hg_proc_ADM_transfer_t(hg_proc_t proc, void* data);

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_transfer, // NOLINT
        ((hg_uint64_t) (t_id))
);
// clang-format on

typedef struct adm_dataset_info {
    // TODO: undefined for now
    int32_t placeholder;
} adm_dataset_info;

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_dataset_info, // NOLINT
        ((hg_int32_t) (placeholder))
);
// clang-format on

typedef struct adm_adhoc_context {
    /** The adhoc storage system execution mode */
    ADM_adhoc_mode_t c_mode;
    /** The adhoc storage system access type */
    ADM_adhoc_access_t c_access;
    /** The number of nodes for the adhoc storage system */
    uint32_t c_nodes;
    /** The adhoc storage system walltime */
    uint32_t c_walltime;
    /** Whether the adhoc storage system should flush data in the background */
    bool c_should_bg_flush;
} adm_adhoc_context;

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_adhoc_context, // NOLINT
        ((hg_int32_t)  (c_mode))
        ((hg_int32_t)  (c_access))
        ((hg_uint32_t) (c_nodes))
        ((hg_uint32_t) (c_walltime))
        ((hg_bool_t)   (c_should_bg_flush))
)
// clang-format on

typedef struct adm_pfs_context {
    /** The PFS mount point */
    const char* c_mount;
} adm_pfs_context;

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_pfs_context, // NOLINT
        ((hg_const_string_t) (c_mount))
);
// clang-format on

typedef struct adm_storage {
    const char* s_id;
    ADM_storage_type_t s_type;
    union {
        ADM_adhoc_context_t s_adhoc_ctx;
        ADM_pfs_context_t s_pfs_ctx;
    };
} adm_storage;

hg_return_t
hg_proc_ADM_storage_t(hg_proc_t proc, void* data);

typedef struct adm_storage_resources {
    // TODO: undefined for now
    int32_t placeholder;
} adm_storage_resources;

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_storage_resources, // NOLINT
        ((hg_int32_t) (placeholder))
);
// clang-format on

typedef struct adm_data_operation {
    // TODO: undefined for now
    int32_t placeholder;
} adm_data_operation;

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_data_operation, // NOLINT
        ((hg_int32_t) (placeholder))
);
// clang-format on


struct adm_dataset_list {
    /** An array of datasets */
    adm_dataset* l_datasets;
    /** The length of the array */
    size_t l_length;
};

hg_return_t
hg_proc_ADM_dataset_list_t(hg_proc_t proc, void* data);

hg_return_t
hg_proc_ADM_adhoc_context_t(hg_proc_t proc, void* data);

hg_return_t
hg_proc_ADM_pfs_context_t(hg_proc_t proc, void* data);


/** The I/O requirements for a job */
typedef struct adm_job_requirements {
    /** An array of input datasets */
    ADM_dataset_list_t r_inputs;
    /** An array of output datasets */
    ADM_dataset_list_t r_outputs;
    /** An optional definition for a specific storage instance */
    ADM_storage_t r_storage;
} adm_job_requirements;

// clang-format off
MERCURY_GEN_STRUCT_PROC(
    adm_job_requirements, // NOLINT
        ((ADM_dataset_list_t) (r_inputs))
        ((ADM_dataset_list_t) (r_outputs))
        ((ADM_storage_t) (r_storage))
);
// clang-format on

// clang-format off

/// ADM_register_job
MERCURY_GEN_PROC(
    ADM_register_job_in_t,
        ((adm_job_requirements) (reqs))
);

MERCURY_GEN_PROC(
    ADM_register_job_out_t,
        ((int32_t) (retval))
        ((ADM_job_t) (job))
);

/// ADM_update_job
MERCURY_GEN_PROC(
    ADM_update_job_in_t,
        ((ADM_job_t) (job))
        ((adm_job_requirements) (reqs))
);

MERCURY_GEN_PROC(
    ADM_update_job_out_t,
        ((int32_t) (retval))
);

/// ADM_remove_job
MERCURY_GEN_PROC(
    ADM_remove_job_in_t,
        ((ADM_job_t) (job))
);

MERCURY_GEN_PROC(
    ADM_remove_job_out_t,
        ((int32_t) (retval))
);

/// ADM_register_adhoc_storage
MERCURY_GEN_PROC(ADM_register_adhoc_storage_in_t, ((int32_t) (reqs)))

MERCURY_GEN_PROC(ADM_register_adhoc_storage_out_t, ((int32_t) (ret)))

/// ADM_update_adhoc_storage
MERCURY_GEN_PROC(ADM_update_adhoc_storage_in_t, ((int32_t) (reqs)))

MERCURY_GEN_PROC(ADM_update_adhoc_storage_out_t, ((int32_t) (ret)))

/// ADM_remove_adhoc_storage
MERCURY_GEN_PROC(ADM_remove_adhoc_storage_in_t, ((int32_t) (reqs)))

MERCURY_GEN_PROC(ADM_remove_adhoc_storage_out_t, ((int32_t) (ret)))

/// ADM_deploy_adhoc_storage
MERCURY_GEN_PROC(ADM_deploy_adhoc_storage_in_t, ((int32_t) (reqs)))

MERCURY_GEN_PROC(ADM_deploy_adhoc_storage_out_t, ((int32_t) (ret)))

/// ADM_register_pfs_storage
MERCURY_GEN_PROC(ADM_register_pfs_storage_in_t, ((int32_t) (reqs)))

MERCURY_GEN_PROC(ADM_register_pfs_storage_out_t, ((int32_t) (ret)))

/// ADM_update_pfs_storage
MERCURY_GEN_PROC(ADM_update_pfs_storage_in_t, ((int32_t) (reqs)))

MERCURY_GEN_PROC(ADM_update_pfs_storage_out_t, ((int32_t) (ret)))

/// ADM_remove_pfs_storage
MERCURY_GEN_PROC(ADM_remove_pfs_storage_in_t, ((int32_t) (reqs)))

MERCURY_GEN_PROC(ADM_remove_pfs_storage_out_t, ((int32_t) (ret)))

/// ADM_input
MERCURY_GEN_PROC(ADM_input_in_t,
                 ((hg_const_string_t) (origin))((hg_const_string_t) (target)))

MERCURY_GEN_PROC(ADM_input_out_t, ((int32_t) (ret)))

/// ADM_output

MERCURY_GEN_PROC(ADM_output_in_t,
                 ((hg_const_string_t) (origin))((hg_const_string_t) (target)))

MERCURY_GEN_PROC(ADM_output_out_t, ((int32_t) (ret)))

/// ADM_inout

MERCURY_GEN_PROC(ADM_inout_in_t,
                 ((hg_const_string_t) (origin))((hg_const_string_t) (target)))

MERCURY_GEN_PROC(ADM_inout_out_t, ((int32_t) (ret)))

/// ADM_adhoc_context

MERCURY_GEN_PROC(ADM_adhoc_context_in_t, ((hg_const_string_t) (context)))

MERCURY_GEN_PROC(ADM_adhoc_context_out_t,
                 ((int32_t) (ret))((int32_t) (adhoc_context)))

/// ADM_adhoc_context_id

MERCURY_GEN_PROC(ADM_adhoc_context_id_in_t, ((int32_t) (context_id)))

MERCURY_GEN_PROC(ADM_adhoc_context_id_out_t, ((int32_t) (ret)))

/// ADM_adhoc_nodes

MERCURY_GEN_PROC(ADM_adhoc_nodes_in_t, ((int32_t) (nodes)))

MERCURY_GEN_PROC(ADM_adhoc_nodes_out_t, ((int32_t) (ret)))

/// ADM_adhoc_walltime

MERCURY_GEN_PROC(ADM_adhoc_walltime_in_t, ((int32_t) (walltime)))

MERCURY_GEN_PROC(ADM_adhoc_walltime_out_t, ((int32_t) (ret)))


/// ADM_adhoc_access

MERCURY_GEN_PROC(ADM_adhoc_access_in_t, ((hg_const_string_t) (access)))

MERCURY_GEN_PROC(ADM_adhoc_access_out_t, ((int32_t) (ret)))

/// ADM_adhoc_distribution

MERCURY_GEN_PROC(ADM_adhoc_distribution_in_t,
                 ((hg_const_string_t) (data_distribution)))

MERCURY_GEN_PROC(ADM_adhoc_distribution_out_t, ((int32_t) (ret)))

/// ADM_adhoc_background_flush

MERCURY_GEN_PROC(ADM_adhoc_background_flush_in_t, ((hg_bool_t) (b_flush)))

MERCURY_GEN_PROC(ADM_adhoc_background_flush_out_t, ((int32_t) (ret)))

/// ADM_in_situ_ops

MERCURY_GEN_PROC(ADM_in_situ_ops_in_t, ((hg_const_string_t) (in_situ)))

MERCURY_GEN_PROC(ADM_in_situ_ops_out_t, ((int32_t) (ret)))

/// ADM_in_transit_ops

MERCURY_GEN_PROC(ADM_in_transit_ops_in_t, ((hg_const_string_t) (in_transit)))

MERCURY_GEN_PROC(ADM_in_transit_ops_out_t, ((int32_t) (ret)))

struct adm_qos_limit_list {
    /** An array of QoS limits */
    adm_qos_limit* l_limits;
    /** The length of the array */
    size_t l_length;
};

hg_return_t
hg_proc_ADM_qos_limit_list_t(hg_proc_t proc, void* data);

/// ADM_transfer_datasets

MERCURY_GEN_PROC(
    ADM_transfer_datasets_in_t,
        ((ADM_job_t) (job))
        ((ADM_dataset_list_t) (sources))
        ((ADM_dataset_list_t) (targets))
        ((ADM_qos_limit_list_t) (qos_limits))
        ((hg_int32_t) (mapping))
)

MERCURY_GEN_PROC(
    ADM_transfer_datasets_out_t,
        ((hg_int32_t) (retval))
        ((ADM_transfer_t) (tx)))


/// ADM_set_dataset_information

MERCURY_GEN_PROC(ADM_set_dataset_information_in_t,
                 ((int32_t) (resource_id))((hg_const_string_t) (info))(
                         (int32_t) (job_id)))

MERCURY_GEN_PROC(ADM_set_dataset_information_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

/// ADM_set_io_resources

MERCURY_GEN_PROC(ADM_set_io_resources_in_t,
                 ((int32_t) (tier_id))((hg_const_string_t) (resources))(
                         (int32_t) (job_id)))

MERCURY_GEN_PROC(ADM_set_io_resources_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

/// ADM_get_transfer_priority

MERCURY_GEN_PROC(ADM_get_transfer_priority_in_t, ((int32_t) (transfer_id)))

MERCURY_GEN_PROC(ADM_get_transfer_priority_out_t,
                 ((int32_t) (ret))((int32_t) (priority)))

/// ADM_set_transfer_priority

MERCURY_GEN_PROC(ADM_set_transfer_priority_in_t,
                 ((int32_t) (transfer_id))((int32_t) (n_positions)))

MERCURY_GEN_PROC(ADM_set_transfer_priority_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

/// ADM_cancel_transfer

MERCURY_GEN_PROC(ADM_cancel_transfer_in_t, ((int32_t) (transfer_id)))

MERCURY_GEN_PROC(ADM_cancel_transfer_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

/// ADM_get_pending_transfers

MERCURY_GEN_PROC(ADM_get_pending_transfers_in_t, ((hg_const_string_t) (value)))

MERCURY_GEN_PROC(ADM_get_pending_transfers_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (pending_transfers)))

/// ADM_set_qos_constraints

MERCURY_GEN_PROC(
        ADM_set_qos_constraints_in_t,
        ((hg_const_string_t) (scope))((hg_const_string_t) (qos_class))(
                (int32_t) (element_id))((hg_const_string_t) (class_value)))

MERCURY_GEN_PROC(ADM_set_qos_constraints_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

/// ADM_get_qos_constraints

MERCURY_GEN_PROC(ADM_get_qos_constraints_in_t,
                 ((hg_const_string_t) (scope))((int32_t) (element_id)))

MERCURY_GEN_PROC(ADM_get_qos_constraints_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (list)))

/// ADM_define_data_operation

MERCURY_GEN_PROC(ADM_define_data_operation_in_t,
                 ((hg_const_string_t) (path))((int32_t) (operation_id))(
                         (hg_const_string_t) (arguments)))

MERCURY_GEN_PROC(ADM_define_data_operation_out_t,
                 ((int32_t) (ret))((int32_t) (status)))

/// ADM_connect_data_operation

MERCURY_GEN_PROC(ADM_connect_data_operation_in_t,
                 ((int32_t) (operation_id))((hg_const_string_t) (input))(
                         (hg_bool_t) (stream))((hg_const_string_t) (arguments))(
                         (int32_t) (job_id)))

MERCURY_GEN_PROC(ADM_connect_data_operation_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (data))(
                         (hg_const_string_t) (operation_handle)))

/// ADM_finalize_data_operation

MERCURY_GEN_PROC(ADM_finalize_data_operation_in_t, ((int32_t) (operation_id)))

MERCURY_GEN_PROC(ADM_finalize_data_operation_out_t,
                 ((int32_t) (ret))((int32_t) (status)))


/// ADM_link_transfer_to_data_operation

MERCURY_GEN_PROC(ADM_link_transfer_to_data_operation_in_t,
                 ((int32_t) (operation_id))((int32_t) (transfer_id))(
                         (hg_bool_t) (stream))((hg_const_string_t) (arguments))(
                         (int32_t) (job_id)))

MERCURY_GEN_PROC(ADM_link_transfer_to_data_operation_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (operation_handle)))

/// ADM_get_statistics

MERCURY_GEN_PROC(ADM_get_statistics_in_t,
                 ((int32_t) (job_id))((int32_t) (job_step)))

MERCURY_GEN_PROC(ADM_get_statistics_out_t,
                 ((int32_t) (ret))((hg_const_string_t) (job_statistics)))

// clang-format on

#ifdef __cplusplus
};     // extern "C"
#endif // __cplusplus

#endif // SCORD_PROTO_TYPES_HPP
