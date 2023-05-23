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

#ifndef SCORD_TYPES_H
#define SCORD_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Public type and struct definitions                                         */
/******************************************************************************/

/* ----------------------------------------------------- */
/*              Error return codes                       */
/* ----------------------------------------------------- */
typedef enum {
    ADM_SUCCESS = 0,
    ADM_ESNAFU,
    ADM_EBADARGS,
    ADM_ENOMEM,
    ADM_EEXISTS,
    ADM_ENOENT,
    ADM_EADHOC_BUSY,
    ADM_EOTHER,
    ADM_ERR_MAX = 512
} ADM_return_t;


/* ----------------------------------------------------- */
/*              General library types                    */
/* ----------------------------------------------------- */

/** A RPC server */
typedef struct adm_server* ADM_server_t;

/** Node types */
typedef enum {
    ADM_NODE_REGULAR,
    ADM_NODE_ADMINISTRATIVE,
} ADM_node_type_t;

/** A node */
typedef struct adm_node* ADM_node_t;

/** A list of nodes */
typedef struct adm_node_list* ADM_node_list_t;

/** A job */
typedef struct adm_job* ADM_job_t;


/* ----------------------------------------------------- */
/*              Jobs                                     */
/* ----------------------------------------------------- */

/** Information about resources assigned to a job */
typedef struct adm_job_resources* ADM_job_resources_t;

/** I/O stats from a job */
typedef struct {
    // TODO: empty for now
    uint32_t placeholder;
} ADM_job_stats_t;

/** The I/O requirements for a job */
typedef struct adm_job_requirements* ADM_job_requirements_t;


/* ----------------------------------------------------- */
/*              Datasets                                 */
/* ----------------------------------------------------- */

/** A dataset */
typedef struct adm_dataset* ADM_dataset_t;

/** Information about a dataset */
typedef struct adm_dataset_info* ADM_dataset_info_t;

/** A list of datasets */
typedef struct adm_dataset_list* ADM_dataset_list_t;

/** A list of QoS limits */
typedef struct adm_qos_limit_list* ADM_qos_limit_list_t;


/* ----------------------------------------------------- */
/*              Storage tiers                            */
/* ----------------------------------------------------- */

/** Adhoc storage tier types */
typedef enum {
    ADM_ADHOC_STORAGE_GEKKOFS,
    ADM_ADHOC_STORAGE_DATACLAY,
    ADM_ADHOC_STORAGE_EXPAND,
    ADM_ADHOC_STORAGE_HERCULES,
} ADM_adhoc_storage_type_t;

/** Adhoc storage tier types */
typedef enum {
    ADM_PFS_STORAGE_LUSTRE,
    ADM_PFS_STORAGE_GPFS,
} ADM_pfs_storage_type_t;

/** An adhoc storage tier */
typedef struct adm_adhoc_storage* ADM_adhoc_storage_t;

/** A PFS storage tier */
typedef struct adm_pfs_storage* ADM_pfs_storage_t;

/** Information about resources assigned to a storage tier */
typedef struct adm_adhoc_resources* ADM_adhoc_resources_t;

/** Execution modes for an adhoc storage system */
typedef enum {
    ADM_ADHOC_MODE_IN_JOB_SHARED,
    ADM_ADHOC_MODE_IN_JOB_DEDICATED,
    ADM_ADHOC_MODE_SEPARATE_NEW,
    ADM_ADHOC_MODE_SEPARATE_EXISTING
} ADM_adhoc_mode_t;

/** Access modes for an adhoc storage system */
typedef enum {
    ADM_ADHOC_ACCESS_RDONLY,
    ADM_ADHOC_ACCESS_WRONLY,
    ADM_ADHOC_ACCESS_RDWR,
} ADM_adhoc_access_t;

/** Abstract type to represent data distributions for adhoc storage systems */
typedef struct adm_adhoc_data_distribution* ADM_adhoc_data_distribution_t;

/** The context for an  adhoc storage instance */
typedef struct adm_adhoc_context* ADM_adhoc_context_t;

/** The context for a parallel file system storage */
typedef struct adm_pfs_context* ADM_pfs_context_t;


/* ----------------------------------------------------- */
/*              Quality of Service                       */
/* ----------------------------------------------------- */

/** The scope affected by a QoS limit */
typedef enum {
    ADM_QOS_SCOPE_DATASET,
    ADM_QOS_SCOPE_NODE,
    ADM_QOS_SCOPE_JOB,
    ADM_QOS_SCOPE_TRANSFER
} ADM_qos_scope_t;

/** The class of QoS limit applied to a scope */
typedef enum { ADM_QOS_CLASS_BANDWIDTH, ADM_QOS_CLASS_IOPS } ADM_qos_class_t;

/** An ADMIRE entity upon which QoS can be defined */
typedef struct adm_qos_entity* ADM_qos_entity_t;

/** A QoS limit */
typedef struct adm_qos_limit* ADM_qos_limit_t;


/* ----------------------------------------------------- */
/*              Data transfers                           */
/* ----------------------------------------------------- */

/** A transfer mapping */
typedef enum {
    ADM_MAPPING_ONE_TO_ONE,
    ADM_MAPPING_ONE_TO_N,
    ADM_MAPPING_N_TO_N
} ADM_transfer_mapping_t;

/** A handle to a created transfer */
typedef struct adm_transfer* ADM_transfer_t;

/** A transfer priority */
typedef int ADM_transfer_priority_t;


/* ----------------------------------------------------- */
/*              Data operations                          */
/* ----------------------------------------------------- */

/** A data operation */
typedef struct adm_data_operation* ADM_data_operation_t;

/** A data operation status */
typedef struct {
    // TODO: empty for now
    uint32_t placeholder;
} ADM_data_operation_status_t;


/******************************************************************************/
/* Public type-related functions                                              */
/******************************************************************************/

/* ----------------------------------------------------- */
/*              General library types                    */
/* ----------------------------------------------------- */

/**
 * Initialize a server from a user-provided name/address.
 *
 * @remark Servers need to be freed by calling ADM_server_destroy().
 *
 * @param[in] protocol The protocol that will be used to access the server.
 * @param[in] address The address of server.
 * @return A valid ADM_server_t if successful or NULL in case of failure.
 */
ADM_server_t
ADM_server_create(const char* protocol, const char* address);

/**
 * Destroy a server created by ADM_server_create().
 *
 * @param[in] server A pointer to a ADM_server_t
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_server_destroy(ADM_server_t server);

/**
 * Initialize a node from a user-provided hostname/address.
 *
 * @remark Nodes need to be freed by calling ADM_server_destroy().
 *
 * @param[in] hostname The hostname of the node.
 * @return A valid ADM_server_t if successful or NULL in case of failure.
 */
ADM_node_t
ADM_node_create(const char* hostname, ADM_node_type_t type);

/**
 * Destroy a node created by ADM_node_create().
 *
 * @param[in] node A valid ADM_node_t
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_node_destroy(ADM_node_t node);

/**
 * Create a node list from an array of ADM_NODEs and its
 * length.
 *
 * @remark node lists need to be freed by calling ADM_node_list_destroy().
 *
 * @param[in] datasets The array of nodes.
 * @param[in] len The length of the array.
 * @return A valid ADM_node_list_t if successful or NULL in case of
 * failure.
 */
ADM_node_list_t
ADM_node_list_create(ADM_node_t nodes[], size_t len);

/**
 * Destroy a node list created by ADM_node_list_create().
 *
 * @param[in] list A valid ADM_node_list_t
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_node_list_destroy(ADM_node_list_t list);

/* ----------------------------------------------------- */
/*              Jobs                                     */
/* ----------------------------------------------------- */

/**
 * Create an ADM_JOB_RESOURCES from information about storage resources.
 *
 * @remark ADM_JOB_RESOURCES need to be freed by calling
 * ADM_job_resources_destroy().
 *
 * @param[in] nodes An array of ADM_NODES describing the nodes assigned
 * by the job_storage.
 * @param[in] nodes_len The number of ADM_NODES stored in nodes.
 *
 * @return A valid ADM_JOB_RESOURCES, or NULL in case of failure
 */
ADM_job_resources_t
ADM_job_resources_create(ADM_node_t nodes[], size_t nodes_len);

/**
 * Destroy a ADM_JOB_RESOURCES created by ADM_job_resources_create().
 *
 * @param[in] res A valid ADM_JOB_RESOURCES
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_job_resources_destroy(ADM_job_resources_t res);

/**
 * Create a JOB_REQUIREMENTS from user-provided information.
 *
 * @remark JOB_REQUIREMENTS created by this function need to be freed by calling
 * ADM_job_requirements_destroy().
 *
 * @param[in] inputs An array of DATASET_DESCRIPTORS describing the input
 * information required by the job.
 * @param[in] inputs_len The number of DATASET_DESCRIPTORS stored in inputs.
 * @param[in] outputs An array of DATASET_DESCRIPTORS describing the output
 * information generated by the job.
 * @param[in] outputs_len The number of DATASET_DESCRIPTORS stored in outputs.
 * @param[in] adhoc_storage An optional ADHOC_DESCRIPTOR describing the adhoc
 * storage system required by the job (can be set to NULL if no adhoc storage
 * system is required).
 * @return A valid ADM_job_requirements_t if sucessfull or NULL in case of
 * failure.
 */
ADM_job_requirements_t
ADM_job_requirements_create(ADM_dataset_t inputs[], size_t inputs_len,
                            ADM_dataset_t outputs[], size_t outputs_len,
                            ADM_adhoc_storage_t adhoc_storage);

/**
 * Destroy a ADM_job_requirements_t created by ADM_job_requirements_create().
 *
 * @param[in] reqs The ADM_job_requirements_t to destroy.
 * @return ADM_SUCCESS or corresponding error code.
 */
ADM_return_t
ADM_job_requirements_destroy(ADM_job_requirements_t reqs);


/* ----------------------------------------------------- */
/*              Datasets                                 */
/* ----------------------------------------------------- */

/**
 * Create a dataset from a user-provided id (e.g. a path for POSIX-like file
 * systems or key for key-value stores).
 *
 * @remark Datasets need to be freed by calling ADM_dataset_destroy().
 *
 * @param[in] id The id for the dataset.
 * @return A valid ADM_dataset_handle_t if successful or NULL in case of
 * failure.
 */
ADM_dataset_t
ADM_dataset_create(const char* id);

/**
 * Destroy a dataset created by ADM_dataset_create().
 *
 * @param[in] dataset A valid ADM_dataset_t
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_dataset_destroy(ADM_dataset_t dataset);

/**
 * Create a dataset from a user-provided id (e.g. a path for POSIX-like file
 * systems or key for key-value stores).
 *
 * @remark Datasets need to be freed by calling ADM_dataset_info_destroy().
 *
 * @return A valid ADM_DATASET_INFO if successful or NULL in case of
 * failure.
 */
ADM_dataset_info_t
ADM_dataset_info_create();

/**
 * Destroy a dataset created by ADM_dataset_info_create().
 *
 * @param[in] dataset A valid ADM_dataset_info_t
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_dataset_info_destroy(ADM_dataset_info_t dataset_info);

/**
 * Create a dataset list from an array of ADM_DATASETs and its
 * length.
 *
 * @remark Dataset lists need to be freed by calling ADM_dataset_list_destroy().
 *
 * @param[in] datasets The array of datasets.
 * @param[in] len The length of the array.
 * @return A valid ADM_dataset_list_t if successful or NULL in case of
 * failure.
 */
ADM_dataset_list_t
ADM_dataset_list_create(ADM_dataset_t datasets[], size_t len);

/**
 * Destroy a dataset list created by ADM_dataset_list_create().
 *
 * @param[in] list A valid ADM_dataset_list_t
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_dataset_list_destroy(ADM_dataset_list_t list);


/* ----------------------------------------------------- */
/*              Storage tiers                            */
/* ----------------------------------------------------- */

/**
 * Create a ADM_ADHOC_STORAGE to represent an adhoc storage tier.
 *
 * @remark ADM_ADHOC_STORAGEs need to be freed by calling
 * ADM_adhoc_storage_destroy().
 *
 * @param[in] name A name for the storage tier
 * @param[in] type The type for the storage tier being created.
 * @param[in] id The assigned id for the storage tier being created.
 * @param[in] adhoc_ctx Some specific context information for the storage
 * tier or NULL if none is required. For instance, an adhoc storage system may
 * find it useful to provide an ADM_adhoc_context_t describing the instance.
 * @param[in] adhoc_resources The adhoc resources for this instance.
 * @return A valid ADM_ADHOC_STORAGE if successful, or NULL in case of failure.
 */
ADM_adhoc_storage_t
ADM_adhoc_storage_create(const char* name, ADM_adhoc_storage_type_t type,
                         uint64_t id, ADM_adhoc_context_t adhoc_ctx,
                         ADM_adhoc_resources_t adhoc_resources);

/**
 * Destroy an ADM_ADHOC_STORAGE created by ADM_adhoc_storage_destroy().
 *
 * @param[in] storage A valid ADM_STORAGE
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_adhoc_storage_destroy(ADM_adhoc_storage_t adhoc_storage);

/**
 * Create an ADM_ADHOC_RESOURCES from information about storage resources.
 *
 * @remark ADM_ADHOC_RESOURCES need to be freed by calling
 * ADM_adhoc_resources_destroy().
 *
 * @param[in] nodes An array of ADM_NODES describing the nodes assigned
 * by the adhoc_storage.
 * @param[in] nodes_len The number of ADM_NODES stored in nodes.
 *
 * @return A valid ADM_ADHOC_RESOURCES, or NULL in case of failure
 */
ADM_adhoc_resources_t
ADM_adhoc_resources_create(ADM_node_t nodes[], size_t nodes_len);

/**
 * Destroy a ADM_ADHOC_RESOURCES created by ADM_adhoc_resources_create().
 *
 * @param[in] res A valid ADM_ADHOC_RESOURCES
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_adhoc_resources_destroy(ADM_adhoc_resources_t res);

/**
 * Create an ADM_ADHOC_CONTEXT from information about how an adhoc storage
 * instance should be executed.
 *
 * @remark ADM_ADHOC_CONTEXTs need to be freed by calling
 * ADM_adhoc_context_destroy().
 *
 * @param[in] ctl_address The address of the control node for the
 * adhoc storage system
 * @param[in] exec_mode The adhoc storage system execution mode
 * @param[in] access_type The adhoc storage system execution type
 * @param[in] walltime The adhoc storage system walltime
 * @param[in] should_flush Whether the adhoc storage system should flush data in
 * the background
 * @return A valid ADM_ADHOC_CONTEXT if successful. NULL otherwise.
 */
ADM_adhoc_context_t
ADM_adhoc_context_create(const char* ctl_address,
                         ADM_adhoc_mode_t exec_mode,
                         ADM_adhoc_access_t access_type, uint32_t walltime,
                         bool should_flush);

/**
 * Destroy an ADM_ADHOC_CONTEXT created by ADM_adhoc_context_create().
 *
 * @param[in] ctx A valid ADM_ADHOC_CONTEXT
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_adhoc_context_destroy(ADM_adhoc_context_t ctx);

/**
 * Create a ADM_PFS_STORAGE to represent a PFS storage tier.
 *
 * @remark ADM_PFS_STORAGEs need to be freed by calling
 * ADM_pfs_storage_destroy().
 *
 * @param[in] name A name for the storage tier
 * @param[in] type The type for the storage tier being created.
 * @param[in] id The assigned id for the storage tier being created.
 * @param[in] pfs_ctx Some specific context information for the storage
 * tier or NULL if none is required. For instance, an adhoc storage system may
 * find it useful to provide an ADM_adhoc_context_t describing the instance.
 * @return A valid ADM_PFS_STORAGE if successful, or NULL in case of failure.
 */
ADM_pfs_storage_t
ADM_pfs_storage_create(const char* name, ADM_pfs_storage_type_t type,
                       uint64_t id, ADM_pfs_context_t pfs_ctx);

/**
 * Destroy an ADM_ADHOC_STORAGE created by ADM_adhoc_storage_destroy().
 *
 * @param[in] storage A valid ADM_STORAGE
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_pfs_storage_destroy(ADM_pfs_storage_t pfs_storage);

/**
 * Create an ADM_PFS_CONTEXT from information about how a PFS is configured.
 *
 * @remark ADM_PFS_CONTEXTs need to be freed by calling
 * ADM_pfs_context_destroy().
 *
 * @param[in] mountpoint The PFS mount point
 * @return A valid ADM_PFS_CONTEXT if successful. NULL otherwise.
 */
ADM_pfs_context_t
ADM_pfs_context_create(const char* mountpoint);

/**
 * Destroy an ADM_PFS_CONTEXT created by ADM_pfs_context_create().
 *
 * @param[in] ctx A valid ADM_PFS_CONTEXT
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_pfs_context_destroy(ADM_pfs_context_t ctx);


/* ----------------------------------------------------- */
/*              Quality of Service                       */
/* ----------------------------------------------------- */

/**
 * Create a QoS entity given a scope, a node, a dataset, or a job.
 *
 * @remark QoS entities need to be freed by calling ADM_qos_entity_destroy().
 *
 * @param[in] scope The scope of the entity, i.e. ADM_QOS_SCOPE_DATASET,
 * ADM_QOS_SCOPE_NODE, or ADM_QOS_SCOPE_JOB.
 * @param[in] data A single argument with data from either a ADM_dataset_t,
 * ADM_node_t, or ADM_job_t variable. The argument must correspond properly
 * to the scope provided.
 * @return A valid ADM_qos_entity_t if successful or NULL in case of failure.
 */
ADM_qos_entity_t
ADM_qos_entity_create(ADM_qos_scope_t scope, void* data);

/**
 * Destroy a QoS entity created by ADM_qos_entity_create().
 *
 * @param[in] entity A valid ADM_qos_entity_t
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_qos_entity_destroy(ADM_qos_entity_t entity);

/**
 * Create a QoS limit given an entity and a QoS class.
 *
 * @remark QoS limits need to be freed by calling ADM_qos_limit_destroy().
 *
 * @param[in] entity The entity upon which the QoS limit should be enforced.
 * @param[in] cls The QoS restriction class to apply.
 * @param[in] value The limit's value.
 * @return A valid ADM_qos_limit_t if successful or NULL in case of failure.
 */
ADM_qos_limit_t
ADM_qos_limit_create(ADM_qos_entity_t entity, ADM_qos_class_t cls,
                     uint64_t value);

/**
 * Destroy a QoS limit created by ADM_qos_limit_create().
 *
 * @param[in] limit A valid ADM_qos_limit_t
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_qos_limit_destroy(ADM_qos_limit_t limit);

/**
 * Create a list of QoS limits from an array of ADM_QOS_LIMITs and its
 * length.
 *
 * @remark QoS limit lists need to be freed by calling
 * ADM_qos_limit_list_destroy().
 *
 * @param[in] limits The array of QoS limits.
 * @param[in] len The length of the array.
 * @return A valid ADM_qos_limit_list_t if successful or NULL in case of
 * failure.
 */
ADM_qos_limit_list_t
ADM_qos_limit_list_create(ADM_qos_limit_t limits[], size_t len);

/**
 * Destroy a QoS limit list created by ADM_qos_limit_list_create().
 *
 * @param[in] list A valid ADM_qos_limit_list_t
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_qos_limit_list_destroy(ADM_qos_limit_list_t list);


/* ----------------------------------------------------- */
/*              Data operations                          */
/* ----------------------------------------------------- */

/**
 * Create an ADM_DATA_OPERATION from information about storage resources.
 *
 * @remark ADM_DATA_OPERATION need to be freed by calling
 * ADM_adhoc_resources_destroy().
 *
 * @return A valid ADM_DATA_OPERATION, or NULL in case of failure
 */
ADM_data_operation_t
ADM_data_operation_create();

/**
 * Destroy a ADM_DATA_OPERATION created by ADM_storage_resources_create().
 *
 * @param[in] op A valid ADM_DATA_OPERATION
 * @return ADM_SUCCESS or corresponding ADM error code
 */
ADM_return_t
ADM_data_operation_destroy(ADM_data_operation_t op);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCORD_TYPES_H
