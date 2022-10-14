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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "admire_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADM_IOSCHED_API_VERSION       "0.1.0"
#define ADM_IOSCHED_API_VERSION_MAJOR 0
#define ADM_IOSCHED_API_VERSION_MINOR 1
#define ADM_IOSCHED_API_VERSION_PATCH 0


/******************************************************************************/
/* Public type definitions and type-related functions                         */
/******************************************************************************/

// See admire_types.h


/******************************************************************************/
/* Public prototypes                                                          */
/******************************************************************************/

/**
 * Send an RPC to a server to check if it's online.
 *
 * @param[in] server The server to which the request is directed
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_ping(ADM_server_t server);

/**
 * Register a job and its requirements.
 *
 * @remark The returned ADM_JOB will be freed when passed to
 * ADM_remove_job().
 *
 * @param[in] server The server to which the request is directed
 * @param[in] res The resources for the job.
 * @param[in] reqs The requirements for the job.
 * @param[out] job An ADM_JOB referring to the newly-registered job.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_register_job(ADM_server_t server, ADM_job_resources_t res,
                 ADM_job_requirements_t reqs, ADM_job_t* job);

ADM_return_t
ADM_update_job(ADM_server_t server, ADM_job_t job,
               ADM_job_resources_t job_resources, ADM_job_requirements_t reqs);

ADM_return_t
ADM_remove_job(ADM_server_t server, ADM_job_t job);

/**
 * Register an adhoc storage system.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] name The desired name for the adhoc storage system.
 * @param[in] type The desired type for the adhoc storage system.
 * @param[in] ctx The EXECUTION_CONTEXT for the adhoc storage system.
 * @param[out] adhoc_storage An ADM_STORAGE referring to the newly-created
 * adhoc storage instance.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_register_adhoc_storage(ADM_server_t server, const char* name,
                           ADM_storage_type_t type, ADM_adhoc_context_t ctx,
                           ADM_storage_t* adhoc_storage);

/**
 * Update an already-registered adhoc storage system.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] ctx The updated EXECUTION_CONTEXT for the adhoc storage system.
 * @param[in] adhoc_storage An ADM_STORAGE referring to the adhoc storage
 * instance of interest.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_update_adhoc_storage(ADM_server_t server, ADM_adhoc_context_t ctx,
                         ADM_storage_t adhoc_storage);

/**
 * Remove an already-registered adhoc storage system.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] adhoc_storage An ADM_STORAGE referring to the adhoc storage
 * instance of interest.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_remove_adhoc_storage(ADM_server_t server, ADM_storage_t adhoc_storage);

/**
 * Initiate the deployment of an adhoc storage system instance.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] adhoc_storage An ADM_STORAGE referring to the adhoc storage
 * instance of interest.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_deploy_adhoc_storage(ADM_server_t server, ADM_storage_t adhoc_storage);

/**
 * Register a PFS storage tier.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] ctx The EXECUTION_CONTEXT for the PFS.
 * @param[out] adhoc_storage An ADM_STORAGE referring to the newly-created
 * PFS instance.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_register_pfs_storage(ADM_server_t server, ADM_pfs_context_t ctx,
                         ADM_storage_t* pfs_storage);

/**
 * Update an already-registered PFS storage tier.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] ctx The updated EXECUTION_CONTEXT for the PFS.
 * @param[in] adhoc_storage An ADM_STORAGE referring to the PFS
 * instance of interest.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_update_pfs_storage(ADM_server_t server, ADM_pfs_context_t ctx,
                       ADM_storage_t adhoc_storage);

/**
 * Remove an already-registered PFS storage tier.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] adhoc_storage An ADM_STORAGE referring to the PFS
 * instance of interest.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_remove_pfs_storage(ADM_server_t server, ADM_storage_t adhoc_storage);

/**
 * Transfers the dataset identified by the source_name to the storage tier
 * defined by destination_name, and apply the provided constraints during the
 * transfer. This function returns a handle that can be used to track the
 * operation (i.e., get statistics, or status).
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] sources An array of DATASETs identifying the source dataset/s
 * to be transferred.
 * @param[in] sources_len The number of DATASETs stored in sources.
 * @param[in] targets An array of DATASETs identifying the destination
 * dataset/s and its/their desired locations in a storage tier.
 * @param[in] targets_len The number of DATASETs stored in targets.
 * @param[in] limits An array of QOS_CONSTRAINTS that must be applied to
 * the transfer. These may not exceed the global ones set at node, application,
 * or resource level.
 * @param[in] limits_len The number of QOS_CONSTRAINTS stored in limits.
 * @param[in] mapping A distribution strategy for the transfers (e.g.
 * ONE_TO_ONE, ONE_TO_MANY, MANY_TO_MANY)
 * @param[out] transfer A ADM_TRANSFER allowing clients to interact
 * with the transfer (e.g. wait for its completion, query its status, cancel it,
 * etc.
 * @return Returns if the remote procedure has been completed
 * successfully or not.
 */
ADM_return_t
ADM_transfer_datasets(ADM_server_t server, ADM_job_t job,
                      ADM_dataset_t sources[], size_t sources_len,
                      ADM_dataset_t targets[], size_t targets_len,
                      ADM_qos_limit_t limits[], size_t limits_len,
                      ADM_transfer_mapping_t mapping, ADM_transfer_t* transfer);


/**
 * Sets information for the dataset identified by resource_id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] target A DATASET_HANDLE referring to the dataset of interest.
 * @param[in] info A DATASET_INFO with information about the
 * dataset (e.g. its lifespan, access methods, intended usage, etc.).
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_set_dataset_information(ADM_server_t server, ADM_job_t job,
                            ADM_dataset_t target, ADM_dataset_info_t info);

/**
 * Changes the I/O resources used by a storage tier, typically an Ad hoc Storage
 * System.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] tier A ADM_STORAGE_TIER referring to the target storage tier.
 * @param[in] resources A RESOURCES argument containing information
 * about the I/O resources to set (e.g. number of I/O nodes.).
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_set_io_resources(ADM_server_t server, ADM_job_t job, ADM_storage_t tier,
                     ADM_adhoc_resources_t resources);


/**
 * Returns the priority of the pending transfer identified by transfer_id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] transfer A ADM_TRANSFER referring to a pending transfer
 * @param[out] priority The priority of the pending transfer or an error code if
 * it didn’t exist or is no longer pending.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 * successfully.
 */
ADM_return_t
ADM_get_transfer_priority(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t transfer,
                          ADM_transfer_priority_t* priority);


/**
 * Moves the operation identified by transfer_id up or down by n positions in
 * its scheduling queue.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] transfer A ADM_TRANSFER referring to a pending transfer
 * @param[in] incr A positive or negative number for the number of
 * positions the transfer should go up or down in its scheduling queue.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_set_transfer_priority(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t transfer, int incr);


/**
 * Cancels the pending transfer identified by transfer_id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] transfer A ADM_TRANSFER referring to a pending transfer.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_cancel_transfer(ADM_server_t server, ADM_job_t job,
                    ADM_transfer_t transfer);


/**
 * Returns a list of pending transfers.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[out] pending_transfers  A list of pending_transfers.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_get_pending_transfers(ADM_server_t server, ADM_job_t job,
                          ADM_transfer_t** pending_transfers);


/**
 * Registers a QoS constraint defined by class, scope, and value for the element
 * identified by id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] entity An QOS_ENTITY referring to the target of the query, i.e. a
 * ADM_DATASET, a ADM_NODE, or a ADM_JOB.
 * @param[in] limit A QOS_LIMIT specifying:
 *                  - The QOS_CLASS of the limit (e.g. "bandwidth", "iops",
 *                  etc.).
 *                  - The VALUE it should be applied to (e.g. job, node,
 *                  dataset, etc.)
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_set_qos_constraints(ADM_server_t server, ADM_job_t job,
                        ADM_qos_entity_t entity, ADM_qos_limit_t limit);


/**
 * Returns a list of QoS constraints defined for an element identified for id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] entity An QOS_ENTITY referring to the target of the query, i.e. a
 * ADM_DATASET, a ADM_NODE, or a ADM_JOB.
 * @param[in] limits A NULL-terminated array of QOS_LIMITS that includes all the
 * classes currently defined for the element as well as the values set for them.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_get_qos_constraints(ADM_server_t server, ADM_job_t job,
                        ADM_qos_entity_t entity, ADM_qos_limit_t** limits);


/**
 * Defines a new operation, with the code found in path. The code will be
 * identified by the user-provided operation_id and will accept the arguments
 * defined, using the next format "arg0, arg1, arg2, ... ".
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] path A valid path for the operation executable.
 * @param[in] ... A list of ARGUMENTS for the operation.
 * @param[out] op  An OPERATION_HANDLE for the newly-defined operation.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_define_data_operation(ADM_server_t server, ADM_job_t job, const char* path,
                          ADM_data_operation_t* op, ...);


/**
 * Connects and starts the data operation referred to by OPERATION_HANDLE and
 * with the arguments, using the input and output data storage (i.e., files). If
 * the operation can be executed in a streaming fashion (i.e., it can start even
 * if the input data is not entirely available), the stream parameter must be
 * set to true.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
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
ADM_connect_data_operation(ADM_server_t server, ADM_job_t job,
                           ADM_dataset_t input, ADM_dataset_t output,
                           bool should_stream, ...);


/**
 * Finalises the operation defined with operation_id.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] op The OPERATION_HANDLE of the operation to be connected.
 * @return[out] status An OPERATION_STATUS type indicating whether the
 * operation was successful.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_finalize_data_operation(ADM_server_t server, ADM_job_t job,
                            ADM_data_operation_t op,
                            ADM_data_operation_status_t* status);


/**
 * Links the data operation defined with operation_id with the pending transfer
 * identified by transf er_id using the values provided as arguments. If the
 * operation can be executed in a streaming fashion (i.e., it can start even if
 * the input data is not entirely available), the stream parameter must be set
 * to true.
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] op The OPERATION_HANDLE of the operation to be connected.
 * @param[in] transfer The ADM_TRANSFER referring to the pending transfer
 * the operation should be linked to.
 * @param[in] job An ADM_JOB identifying the originating job.
 * @param[in] should_stream A boolean indicating whether the operation
 * should be executed in a streaming fashion.
 * @param[in] ... The VALUES for the arguments required by the operation.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_link_transfer_to_data_operation(ADM_server_t server, ADM_job_t job,
                                    ADM_data_operation_t op,
                                    ADM_transfer_t transfer, bool should_stream,
                                    ...);


/**
 * Returns the current I/O statistics for a specified job_id and an optional
 * corresponding job_step. The information will be returned in an
 * easy-to-process format, e.g., JSON (see Listing 3.1).
 *
 * @param[in] server The server to which the request is directed
 * @param[in] job An ADM_JOB identifying the originating job and,
 * optionally, its JOB_STEP.
 * @return[out] stats A list of JOB_STATS.
 * @return Returns ADM_SUCCESS if the remote procedure has completed
 */
ADM_return_t
ADM_get_statistics(ADM_server_t server, ADM_job_t job, ADM_job_stats_t** stats);

/**
 * Return a string describing the error number
 *
 * @param[in] errnum The error number for which a description should be
 * returned.
 * @return A pointer to a string describing `errnum`.
 */
const char*
ADM_strerror(ADM_return_t errnum);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCORD_ADMIRE_H
