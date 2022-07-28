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

#include <logger/logger.hpp>
#include <net/proto/rpc_types.h>
#include <admire.hpp>
#include "rpc_handlers.hpp"

struct remote_procedure {
    static std::uint64_t
    new_id() {
        static std::atomic_uint64_t current_id;
        return current_id++;
    }
};

static void
ADM_ping(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    LOGGER_INFO("PING(noargs)");

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_ping);

static void
ADM_register_job(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_register_job_in_t in;
    ADM_register_job_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    const admire::job_requirements reqs(&in.reqs);

    const auto id = remote_procedure::new_id();
    LOGGER_INFO("RPC ID {} ({}) <= {{job_requirements: {{{}}}}}", id,
                __FUNCTION__, reqs);

    const auto job = admire::job{42};

    const auto rpc_job = admire::unmanaged_rpc_type<admire::job>{job};
    admire::error_code rv = ADM_SUCCESS;

    out.retval = rv;
    out.job = rpc_job.get();

    LOGGER_INFO("RPC ID {} ({}) => {{retval: {}, job: {{{}}}}}", id,
                __FUNCTION__, rv, job);

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_register_job);


static void
ADM_update_job(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_update_job_in_t in;
    ADM_update_job_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    const admire::job job(in.job);
    const admire::job_requirements reqs(&in.reqs);

    const auto id = remote_procedure::new_id();
    LOGGER_INFO("RPC ID {} ({}) <= {{job: {{{}}}, job_requirements: {{{}}}}}",
                id, __FUNCTION__, job, reqs);

    admire::error_code rv = ADM_SUCCESS;
    out.retval = rv;

    LOGGER_INFO("RPC ID {} ({}) => {{retval: {}}}", id, __FUNCTION__, rv);

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_update_job);


static void
ADM_remove_job(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_remove_job_in_t in;
    ADM_remove_job_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    const admire::job job(in.job);

    const auto id = remote_procedure::new_id();
    LOGGER_INFO("RPC ID {} ({}) <= {{job: {{{}}}", id, __FUNCTION__, job);

    admire::error_code rv = ADM_SUCCESS;
    out.retval = rv;

    LOGGER_INFO("RPC ID {} ({}) => {{retval: {}}}", id, __FUNCTION__, rv);

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_remove_job);

static void
ADM_register_adhoc_storage(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_register_adhoc_storage_in_t in;
    ADM_register_adhoc_storage_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    LOGGER_INFO("ADM_register_adhoc_storage()");

    out.ret = 0;

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_register_adhoc_storage);

static void
ADM_update_adhoc_storage(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_update_adhoc_storage_in_t in;
    ADM_update_adhoc_storage_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    LOGGER_INFO("ADM_update_adhoc_storage()");

    out.ret = 0;

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_update_adhoc_storage);

static void
ADM_remove_adhoc_storage(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_remove_adhoc_storage_in_t in;
    ADM_remove_adhoc_storage_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    LOGGER_INFO("ADM_remove_adhoc_storage()");

    out.ret = 0;

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_remove_adhoc_storage);

static void
ADM_deploy_adhoc_storage(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_deploy_adhoc_storage_in_t in;
    ADM_deploy_adhoc_storage_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    LOGGER_INFO("ADM_deploy_adhoc_storage()");

    out.ret = 0;

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_deploy_adhoc_storage);

static void
ADM_register_pfs_storage(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_register_pfs_storage_in_t in;
    ADM_register_pfs_storage_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    LOGGER_INFO("ADM_register_pfs_storage()");

    out.ret = 0;

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_register_pfs_storage);

static void
ADM_update_pfs_storage(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_update_pfs_storage_in_t in;
    ADM_update_pfs_storage_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    LOGGER_INFO("ADM_update_pfs_storage()");

    out.ret = 0;

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_update_pfs_storage);

static void
ADM_remove_pfs_storage(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_remove_pfs_storage_in_t in;
    ADM_remove_pfs_storage_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    LOGGER_INFO("ADM_remove_pfs_storage()");

    out.ret = 0;

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_remove_pfs_storage);

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

    [[maybe_unused]] hg_return_t ret;

    ADM_input_in_t in;
    ADM_input_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    if(in.origin == nullptr) {
        LOGGER_ERROR("ADM_input(): invalid origin (nullptr)");
    } else if(in.target == nullptr) {
        LOGGER_ERROR("ADM_input(): invalid target (nullptr)");
    } else {
        LOGGER_INFO("ADM_input({}, {})", in.origin, in.target);
        out.ret = 0;
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

    [[maybe_unused]] hg_return_t ret;

    ADM_output_in_t in;
    ADM_output_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    if(in.origin == nullptr) {
        LOGGER_ERROR("ADM_output(): invalid origin (nullptr)");
    } else if(in.target == nullptr) {
        LOGGER_ERROR("ADM_output(): invalid target (nullptr)");
    } else {
        LOGGER_INFO("ADM_output({}, {})", in.origin, in.target);
        out.ret = 0;
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

    [[maybe_unused]] hg_return_t ret;

    ADM_inout_in_t in;
    ADM_inout_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    if(in.origin == nullptr) {
        LOGGER_ERROR("ADM_inout(): invalid origin (nullptr)");
    } else if(in.target == nullptr) {
        LOGGER_ERROR("ADM_inout(): invalid target (nullptr)");
    } else {
        LOGGER_INFO("ADM_inout({}, {})", in.origin, in.target);
        out.ret = 0;
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
 * options: in_job:shared (run while sharing the application’s compute nodes),
 * in_job:dedicated (run using a subset of the application’s compute nodes),
 * separate:new (ask the system to allocate a separate job with separate runtime
 * and number of nodes) and separate:existing (ask the system to reuse an
 * already running Ad hoc Storage System instance). The number of nodes assigned
 * for the Ad hoc Storage System must be specified with ADM_adhoc_nodes. In the
 * separate:new execution_mode, the lifetime of the Ad hoc Storage System will
 * be controlled with ADM_adhoc_walltime. In the separate:existing
 * execution_mode, a valid context ID must be provided with
 * ADM_adhoc_context_id.
 *
 * @param in.context A valid execution_mode describing how the Ad hoc Storage
 * System should behave.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 * @return out.adhoc_context_id A number that identifies the context.
 */
static void
ADM_adhoc_context(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_adhoc_context_in_t in;
    ADM_adhoc_context_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    const std::string ctx(in.context);

    out.ret = -1;
    out.adhoc_context = -1;

    if(in.context == nullptr) {
        LOGGER_ERROR("ADM_adhoc_context(): invalid context (nullptr)");
    } else {
        LOGGER_INFO("ADM_adhoc_context({})", in.context);

        if(ctx == "in_job:shared" || ctx == "in_job:dedicated" ||
           ctx == "separate:new" || ctx == "separate:existing") {
            LOGGER_INFO("ADM_adhoc_context value is acceptable ({})",
                        in.context);
            out.ret = 0;
            out.adhoc_context = rand();
        } else {
            LOGGER_ERROR(
                    "ADM_adhoc_context is not valid. Please use: in_job:shared, in_job:dedicated, separate:new or separate:existing");
        }
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

    [[maybe_unused]] hg_return_t ret;

    ADM_adhoc_context_id_in_t in;
    ADM_adhoc_context_id_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);


    out.ret = -1;

    if(in.context_id < 0) {
        LOGGER_ERROR("ADM_adhoc_context_id(): invalid context_id (< 0)");
    } else {
        LOGGER_INFO("ADM_adhoc_context_id({})", in.context_id);
        out.ret = 0;
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

    [[maybe_unused]] hg_return_t ret;

    ADM_adhoc_nodes_in_t in;
    ADM_adhoc_nodes_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    if(in.nodes <= 0) {
        LOGGER_ERROR("ADM_adhoc_nodes(): invalid n_nodes (<= 0)");
    } else {
        LOGGER_INFO("ADM_adhoc_nodes({})", in.nodes);
        out.ret = 0;
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

/**
 * Specifies for how long the ad hoc storage system should run before should
 * down. Only relevant in the context of the ADM_adhoc_context function.
 *
 * @param in.walltime The desired walltime in minutes.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_adhoc_walltime(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_adhoc_walltime_in_t in;
    ADM_adhoc_walltime_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    if(in.walltime < 0) {
        LOGGER_ERROR("ADM_adhoc_walltime(): invalid walltime (< 0)");
    } else {
        LOGGER_INFO("ADM_adhoc_walltime({})", in.walltime);
        out.ret = 0;
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_adhoc_walltime)

/**
 * Specifies access to the ad hoc storage system: write-only, read-only,
 * read-write. Cannot be used when using an existing Ad hoc Storage System
 * instance.
 *
 * @param in.access The desired access method
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_adhoc_access(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_adhoc_access_in_t in;
    ADM_adhoc_access_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    const std::string acc(in.access);

    out.ret = -1;

    if(in.access == nullptr) {
        LOGGER_ERROR("ADM_adhoc_access(): invalid access (nullptr)");
    } else {
        LOGGER_INFO("ADM_adhoc_access({}, {})", in.access);

        if((acc == "write-only") || (acc == "read-only") ||
           (acc == "read-write")) {
            out.ret = 0;
            LOGGER_INFO("ADM_adhoc_access value is acceptable ({})", in.access);
        } else {
            LOGGER_ERROR(
                    "ADM_adhoc_access is not valid. Please use: write-only, read-only or read-write");
        }
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_adhoc_access)


/**
 * Specifies the data distribution within the ad hoc storage system, e.g.,
 * wide-striping, local, local-data-global-metadata.
 *
 * @param in.data_distribution The desired data distribution
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_adhoc_distribution(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_adhoc_distribution_in_t in;
    ADM_adhoc_distribution_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    if(in.data_distribution == nullptr) {
        LOGGER_ERROR(
                "ADM_adhoc_distribution(): invalid data_distribution (nullptr)");
    } else {
        LOGGER_INFO("ADM_adhoc_distribution({})", in.data_distribution);
        out.ret = 0;
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_adhoc_distribution)

/**
 * Specifies if data in the output location should be moved to the shared
 * backend storage system in the background (default false).
 *
 * @param in.b_flush A boolean enabling or disabling the option.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_adhoc_background_flush(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_adhoc_background_flush_in_t in;
    ADM_adhoc_background_flush_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("ADM_adhoc_background_flush({})", in.b_flush);
    out.ret = 0;

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_adhoc_background_flush)

/**
 * In situ data operations specified in a given configuration file.
 *
 * @param in.in_situ A path to the configuration file.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_in_situ_ops(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_in_situ_ops_in_t in;
    ADM_in_situ_ops_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    if(in.in_situ == nullptr) {
        LOGGER_ERROR("ADM_in_situ_ops(): invalid in_situ_ops (nullptr)");
    } else {
        LOGGER_INFO("ADM_in_situ_ops({})", in.in_situ);
        out.ret = 0;
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_in_situ_ops)

/**
 * In transit data operations specified in a given configuration file.
 *
 * @param in.in_transit A path to the configuration file.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_in_transit_ops(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_in_transit_ops_in_t in;
    ADM_in_transit_ops_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;

    if(in.in_transit == nullptr) {
        LOGGER_ERROR("ADM_in_transit_ops(): invalid in_transit (nullptr)");
    } else {
        LOGGER_INFO("ADM_in_transit_ops({})", in.in_transit);
        out.ret = 0;
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_in_transit_ops)


/**
 * Transfers the dataset identified by the source_name to the storage tier
 * defined by destination_name, and apply the provided constraints during the
 * transfer. This function returns a handle that can be used to track the
 * operation (i.e., get statistics, or status).
 *
 * @param in.source A source_location identifying the source dataset/s in the
 * source storage tier.
 * @param in.destination A destination_location identifying the destination
 * dataset/s in its desired location in a storage tier.
 * @param in.qos_constraints A list of qos_constraints that must be applied to
 * the transfer. These may not exceed the global ones set at node, application,
 * or resource level (see Section 3.4).
 * @param in.distribution A distribution strategy for data (e.g. one-to-one,
 * one-to-many, many-to-many)
 * @param in.job_id A job_id identifying the originating job.
 * @param out.transfer_handle A transfer_handle allowing clients to interact
 * with the transfer (e.g. wait for its completion, query its status, cancel it,
 * etc.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_transfer_dataset(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_transfer_dataset_in_t in;
    ADM_transfer_dataset_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.transfer_handle = "fail";

    if(in.source == nullptr) {
        LOGGER_ERROR("ADM_transfer_dataset(): invalid source (nullptr)");
    } else if(in.destination == nullptr) {
        LOGGER_ERROR("ADM_transfer_dataset(): invalid destination (nullptr)");
    } else if(in.qos_constraints == nullptr) {
        LOGGER_ERROR(
                "ADM_transfer_dataset(): invalid qos_constraints (nullptr)");
    } else if(in.distribution == nullptr) {
        LOGGER_ERROR("ADM_transfer_dataset(): invalid distribution (nullptr)");
    } else if(in.job_id < 0) {
        LOGGER_ERROR("ADM_transfer_dataset(): invalid job_id (< 0)");
    } else {
        LOGGER_INFO("ADM_transfer_dataset({},{},{},{},{})", in.source,
                    in.destination, in.qos_constraints, in.distribution,
                    in.job_id);
        out.ret = 0;
        out.transfer_handle = "ok";
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_transfer_dataset)

/**
 * Sets information for the dataset identified by resource_id.
 *
 * @param in.resource_id A resource_id identifying the dataset of interest.
 * @param in.info An opaque inf o argument containing information about the
 * dataset (e.g. its lifespan, access methods, intended usage, etc.).
 * @param in.job_id A job_id identifying the originating job.
 * @param out.status A status code determining whether the operation was
 * successful.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_set_dataset_information(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_set_dataset_information_in_t in;
    ADM_set_dataset_information_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.status = -1;

    if(in.resource_id < 0) {
        LOGGER_ERROR(
                "ADM_set_dataset_information(): invalid resource_id (< 0)");
    } else if(in.info == nullptr) {
        LOGGER_ERROR("ADM_set_dataset_information(): invalid info (nullptr)");
    } else if(in.job_id < 0) {
        LOGGER_ERROR("ADM_set_dataset_information(): invalid job_id (< 0)");
    } else {
        LOGGER_INFO("ADM_set_dataset_information({},{},{})", in.resource_id,
                    in.info, in.job_id);
        out.ret = 0;
        out.status = 0;
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_set_dataset_information)

/**
 * Changes the I/O resources used by a storage tier, typically an Ad hoc Storage
 * System.
 *
 * @param in.tier_id A tier_id specifying the target storage tier.
 * @param in.resources An opaque resources argument containing information about
 * the I/O resources to modify (e.g. number of I/O nodes.).
 * @param in.job_id A job_id identifying the originating job.
 * @param out.status A status code determining whether the operation was
 * successful.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_set_io_resources(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_set_io_resources_in_t in;
    ADM_set_io_resources_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.status = -1;

    if(in.tier_id < 0) {
        LOGGER_ERROR("ADM_set_io_resources(): invalid tier_id (nullptr)");
    } else if(in.resources == nullptr) {
        LOGGER_ERROR("ADM_set_io_resources(): invalid resources (nullptr)");
    } else if(in.job_id < 0) {
        LOGGER_ERROR("ADM_set_io_resources(): invalid job_id (< 0)");
    } else {
        LOGGER_INFO("ADM_set_io_resources({},{},{})", in.tier_id, in.resources,
                    in.job_id);
        out.ret = 0;
        out.status = 0;
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_set_io_resources)


/**
 * Returns the priority of the pending transfer identified by transfer_id.
 *
 * @param in.transfer_id A tier_id specifying the target storage tier.
 * @param out.priority The priority of the pending transfer or an error code if
 * it didn’t exist or is no longer pending.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_get_transfer_priority(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_get_transfer_priority_in_t in;
    ADM_get_transfer_priority_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.priority = -1;

    if(in.transfer_id < 0) {
        LOGGER_ERROR(
                "ADM_get_transfer_priority(): invalid transfer_id (nullptr)");
    } else {
        LOGGER_INFO("ADM_get_transfer_priority({})", in.transfer_id);
        out.ret = 0;
        out.priority = 0;
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_get_transfer_priority)

/**
 * Moves the operation identified by transfer_id up or down by n positions in
 * its scheduling queue.
 *
 * @param in.transfer_id A transf er_id identifying a pending transfer.
 * @param in.n_positions A positive or negative number n for the number of
 * positions the transfer should go up or down in its scheduling queue.
 * @param out.status A status code indicating whether the operation was
 * successful.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_set_transfer_priority(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_set_transfer_priority_in_t in;
    ADM_set_transfer_priority_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.status = -1;

    if(in.transfer_id < 0) {
        LOGGER_ERROR(
                "ADM_set_transfer_priority(): invalid transfer_id (nullptr)");
    } else {
        LOGGER_INFO("ADM_set_transfer_priority({}, {})", in.transfer_id,
                    in.n_positions);
        out.ret = 0;
        out.status = 0;
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_set_transfer_priority)

/**
 * Cancels the pending transfer identified by transfer_id.
 *
 * @param in.transfer_id A transfer_id identifying a pending transfer.
 * @param out.status A status code indicating whether the operation was
 * successful.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_cancel_transfer(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_cancel_transfer_in_t in;
    ADM_cancel_transfer_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.status = -1;

    if(in.transfer_id < 0) {
        LOGGER_ERROR("ADM_cancel_transfer(): invalid transfer_id (< 0)");
    } else {
        LOGGER_INFO("ADM_cancel_transfer({})", in.transfer_id);
        out.ret = 0;
        out.status = 0;
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_cancel_transfer)

/**
 * Returns a list of pending transfers. Each operation will include a transf
 * er_id as well as information about the involved resources and tiers.
 *
 * @param out.pending_transfers  A list of pending_transfers.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_get_pending_transfers(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_get_pending_transfers_in_t in;
    ADM_get_pending_transfers_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = 0;
    out.pending_transfers = "list";

    LOGGER_INFO("ADM_get_pending_transfers()");

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_get_pending_transfers)

/**
 * Registers a QoS constraint defined by class, scope, and value for the element
 * identified by id.
 *
 * @param in.scope The scope it should be applied to: dataset, node, or job.
 * @param in.qos_class A QoS class (e.g. "badwidth", "iops", etc.).
 * @param in.element_id A valid id for the element that should be constrained,
 * i.e. a resource ID, a node hostname, or a Job ID.
 * @param in.class_value An appropriate value for the selected class.
 * @param out.status A status code indicating whether the operation was
 * successful.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_set_qos_constraints(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_set_qos_constraints_in_t in;
    ADM_set_qos_constraints_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.status = -1;

    if(in.scope == nullptr) {
        LOGGER_ERROR("ADM_set_qos_constraints(): invalid scope (nullptr)");
    } else if(in.qos_class == nullptr) {
        LOGGER_ERROR("ADM_set_qos_constraints(): invalid qos_class (nullptr)");
    } else if(in.element_id < 0) {
        LOGGER_ERROR("ADM_set_qos_constraints(): invalid element_id (< 0)");
    } else if(in.class_value == nullptr) {
        LOGGER_ERROR(
                "ADM_set_qos_constraints(): invalid class_value (nullptr)");
    } else {
        LOGGER_INFO("ADM_set_qos_constraints({}, {}, {}, {})", in.scope,
                    in.qos_class, in.element_id, in.class_value);
        const std::string scp(in.scope);
        if((scp == "dataset") || (scp == "node") || (scp == "job")) {
            LOGGER_INFO(
                    "ADM_set_qos_constraints scope value is acceptable ({})",
                    in.scope);
            out.ret = 0;
            out.status = 0;
        } else {
            LOGGER_ERROR(
                    "ADM_set_qos_constraints scope value is not valid. Please use: dataset, node or job");
        }
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_set_qos_constraints)

/**
 * Returns a list of QoS constraints defined for an element identified for id.
 *
 * @param in.scope The scope being queried: dataset, node, or job.
 * @param in.element_id  A valid id for the element of interest, i.e. a resource
 * ID, a node hostname, or a Job ID.
 * @param out.list A list of QoS constraints that includes all the classes
 * currently defined for the element as well as the values set for them.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_get_qos_constraints(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_get_qos_constraints_in_t in;
    ADM_get_qos_constraints_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.list = nullptr;

    if(in.scope == nullptr) {
        LOGGER_ERROR("ADM_get_qos_constraints(): invalid scope (nullptr)");
    } else if(in.element_id < 0) {
        LOGGER_ERROR("ADM_get_qos_constraints(): invalid element_id (< 0)");
    } else {
        LOGGER_INFO("ADM_get_qos_constraints({}, {})", in.scope, in.element_id);

        const std::string scp(in.scope);

        if((scp == "dataset") || (scp == "node") || (scp == "job")) {
            LOGGER_INFO(
                    "ADM_get_qos_constraints scope value is acceptable ({})",
                    in.scope);
            out.ret = 0;
            out.list = "list";
        } else {
            LOGGER_ERROR(
                    "ADM_get_qos_constraints scope value is not valid. Please use: dataset, node or job ");
        }
    }


    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_get_qos_constraints)

/**
 * Defines a new operation, with the code found in path. The code will be
 * identified by the user-provided operation_id and will accept the arguments
 * defined, using the next format "arg0, arg1, arg2, . . . ".
 *
 * @param in.path A valid path for the operation code.
 * @param in.operation_id A user-defined operation_id for the operation.
 * @param in.arguments A list of arguments for the operation.
 * @param out.status A status code indicating whether the operation was
 * successful.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_define_data_operation(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_define_data_operation_in_t in;
    ADM_define_data_operation_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.status = -1;

    if(in.path == nullptr) {
        LOGGER_ERROR("ADM_define_data_operation(): invalid path (nullptr)");
    } else if(in.operation_id < 0) {
        LOGGER_ERROR("ADM_define_data_operation(): invalid operation_id (< 0)");
    } else if(in.arguments == nullptr) {
        LOGGER_ERROR(
                "ADM_define_data_operation(): invalid arguments (nullptr)");
    } else {
        LOGGER_INFO("ADM_define_data_operation ({}, {}, {})", in.path,
                    in.operation_id, in.arguments);
        out.ret = 0;
        out.status = 0;
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_define_data_operation)


/**
 * Connects and starts the data operation defined with operation_id and with the
 * arguments, using the input and output data storage (i.e., files). If the
 * operation can be executed in a streaming fashion (i.e., it can start even if
 * the input data is not entirely available), the stream parameter must be set
 * to true.
 *
 * @param in.operation_id The operation_id of the operation to be connected.
 * @param in.input An input data resource for the operation.
 * @param in.stream A stream boolean indicating if the operation should be
 * executed in a streaming fashion.
 * @param in.arguments The values for the arguments required by the operation.
 * @param in.job_id A job_id identifying the originating job.
 * @param out.data An output data resource where the result of the operation
 * should be stored.
 * @return out.operation_handle An operation_handle for the operation that
 * allows clients to further interact with the operation (e.g query its status,
 * cancel it, etc.).
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_connect_data_operation(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_connect_data_operation_in_t in;
    ADM_connect_data_operation_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.data = nullptr;
    out.operation_handle = nullptr;

    if(in.operation_id < 0) {
        LOGGER_ERROR(
                "ADM_connect_data_operation(): invalid operation_id (< 0)");
    } else if(in.input == nullptr) {
        LOGGER_ERROR("ADM_define_data_operation(): invalid input (nullptr)");
    } else if(in.stream != true && in.stream != false) {
        LOGGER_ERROR(
                "ADM_connect_data_operation(): invalid stream (not true/false)");
    } else if(in.arguments == nullptr) {
        LOGGER_ERROR(
                "ADM_connect_data_operation(): invalid arguments (nullptr)");
    } else if(in.job_id < 0) {
        LOGGER_ERROR("ADM_connect_data_operation(): invalid job_id (< 0)");
    } else {
        LOGGER_INFO("ADM_connect_data_operation({}, {}, {}, {}, {})",
                    in.operation_id, in.input, in.stream, in.arguments,
                    in.job_id);
        out.ret = 0;
        out.data = "ouput";
        out.operation_handle = "operation_handle";
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_connect_data_operation)

/**
 * Finalises the operation defined with operation_id.
 *
 * @param in.operation_id The operation_id of the operation to be connected.
 * @return out.status A status code indicating whether the operation was
 * successful.
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_finalize_data_operation(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_finalize_data_operation_in_t in;
    ADM_finalize_data_operation_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.status = -1;

    if(in.operation_id < 0) {
        LOGGER_ERROR(
                "ADM_finalize_data_operation(): invalid operation_id (< 0)");
    } else {
        LOGGER_INFO("ADM_finalize_data_operation({})", in.operation_id);
        out.ret = 0;
        out.status = 0;
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_finalize_data_operation)

/**
 * Links the data operation defined with operation_id with the pending transfer
 * identified by transf er_id using the values provided as arguments. If the
 * operation can be executed in a streaming fashion (i.e., it can start even if
 * the input data is not entirely available), the stream parameter must be set
 * to true.
 *
 * @param in.operation_id The operation_id of the operation to be connected.
 * @param in.transfer_id The transfer_id of the pending transfer the operation
 * should be linked to.
 * @param in.stream A stream boolean indicating if the operation should be
 * executed in a streaming fashion.
 * @param in.arguments The values for the arguments required by the operation.
 * @param in.job_id A job_id identifying the originating job.
 * @return out.operation_handle An operation_handle for the operation that
 * allows clients to further interact with the operation (e.g query its status,
 * cancel it, etc.).
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_link_transfer_to_data_operation(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_link_transfer_to_data_operation_in_t in;
    ADM_link_transfer_to_data_operation_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.operation_handle = nullptr;

    if(in.operation_id < 0) {
        LOGGER_ERROR(
                "ADM_link_transfer_to_data_operation(): invalid operation_id (< 0)");
    } else if(in.transfer_id < 0) {
        LOGGER_ERROR(
                "ADM_link_transfer_to_data_operation(): invalid transfer_id (< 0)");
    } else if(in.arguments == nullptr) {
        LOGGER_ERROR(
                "ADM_link_transfer_to_data_operation(): invalid arguments (nullptr)");
    } else if(in.stream != true && in.stream != false) {
        LOGGER_ERROR(
                "ADM_link_transfer_to_data_operation(): invalid stream (not true/false)");
    } else if(in.job_id < 0) {
        LOGGER_ERROR(
                "ADM_link_transfer_to_data_operation(): invalid job_id (< 0)");
    } else {
        LOGGER_INFO("ADM_link_transfer_to_data_operation ({}, {}, {}, {}, {})",
                    in.operation_id, in.transfer_id, in.stream, in.arguments,
                    in.job_id);
        out.ret = 0;
        out.operation_handle = "operation_handle";
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_link_transfer_to_data_operation)


/**
 * Returns the current I/O statistics for a specified job_id and an optional
 * corresponding job_step. The information will be returned in an
 * easy-to-process format, e.g., JSON (see Listing 3.1).
 *
 * @param in.job_id
 * @param in.job_step
 * @return out.job_statistics
 * @return out.ret Returns if the remote procedure has been completed
 * successfully or not.
 */
static void
ADM_get_statistics(hg_handle_t h) {

    [[maybe_unused]] hg_return_t ret;

    ADM_get_statistics_in_t in;
    ADM_get_statistics_out_t out;

    [[maybe_unused]] margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    out.ret = -1;
    out.job_statistics = nullptr;

    if(in.job_id < 0) {
        LOGGER_ERROR("ADM_get_statistics(): invalid job_id (< 0)");
    } else if(in.job_step < 0) {
        LOGGER_ERROR("ADM_get_statistics(): invalid job_step (< 0)");
    } else {
        LOGGER_INFO("ADM_get_statistics ({}, {})", in.job_id, in.job_step);
        out.ret = 0;
        out.job_statistics = "job_statistics";
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_get_statistics)
