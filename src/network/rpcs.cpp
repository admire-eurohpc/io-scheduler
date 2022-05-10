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

/**
 * Specifes the origin location in a storage tier where input is located, as
 * well as the target location where it should be placed in a different storage
 * tier.
 *
 * @param in.origin An origin location for the source dataset.
 * @param in.target A target location for the destination dataset.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_input(hg_handle_t h) {
    hg_return_t ret;

    ADM_input_in_t in;
    ADM_input_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_input");
    LOGGER_INFO("remote_procedure::ADM_input({},{})", in.origin, in.target);

    if(in.origin != nullptr && in.target != nullptr) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_input not null ({},{})", in.origin,
                    in.target);
    } else {
        out.ret = -1;
        LOGGER_INFO("remote_procedure::ADM_input null ({},{})", in.origin,
                    in.target);
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_input)

/**
 * Specifies the origin location in a storage tier where output is located, as
 * well as the target location where it should be placed in a different storage
 * tier.
 *
 * @param in.origin An origin location for the source dataset.
 * @param in.target A target location for the destination dataset.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_output(hg_handle_t h) {
    hg_return_t ret;

    ADM_output_in_t in;
    ADM_output_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_output");
    LOGGER_INFO("remote_procedure::ADM_output({},{})", in.origin, in.target);

    if(in.origin != nullptr && in.target != nullptr) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_output not null ({},{})", in.origin,
                    in.target);
    } else {
        out.ret = -1;
        LOGGER_INFO("remote_procedure::ADM_output null ({},{})", in.origin,
                    in.target);
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_output)

/**
 * Specifies both the input and output locations in a storage tier. This
 * combines both ADM_input and ADM_output for user convenience: the input data
 * provided by origin is overwritten by the output data generated at target.
 *
 * @param in.origin An origin location for the source dataset.
 * @param in.target A target location for the destination dataset.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_inout(hg_handle_t h) {
    hg_return_t ret;

    ADM_inout_in_t in;
    ADM_inout_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_inout");
    LOGGER_INFO("remote_procedure::ADM_inout({},{})", in.origin, in.target);

    if(in.origin != nullptr && in.target != nullptr) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_inout not null ({},{})", in.origin,
                    in.target);
    } else {
        out.ret = -1;
        LOGGER_INFO("remote_procedure::ADM_inout null ({},{})", in.origin,
                    in.target);
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_inout)

/**
 * Specifies the execution_mode an Ad hoc Storage System should use. Valid
 options: in_job:shared (run while
 * sharing the application’s compute nodes), in_job:dedicated (run using a
 subset of the application’s
 * compute nodes), separate:new (ask the system to allocate a separate job with
 separate runtime and
 * number of nodes) and separate:existing (ask the system to reuse an already
 running Ad hoc Storage System instance).
 * The number of nodes assigned for the Ad hoc Storage System must be specified
 with ADM_adhoc_nodes.
 * In the separate:new execution_mode, the lifetime of the Ad hoc Storage System
 will be controlled with
 * ADM_adhoc_walltime. In the separate:existing execution_mode, a valid context
 ID must be provided with
 * ADM_adhoc_context_id
 *
 * @param in.context A valid execution_mode describing how the Ad hoc Storage
 System should behave.
 * @return out.ret Returns if the remote procedure has been completed
 successfully or not.
 * @return out.adhoc_context_id A number that identifies the context.

 */
static void
ADM_adhoc_context(hg_handle_t h) {
    hg_return_t ret;

    ADM_adhoc_context_in_t in;
    ADM_adhoc_context_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_adhoc_context");
    LOGGER_INFO("remote_procedure::ADM_adhoc_context({})", in.context);

    if(in.context != nullptr) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_adhoc_context not null ({})",
                    in.context);
    } else {
        out.ret = -1;
        LOGGER_INFO(
                "remote_procedure::ADM_adhoc_context null or invalid ({}). Please use",
                in.context);
    }

    if((strcmp(in.context, "in_job:shared")) == 0 ||
       (strcmp(in.context, "in_job:dedicated")) == 0 ||
       (strcmp(in.context, "separate:new")) == 0 ||
       (strcmp(in.context, "separate:existing")) == 0) {
        LOGGER_INFO(
                "remote_procedure::ADM_adhoc_context value is acceptable ({})",
                in.context);
        out.adhoc_context = rand();
    } else {
        LOGGER_INFO(
                "remote_procedure::ADM_adhoc_context is not valid. Please use: in_job:shared, in_job:dedicated, separate:new or separate:existing ({})",
                in.context);
        out.adhoc_context = -1;
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_adhoc_context)

/**
 * Specifies an existing Ad hoc Storage System to use via its ID.
 *
 * @param in.context_id A valid context_id for a separate instance of an Ad hoc
 * Storage System.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_adhoc_context_id(hg_handle_t h) {
    hg_return_t ret;

    ADM_adhoc_context_id_in_t in;
    ADM_adhoc_context_id_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_adhoc_context_id");
    LOGGER_INFO("remote_procedure::ADM_adhoc_context_id({})", in.context_id);

    if(in.context_id >= 0) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_adhoc_context_id not null ({})",
                    in.context_id);
    } else {
        out.ret = -1;
        LOGGER_INFO(
                "remote_procedure::ADM_adhoc_context_id null or invalid ({}). Please use",
                in.context_id);
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_adhoc_context_id)

/**
 * Specifies the number of nodes for the Ad hoc Storage System. If the
 * ADM_adhoc_execution_mode is shared, the number cannot exceed the number of
 * allocated nodes within the compute job. If the ADM_adhoc_execution_mode is
 * dedicated, the number of nodes is not restricted.
 *
 * @param in.number_of_nodes The desired number_of_nodes.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_adhoc_nodes(hg_handle_t h) {
    hg_return_t ret;

    ADM_adhoc_nodes_in_t in;
    ADM_adhoc_nodes_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_adhoc_nodes");
    LOGGER_INFO("remote_procedure::ADM_adhoc_nodes({})", in.nodes);

    if(in.nodes > 0) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_adhoc_nodes not null ({})",
                    in.nodes);
    } else {
        out.ret = -1;
        LOGGER_INFO("remote_procedure::ADM_adhoc_nodes null ({})", in.nodes);
    }

    /*Specifies the number of nodes for the Ad hoc Storage System. If the
    ADM_adhoc_execution_mode is shared, the number cannot exceed the number of
    allocated nodes within the compute job. If the ADM_adhoc_execution_mode is
    dedicated, the number of nodes is not restricted. Should this be checked
    now? */

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_adhoc_nodes)


