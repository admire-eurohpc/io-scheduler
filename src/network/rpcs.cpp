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
    hg_return_t ret;

    ADM_adhoc_walltime_in_t in;
    ADM_adhoc_walltime_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_adhoc_walltime");
    LOGGER_INFO("remote_procedure::ADM_adhoc_walltime({})", in.walltime);

    if(in.walltime >= 0) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_adhoc_walltime not null ({})",
                    in.walltime);
    } else {
        out.ret = -1;
        LOGGER_INFO(
                "remote_procedure::ADM_adhoc_walltime null or invalid ({}). Please use",
                in.walltime);
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
    hg_return_t ret;

    ADM_adhoc_access_in_t in;
    ADM_adhoc_access_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_adhoc_access");
    LOGGER_INFO("remote_procedure::ADM_adhoc_access({})", in.access);

    if(in.access != nullptr) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_adhoc_access not null ({})",
                    in.access);
    } else {
        out.ret = -1;
        LOGGER_INFO(
                "remote_procedure::ADM_adhoc_access null or invalid ({}). Please use",
                in.access);
    }

    if((strcmp(in.access, "write-only")) == 0 ||
       (strcmp(in.access, "read-only")) == 0 ||
       (strcmp(in.access, "read-write") == 0)) {
        out.ret = 0;
        LOGGER_INFO(
                "remote_procedure::ADM_adhoc_access value is acceptable ({})",
                in.access);
    } else {
        out.ret = -1;
        LOGGER_INFO(
                "remote_procedure::ADM_adhoc_access is not valid. Please use: write-only, read-only or read-write",
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
    hg_return_t ret;

    ADM_adhoc_distribution_in_t in;
    ADM_adhoc_distribution_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_adhoc_distribution");
    LOGGER_INFO("remote_procedure::ADM_adhoc_distribution({})",
                in.data_distribution);

    if(in.data_distribution != nullptr) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_adhoc_distribution not null ({})",
                    in.data_distribution);
    } else {
        out.ret = -1;
        LOGGER_INFO(
                "remote_procedure::ADM_adhoc_distribution null or invalid ({}). Please use",
                in.data_distribution);
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
    hg_return_t ret;

    ADM_adhoc_background_flush_in_t in;
    ADM_adhoc_background_flush_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_adhoc_background_flush");
    LOGGER_INFO("remote_procedure::ADM_adhoc_background_flush({})", in.b_flush);

    if(in.b_flush == 0 || in.b_flush == 1) {
        out.ret = 0;
        LOGGER_INFO(
                "remote_procedure::ADM_adhoc_background_flush not null ({})",
                in.b_flush);
    } else {
        out.ret = -1;
        LOGGER_INFO("remote_procedure::ADM_adhoc_background_flush null ({})",
                    in.b_flush);
    }

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
    hg_return_t ret;

    ADM_in_situ_ops_in_t in;
    ADM_in_situ_ops_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_in_situ_ops");
    LOGGER_INFO("remote_procedure::ADM_in_situ_ops({})", in.in_situ);

    if(in.in_situ != nullptr) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_in_situ_ops not null ({})",
                    in.in_situ);
    } else {
        out.ret = -1;
        LOGGER_INFO(
                "remote_procedure::ADM_in_situ_ops null or invalid ({}). Please use",
                in.in_situ);
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
    hg_return_t ret;

    ADM_in_transit_ops_in_t in;
    ADM_in_transit_ops_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_in_transit_ops");
    LOGGER_INFO("remote_procedure::ADM_in_transit_ops({})", in.in_transit);

    if(in.in_transit != nullptr) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_in_transit_ops not null ({})",
                    in.in_transit);
    } else {
        out.ret = -1;
        LOGGER_INFO(
                "remote_procedure::ADM_in_transit_ops null or invalid ({}). Please use",
                in.in_transit);
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
    hg_return_t ret;

    ADM_transfer_dataset_in_t in;
    ADM_transfer_dataset_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_transfer_dataset");
    LOGGER_INFO("remote_procedure::ADM_transfer_dataset({},{},{},{},{})",
                in.source, in.destination, in.qos_constraints, in.distribution,
                in.job_id);

    if(in.source != nullptr && in.destination != nullptr &&
       in.qos_constraints != nullptr && in.distribution != nullptr &&
       in.job_id >= 0) {
        out.ret = 0;
        LOGGER_INFO(
                "remote_procedure::ADM_transfer_dataset not null ({},{},{},{},{})",
                in.source, in.destination, in.qos_constraints, in.distribution,
                in.job_id);
        out.transfer_handle = "ok";
    } else {
        out.ret = -1;
        LOGGER_INFO(
                "remote_procedure::ADM_transfer_dataset null ({},{},{},{},{})",
                in.source, in.destination, in.qos_constraints, in.distribution,
                in.job_id);
        out.transfer_handle = "fail";
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
    hg_return_t ret;

    ADM_set_dataset_information_in_t in;
    ADM_set_dataset_information_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_set_dataset_information");
    LOGGER_INFO("remote_procedure::ADM_set_dataset_information({},{},{})",
                in.resource_id, in.info, in.job_id);

    if(in.resource_id >= 0 && in.info != nullptr && in.job_id >= 0) {
        out.ret = 0;
        LOGGER_INFO(
                "remote_procedure::ADM_transfer_dataset not null ({},{},{})",
                in.resource_id, in.info, in.job_id);
        out.status = 0;
    } else {
        out.ret = -1;
        LOGGER_INFO("remote_procedure::ADM_transfer_dataset null ({},{},{})",
                    in.resource_id, in.info, in.job_id);
        out.status = -1;
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
    hg_return_t ret;

    ADM_set_io_resources_in_t in;
    ADM_set_io_resources_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_set_io_resources");
    LOGGER_INFO("remote_procedure::ADM_set_io_resources({},{},{})", in.tier_id,
                in.resources, in.job_id);

    if(in.tier_id >= 0 && in.resources != nullptr && in.job_id >= 0) {
        out.ret = 0;
        LOGGER_INFO(
                "remote_procedure::ADM_transfer_dataset not null ({},{},{})",
                in.tier_id, in.resources, in.job_id);
        out.status = 0;
    } else {
        out.ret = -1;
        LOGGER_INFO("remote_procedure::ADM_transfer_dataset null ({},{},{})",
                    in.tier_id, in.resources, in.job_id);
        out.status = -1;
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
    hg_return_t ret;

    ADM_get_transfer_priority_in_t in;
    ADM_get_transfer_priority_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_get_transfer_priority");
    LOGGER_INFO("remote_procedure::ADM_get_transfer_priority({})",
                in.transfer_id);

    if(in.transfer_id >= 0) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_get_transfer_priority not null ({})",
                    in.transfer_id);
        out.priority = 0;
    } else {
        out.ret = -1;
        LOGGER_INFO("remote_procedure::ADM_get_transfer_priority null ({})",
                    in.transfer_id);
        out.priority = -1;
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
 * Moves the operation identified by transf er_id up or down by n positions in
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
    hg_return_t ret;

    ADM_set_transfer_priority_in_t in;
    ADM_set_transfer_priority_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_set_transfer_priority");
    LOGGER_INFO("remote_procedure::ADM_set_transfer_priority({}, {})",
                in.transfer_id, in.n_positions);

    if(in.transfer_id >= 0 && (in.n_positions >= 0 || in.n_positions < 0)) {
        out.ret = 0;
        LOGGER_INFO(
                "remote_procedure::ADM_set_transfer_priority not null ({}, {})",
                in.transfer_id, in.n_positions);
        out.status = 0;
    } else {
        out.ret = -1;
        LOGGER_INFO("remote_procedure::ADM_set_transfer_priority null ({}, {})",
                    in.transfer_id, in.n_positions);
        out.status = -1;
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
 * Moves the operation identified by transf er_id up or down by n positions in
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
ADM_cancel_transfer(hg_handle_t h) {
    hg_return_t ret;

    ADM_cancel_transfer_in_t in;
    ADM_cancel_transfer_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_cancel_transfer");
    LOGGER_INFO("remote_procedure::ADM_cancel_transfer({})", in.transfer_id);

    if(in.transfer_id >= 0) {
        out.ret = 0;
        LOGGER_INFO("remote_procedure::ADM_cancel_transfer not null ({})",
                    in.transfer_id);
        out.status = 0;
    } else {
        out.ret = -1;
        LOGGER_INFO("remote_procedure::ADM_cancel_transfer null ({})",
                    in.transfer_id);
        out.status = -1;
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
    hg_return_t ret;

    ADM_get_pending_transfers_in_t in;
    ADM_get_pending_transfers_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_get_pending_transfers");
    LOGGER_INFO("remote_procedure::ADM_get_pending_transfers");

    out.ret = 0;
    out.pending_transfers = "list";

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
 * @return out.ret A status code indicating whether the operation succeeded.
 */
static void
ADM_set_qos_constraints_push(hg_handle_t h) {
    hg_return_t ret;

    ADM_set_qos_constraints_push_in_t in;
    ADM_set_qos_constraints_push_out_t out;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    LOGGER_INFO("LOADED ADM_set_qos_constraints_push");
    LOGGER_INFO("remote_procedure::ADM_set_qos_constraints_push({}, {}, {}, {})",
                in.scope, in.qos_class, in.element_id, in.class_value);

    int aux_not_null = NULL;
    int aux_score_value_ok = NULL;


    if(in.scope != nullptr && in.qos_class != nullptr && in.element_id >= 0 &&
       in.class_value != nullptr) {
        aux_not_null = 0;
        LOGGER_INFO(
                "remote_procedure::ADM_set_qos_constraints_push not null ({}, {}, {}, {})",
                in.scope, in.qos_class, in.element_id, in.class_value);
    } else {
        aux_not_null = -1;
        LOGGER_INFO(
                "remote_procedure::ADM_set_qos_constraints_push null ({}, {}, {}, {})",
                in.scope, in.qos_class, in.element_id, in.class_value);
    }

    if((strcmp(in.scope, "dataset")) == 0 || (strcmp(in.scope, "node")) == 0 ||
       (strcmp(in.scope, "job")) == 0) {
        LOGGER_INFO(
                "remote_procedure::ADM_set_qos_constraints_push scope value is acceptable ({})",
                in.scope);
        aux_score_value_ok = 0;

    } else {
        LOGGER_INFO(
                "remote_procedure::ADM_set_qos_constraints_push scope value is not valid. Please use: dataset, node or job ({})",
                in.scope);
        aux_score_value_ok = -1;
    } 

    if(aux_not_null == 0 && aux_score_value_ok == 0) {
        out.ret = 0;
        out.status = 0;
    } else {
        out.ret = -1;
        out.status = -1;
    }

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    ret = margo_free_input(h, &in);
    assert(ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);
}

DEFINE_MARGO_RPC_HANDLER(ADM_set_qos_constraints_push)
