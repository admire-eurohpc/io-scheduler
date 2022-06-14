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

#ifndef SCORD_ADMIRE_H
#define SCORD_ADMIRE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADM_IOSCHED_API_VERSION       "0.1.0"
#define ADM_IOSCHED_API_VERSION_MAJOR 0
#define ADM_IOSCHED_API_VERSION_MINOR 1
#define ADM_IOSCHED_API_VERSION_PATCH 0

/**
 * Public type and struct definitions
 */

/* Error return codes */
enum ADM_return_t {
    ADM_SUCCESS = 0,
    // FIXME: generic error only for now
    ADM_OTHER_ERROR
};

/* A server */
typedef struct adm_server ADM_server_t;

/* A node */
typedef const char* ADM_node_t;


/* A dataset handle */
typedef struct adm_dataset* ADM_dataset_handle_t;

/* A job handle */
typedef struct adm_job* ADM_job_handle_t;

/* The scope affected by a QoS limit */
enum ADM_qos_scope_t {
    ADM_QOS_SCOPE_DATASET,
    ADM_QOS_SCOPE_NODE,
    ADM_QOS_SCOPE_JOB
};

/** The class of QoS limit applied to a scope */
enum ADM_qos_class_t { ADM_QOS_CLASS_BANDWIDTH, ADM_QOS_CLASS_IOPS };

/** An ADMIRE entity upon which QoS can be defined */
union ADM_qos_entity_t {
    ADM_node_t l_node;
    ADM_job_handle_t l_job;
    ADM_dataset_handle_t l_dataset;
};

/** A QoS limit */
typedef struct {
    // TODO: empty for now
    ADM_qos_scope_t l_scope;
    ADM_qos_class_t l_class;
    ADM_qos_entity_t l_element;
} ADM_limit_t;

/** A transfer mapping */
typedef enum {
    ADM_MAPPING_ONE_TO_ONE,
    ADM_MAPPING_ONE_TO_N,
    ADM_MAPPING_N_TO_N
} ADM_tx_mapping_t;

/** A handle to a created transfer */
typedef struct {
    // TODO: empty for now
} ADM_transfer_handle_t;

/** Information about a dataset */
typedef struct {
    // TODO: empty for now
} ADM_dataset_info_t;

/** A storage tier handle */
typedef struct {
    // TODO: empty for now
} ADM_storage_handle_t;

/** Information about resources assigned to a storage tier */
typedef struct {
    // TODO: empty for now
} ADM_storage_resources_t;

typedef int ADM_transfer_priority_t;

typedef struct {
    // TODO: empty for now
} ADM_data_operation_handle_t;

typedef struct {
    // TODO: empty for now
} ADM_data_operation_status_t;

typedef struct {
    // TODO: empty for now
} ADM_job_stats_t;

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
typedef struct {
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
} ADM_adhoc_context_t;

typedef ADM_adhoc_context_t* ADM_adhoc_storage_handle_t;

/** The I/O requirements for a job */
typedef struct {
    /** A list of input datasets */
    ADM_dataset_handle_t** r_inputs;
    /** A list of output datasets */
    ADM_dataset_handle_t** r_outputs;
    /** A definition for a specific adhoc storage instance */
    ADM_adhoc_storage_handle_t r_adhoc_storage;
} ADM_job_requirements_t;


/******************************************************************************/
/* Public prototypes                                                          */
/******************************************************************************/

/**
 * Register a job and its requirements.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] reqs The requirements for the job.
 * @param[out] job An ADHOC_HANDLE referring to the newly-created
 * adhoc storage instance.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_register_job(ADM_server_t server, ADM_job_requirements_t reqs,
                 ADM_job_handle_t* job);

ADM_return_t
ADM_update_job(ADM_server_t server, ADM_job_handle_t job,
               ADM_job_requirements_t reqs);

ADM_return_t
ADM_remove_job(ADM_server_t server, ADM_job_handle_t job);

/**
 * Register an adhoc storage system.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] ctx The EXECUTION_CONTEXT for the adhoc storage system.
 * @param[out] adhoc_handle An ADHOC_HANDLE referring to the newly-created
 * adhoc storage instance.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_register_adhoc_storage(ADM_server_t server, ADM_job_handle_t job,
                           ADM_adhoc_context_t ctx,
                           ADM_adhoc_storage_handle_t* adhoc_handle);

/**
 * Update an already-registered adhoc storage system.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] ctx The updated EXECUTION_CONTEXT for the adhoc storage system.
 * @param[in] adhoc_handle An ADHOC_HANDLE referring to the adhoc storage
 * instance of interest.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_update_adhoc_storage(ADM_server_t server, ADM_job_handle_t job,
                         ADM_adhoc_context_t ctx,
                         ADM_adhoc_storage_handle_t adhoc_handle);

/**
 * Remove an already-registered adhoc storage system.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] adhoc_handle An ADHOC_HANDLE referring to the adhoc storage
 * instance of interest.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_remove_adhoc_storage(ADM_server_t server, ADM_job_handle_t job,
                         ADM_adhoc_storage_handle_t adhoc_handle);

/**
 * Initiate the deployment of an adhoc storage system instance.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] adhoc_handle An ADHOC_HANDLE referring to the adhoc storage
 * instance of interest.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_deploy_adhoc_storage(ADM_server_t server, ADM_job_handle_t job,
                         ADM_adhoc_storage_handle_t adhoc_handle);

/**
 * Transfers the dataset identified by the source_name to the storage tier
 * defined by destination_name, and apply the provided constraints during the
 * transfer. This function returns a handle that can be used to track the
 * operation (i.e., get statistics, or status).
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] sources A list of DATASETs identifying the source dataset/s
 * to be transferred.
 * @param[in] targets A list of DATASETs identifying the destination
 * dataset/s and its/their desired locations in a storage tier.
 * @param[in] limits A list of QOS_CONSTRAINTS that must be applied to
 * the transfer. These may not exceed the global ones set at node, application,
 * or resource level.
 * @param[in] mapping A distribution strategy for the transfers (e.g.
 * ONE_TO_ONE, ONE_TO_MANY, MANY_TO_MANY)
 * @param[out] transfer_handle A TRANSFER_HANDLE allowing clients to interact
 * with the transfer (e.g. wait for its completion, query its status, cancel it,
 * etc.
 * @return Returns if the remote procedure has been completed
 * successfully or not.
 */
ADM_return_t
ADM_transfer_dataset(ADM_server_t server, ADM_job_handle_t job,
                     ADM_dataset_handle_t** sources,
                     ADM_dataset_handle_t** targets, ADM_limit_t** limits,
                     ADM_tx_mapping_t mapping,
                     ADM_transfer_handle_t* tx_handle);


/**
 * Sets information for the dataset identified by resource_id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] target A DATASET_HANDLE referring to the dataset of interest.
 * @param[in] info A DATASET_INFO with information about the
 * dataset (e.g. its lifespan, access methods, intended usage, etc.).
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_set_dataset_information(ADM_server_t server, ADM_job_handle_t job,
                            ADM_dataset_handle_t target,
                            ADM_dataset_info_t info);

/**
 * Changes the I/O resources used by a storage tier, typically an Ad hoc Storage
 * System.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] tier A STORAGE_HANDLE referring to the target storage tier.
 * @param[in] resources A RESOURCES argument containing information
 * about the I/O resources to set (e.g. number of I/O nodes.).
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_set_io_resources(ADM_server_t server, ADM_job_handle_t job,
                     ADM_storage_handle_t tier,
                     ADM_storage_resources_t resources);


/**
 * Returns the priority of the pending transfer identified by transfer_id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] tx_handle A TRANSFER_HANDLE referring to a pending transfer
 * @param[out] priority The priority of the pending transfer or an error code if
 * it didn’t exist or is no longer pending.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_get_transfer_priority(ADM_server_t server, ADM_job_handle_t job,
                          ADM_transfer_handle_t tx_handle,
                          ADM_transfer_priority_t* priority);


/**
 * Moves the operation identified by transfer_id up or down by n positions in
 * its scheduling queue.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] tx_handle A TRANSFER_HANDLE referring to a pending transfer
 * @param[in] incr A positive or negative number for the number of
 * positions the transfer should go up or down in its scheduling queue.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_set_transfer_priority(ADM_server_t server, ADM_job_handle_t job,
                          ADM_transfer_handle_t tx_handle, int incr);


/**
 * Cancels the pending transfer identified by transfer_id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] tx_handle A TRANSFER_HANDLE referring to a pending transfer.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_cancel_transfer(ADM_server_t server, ADM_job_handle_t job,
                    ADM_transfer_handle_t tx_handle);


/**
 * Returns a list of pending transfers.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[out] pending_transfers  A list of pending_transfers.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_get_pending_transfers(ADM_server_t server, ADM_job_handle_t job,
                          ADM_transfer_handle_t** pending_transfers);


/**
 * Registers a QoS constraint defined by class, scope, and value for the element
 * identified by id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] limit A QOS_LIMIT specifying at least:
 *                  - The QOS_SCOPE the limit  should be applied to: e.g.
 *                    dataset, node, or job.
 *                  - The QOS_CLASS of the limit (e.g. "bandwidth", "iops",
 *                  etc.).
 *                  - The QOS_ENTITY it should be applied to (e.g. job, node,
 *                  dataset, etc.)
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_set_qos_constraints(ADM_server_t server, ADM_job_handle_t job,
                        ADM_limit_t limit);


/**
 * Returns a list of QoS constraints defined for an element identified for id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] scope The scope being queried: dataset, node, or job.
 * @param[in] entity An QOS_ENTITY referring to the target of the query, i.e. a
 * RESOURCE_HANDLE, a NODE hostname, or a JOB_HANDLE.
 * @param[in] limits A list of QOS_LIMITS that includes all the classes
 * currently defined for the element as well as the values set for them.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_get_qos_constraints(ADM_server_t server, ADM_job_handle_t job,
                        ADM_qos_scope_t scope, ADM_qos_entity_t entity,
                        ADM_limit_t** limits);


/**
 * Defines a new operation, with the code found in path. The code will be
 * identified by the user-provided operation_id and will accept the arguments
 * defined, using the next format "arg0, arg1, arg2, ... ".
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] path A valid path for the operation executable.
 * @param[in] ... A list of ARGUMENTS for the operation.
 * @param[out] op  An OPERATION_HANDLE for the newly-defined operation.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_define_data_operation(ADM_server_t server, ADM_job_handle_t job,
                          const char* path, ADM_data_operation_handle_t* op,
                          ...);


/**
 * Connects and starts the data operation referred to by OPERATION_HANDLE and
 * with the arguments, using the input and output data storage (i.e., files). If
 * the operation can be executed in a streaming fashion (i.e., it can start even
 * if the input data is not entirely available), the stream parameter must be
 * set to true.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] op The OPERATION_HANDLE of the operation to be connected.
 * @param[in] input An input DATASET_HANDLE for the operation.
 * @param[in] output An output DATASET_HANDLE where the result of
 * the operation should be stored.
 * @param[in] should_stream A boolean indicating if the operation
 * should be executed in a streaming fashion.
 * @param[in] ... The VALUES for the arguments required by the operation.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_connect_data_operation(ADM_server_t server, ADM_job_handle_t job,
                           ADM_dataset_handle_t input,
                           ADM_dataset_handle_t output, bool should_stream,
                           ...);


/**
 * Finalises the operation defined with operation_id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] op The OPERATION_HANDLE of the operation to be connected.
 * @return[out] status An OPERATION_STATUS type indicating whether the
 * operation was successful.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_finalize_data_operation(ADM_server_t server, ADM_job_handle_t job,
                            ADM_data_operation_handle_t op,
                            ADM_data_operation_status_t* status);


/**
 * Links the data operation defined with operation_id with the pending transfer
 * identified by transf er_id using the values provided as arguments. If the
 * operation can be executed in a streaming fashion (i.e., it can start even if
 * the input data is not entirely available), the stream parameter must be set
 * to true.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] op The OPERATION_HANDLE of the operation to be connected.
 * @param[in] tx_handle The TRANSFER_HANDLE referring to the pending transfer
 * the operation should be linked to.
 * @param[in] job A JOB_HANDLE identifying the originating job.
 * @param[in] should_stream A boolean indicating whether the operation
 * should be executed in a streaming fashion.
 * @param[in] ... The VALUES for the arguments required by the operation.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_link_transfer_to_data_operation(ADM_server_t server, ADM_job_handle_t job,
                                    ADM_data_operation_handle_t op,
                                    bool should_stream, ...);


/**
 * Returns the current I/O statistics for a specified job_id and an optional
 * corresponding job_step. The information will be returned in an
 * easy-to-process format, e.g., JSON (see Listing 3.1).
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job A JOB_HANDLE identifying the originating job and,
 * optionally, its JOB_STEP.
 * @return[out] stats A list of JOB_STATS.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_get_statistics(ADM_server_t server, ADM_job_handle_t job,
                   ADM_job_stats_t** stats);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCORD_ADMIRE_H