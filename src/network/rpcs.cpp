/******************************************************************************
 * Copyright 2021, Barcelona Supercomputing Center (BSC), Spain
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

#include "rpcs.hpp"

static void
ping(hg_handle_t h) {

    hg_return_t ret;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    LOGGER_INFO("remote_procedure::PING(noargs)");

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ping);

static void 
ADM_input(hg_handle_t h){
    hg_return_t ret;

    ADM_input_in_t in;
    ADM_input_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_input");
    LOGGER_INFO("remote_procedure::ADM_input({},{})",
                in.origin, in.target);

    if (in.origin!=nullptr && in.target!=nullptr){
       out.ret = true;
        LOGGER_INFO("remote_procedure::ADM_input not null ({},{})",
                in.origin, in.target);
    }
    else {
       out.ret = false;
       LOGGER_INFO("remote_procedure::ADM_input null ({},{})",
                in.origin, in.target);
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_input)

static void 
ADM_output(hg_handle_t h){
    hg_return_t ret;

    ADM_output_in_t in;
    ADM_output_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_output");
    LOGGER_INFO("remote_procedure::ADM_output({},{})",
                in.origin, in.target);

    if (in.origin!=nullptr && in.target!=nullptr){
       out.ret = true;
        LOGGER_INFO("remote_procedure::ADM_output not null ({},{})",
                in.origin, in.target);
    }
    else {
       out.ret = false;
       LOGGER_INFO("remote_procedure::ADM_output null ({},{})",
                in.origin, in.target);
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_output)

static void 
ADM_inout(hg_handle_t h){
    hg_return_t ret;

    ADM_inout_in_t in;
    ADM_inout_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_inout");
    LOGGER_INFO("remote_procedure::ADM_inout({},{})",
                in.origin, in.target);

    if (in.origin!=nullptr && in.target!=nullptr){
       out.ret = true;
        LOGGER_INFO("remote_procedure::ADM_inout not null ({},{})",
                in.origin, in.target);
    }
    else {
       out.ret = false;
       LOGGER_INFO("remote_procedure::ADM_inout null ({},{})",
                in.origin, in.target);
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_inout)


static void 
ADM_adhoc_nodes(hg_handle_t h){
    hg_return_t ret;

    ADM_adhoc_nodes_in_t in;
    ADM_adhoc_nodes_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_adhoc_nodes");
    LOGGER_INFO("remote_procedure::ADM_adhoc_nodes({})",
                in.nodes);

    if (in.nodes!=nullptr){
       out.ret = true;
        LOGGER_INFO("remote_procedure::ADM_adhoc_nodes not null ({})",
                in.nodes);
    }
    else {
       out.ret = false;
       LOGGER_INFO("remote_procedure::ADM_adhoc_nodes null ({})",
                in.nodes);
    }

    /*Specifies the number of nodes for the Ad hoc Storage System. If the
    ADM_adhoc_execution_mode is shared, the number cannot exceed the number of allocated
    nodes within the compute job. If the ADM_adhoc_execution_mode is dedicated, the number
    of nodes is not restricted. Should this be checked now? */

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_adhoc_nodes)


static void 
ADM_adhoc_access(hg_handle_t h){
    hg_return_t ret;

    ADM_adhoc_access_in_t in;
    ADM_adhoc_access_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_adhoc_access");
    LOGGER_INFO("remote_procedure::ADM_adhoc_access({})",
                in.access);

    if (in.access!=nullptr){
       out.ret = true;
        LOGGER_INFO("remote_procedure::ADM_adhoc_access not null ({})",
                in.access);
    }
    else {
       out.ret = false;
       LOGGER_INFO("remote_procedure::ADM_adhoc_access null or invalid ({}). Please use",
                in.access);
    }

    if (in.access == "write-only" || in.access == "read-only" || in.access == "read-write"){
       out.ret = true;
        LOGGER_INFO("remote_procedure::ADM_adhoc_access value is acceptable ({})",
                in.access);
    }
    else {
       out.ret = false;
       LOGGER_INFO("remote_procedure::ADM_adhoc_access is not valid. Please use: write-only, read-only or read-write",
                in.access);
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_adhoc_access)
