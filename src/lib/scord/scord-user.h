/******************************************************************************
 * Copyright 2021-2023, Barcelona Supercomputing Center (BSC), Spain
 *
 * This software was partially supported by the EuroHPC-funded project ADMIRE
 *   (Project ID: 956748, https://www.admire-eurohpc.eu).
 *
 * This file is part of scord.
 *
 * scord is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * scord is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with scord.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *****************************************************************************/


#ifndef SCORD_USER_H
#define SCORD_USER_H

#include <time.h>
#include <scord/types.h>

typedef struct adm_transfer_status* ADM_transfer_status_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Transfer datasets between storage tiers asynchronously.
 *
 * @param[in] sources The datasets to transfer.
 * @param[in] sources_len The number of datasets to transfer.
 * @param[in] targets The destination datasets.
 * @param[in] targets_len The number of destination datasets.
 * @param[out] transfer A transfer handle to query the operation status.
 * @return
 */
ADM_return_t
ADM_transfer_datasets(ADM_dataset_t sources[], size_t sources_len,
                      ADM_dataset_t targets[], size_t targets_len,
                      ADM_transfer_t* transfer);


/**
 * @brief Wait for a transfer to complete.
 *
 * @param[in] transfer The transfer handle.
 * @param[out] status A pointer to a dynamically allocated
 * `ADM_transfer_status_t` structure where status information should be stored.
 * The caller is responsible for freeing the memory allocated for this
 * structure.
 * @param[out] timeout The maximum time to wait for the transfer to complete. If
 * NULL, query the transfer status and return immediately. If not NULL, wait for
 * the transfer to complete or the timeout to expire.
 * @return ADM_SUCCESS if the transfer completed successfully. ADM_ETIMEOUT if
 * the transfer did not complete before the timeout expired. An
 * specifc ADM_E* error code otherwise.
 */
ADM_return_t
ADM_transfer_wait(ADM_transfer_t transfer, ADM_transfer_status_t* status,
                  struct timespec* timeout);

bool
__adm_transfer_succeeded(ADM_transfer_status_t status);

bool
__adm_transfer_failed(ADM_transfer_status_t status);

bool
__adm_transfer_pending(ADM_transfer_status_t status);

bool
__adm_transfer_in_progress(ADM_transfer_status_t status);

ADM_return_t
__adm_transfer_error(ADM_transfer_status_t status);

/**
 * @brief Check if a transfer completed successfully.
 */
#define ADM_TRANSFER_SUCCEEDED(st) __adm_transfer_succeeded(st)

/**
 * @brief Check if a transfer failed.
 */
#define ADM_TRANSFER_FAILED(st) __adm_transfer_failed(st)

/**
 * @brief Check if a transfer is pending.
 */
#define ADM_TRANSFER_PENDING(st) __adm_transfer_pending(st)

/**
 * @brief Check if a transfer is in progress.
 */
#define ADM_TRANSFER_IN_PROGRESS(st) __adm_transfer_in_progress(st)

/**
 * @brief Return the error code of a failed transfer.
 */
#define ADM_TRANSFER_ERROR(st) __adm_transfer_error(st)

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

#endif // SCORD_USER_H
