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

#include <scord/scord-user.h>
#include "types_private.h"

ADM_return_t
ADM_transfer_datasets(ADM_dataset_t sources[], size_t sources_len,
                      ADM_dataset_t targets[], size_t targets_len,
                      ADM_transfer_t* transfer) {
    (void) sources;
    (void) sources_len;
    (void) targets;
    (void) targets_len;
    (void) transfer;

    return ADM_SUCCESS;
}


ADM_return_t
ADM_transfer_wait(ADM_transfer_t transfer, ADM_transfer_status_t* status,
                  struct timespec* timeout) {
    (void) transfer;
    (void) status;
    (void) timeout;

    if(transfer == NULL || status == NULL) {
        return ADM_EBADARGS;
    }

    *status = malloc(sizeof(struct adm_transfer_status));

    if(*status == NULL) {
        return ADM_ENOMEM;
    }

    (*status)->s_state = ADM_TRANSFER_FINISHED;
    (*status)->s_error = ADM_SUCCESS;

    return ADM_SUCCESS;
}

inline bool
__adm_transfer_succeeded(ADM_transfer_status_t st) {
    return st->s_state == ADM_TRANSFER_FINISHED && st->s_error == ADM_SUCCESS;
}

inline bool
__adm_transfer_failed(ADM_transfer_status_t st) {
    return st->s_state == ADM_TRANSFER_FINISHED && st->s_error != ADM_SUCCESS;
}

inline bool
__adm_transfer_pending(ADM_transfer_status_t st) {
    return st->s_state == ADM_TRANSFER_QUEUED;
}

inline bool
__adm_transfer_in_progress(ADM_transfer_status_t st) {
    return st->s_state == ADM_TRANSFER_RUNNING;
}

inline ADM_return_t
__adm_transfer_error(ADM_transfer_status_t st) {
    return st->s_error;
}
