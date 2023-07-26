/******************************************************************************
 * Copyright 2022-2023, Inria, France.
 * Copyright 2023, Barcelona Supercomputing Center (BSC), Spain.
 * All rights reserved.
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

#ifndef SCORD_SLURM_PLUGIN_UTILS_H
#define SCORD_SLURM_PLUGIN_UTILS_H

#include <slurm/spank.h>
#include <scord/types.h>

hostlist_t
get_slurm_hostlist(spank_t sp);

typedef struct scord_nodelist {
    ADM_node_t* nodes;
    ssize_t nnodes;
}* scord_nodelist_t;

scord_nodelist_t
scord_nodelist_create(hostlist_t hostlist);

int
scord_nodelist_get_nodecount(scord_nodelist_t nodelist);

ADM_node_t*
scord_nodelist_get_nodes(scord_nodelist_t nodelist);

ADM_node_t
scord_nodelist_get_node(scord_nodelist_t nodelist, int index);

void
scord_nodelist_destroy(scord_nodelist_t nodelist);

const char*
margo_address_create(const char* protocol, const char* hostname, int port);

#endif // SCORD_SLURM_PLUGIN_UTILS_H
