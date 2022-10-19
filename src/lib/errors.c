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

#include <admire.h>

const char* const adm_errlist[ADM_ERR_MAX + 1] = {
        [ADM_SUCCESS] = "Success",
        [ADM_ESNAFU] = "Internal error",
        [ADM_EBADARGS] = "Bad arguments",
        [ADM_ENOMEM] = "Cannot allocate memory",
        [ADM_EEXISTS] = "Entity already exists",
        [ADM_ENOENT] = "Entity does not exist",
        [ADM_EADHOC_BUSY] = "Adhoc storage already in use",

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
