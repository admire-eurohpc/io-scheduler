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
            tmp = (ADM_dataset_list_t) calloc(1, sizeof(struct adm_dataset_list));
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
