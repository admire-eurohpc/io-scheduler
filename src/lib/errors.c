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

#include "scord/types.h"

const char* const adm_errlist[ADM_ERR_MAX + 1] = {
        [ADM_SUCCESS] = "Success",
        [ADM_ESNAFU] = "Internal error",
        [ADM_EBADARGS] = "Bad arguments",
        [ADM_ENOMEM] = "Cannot allocate memory",
        [ADM_EEXISTS] = "Entity already exists",
        [ADM_ENOENT] = "Entity does not exist",
        [ADM_EADHOC_BUSY] = "Adhoc storage already in use",
        [ADM_EADHOC_TYPE_UNSUPPORTED] = "Unsupported adhoc storage type",
        [ADM_EADHOC_DIR_CREATE_FAILED] =
                "Cannot create adhoc storage directory",
        [ADM_EADHOC_DIR_EXISTS] = "Adhoc storage directory already exists",
        [ADM_ESUBPROCESS_ERROR] = "Subprocess error",
        [ADM_ETIMEOUT] = "Timeout",
        [ADM_EOTHER] = "Undetermined error",

        /* fallback */
        [ADM_ERR_MAX] = "Unknown error",

};

const char*
ADM_strerror(ADM_return_t errnum) {

    if(errnum > ADM_ERR_MAX) {
        errnum = ADM_ERR_MAX;
    }

    const char* s = adm_errlist[errnum];
    return s ? s : adm_errlist[ADM_EOTHER];
}
