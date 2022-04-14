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