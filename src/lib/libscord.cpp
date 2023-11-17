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

#include <scord/scord.hpp>
#include <logger/logger.hpp>
#include <utils/ctype_ptr.hpp>
#include <env.hpp>
#include <iostream>
#include "detail/impl.hpp"


namespace {

[[maybe_unused]] void
init_library() __attribute__((constructor));

void
init_logger();

[[maybe_unused]] void
init_library() {
    init_logger();
}

/** Logging for the library */
void
init_logger() {

    try {
        if(const auto p = std::getenv(scord::env::LOG);
           p && !std::string{p}.empty() && std::string{p} != "0") {

            if(const auto log_file = std::getenv(scord::env::LOG_OUTPUT);
               log_file) {
                logger::create_default_logger(logger::logger_config{
                        "libscord", logger::logger_type::file, log_file});
                return;
            }

            logger::create_default_logger(logger::logger_config{
                    "libscord", logger::logger_type::console_color});
        }
    } catch(const std::exception& ex) {
        std::cerr << fmt::format("WARNING: Error initializing logger: {}",
                                 ex.what());
    }
}


#if 0
void
rpc_registration_cb(scord::network::rpc_client* client) {

    REGISTER_RPC(client, "ADM_ping", void, ADM_ping_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_register_job", ADM_register_job_in_t,
                 ADM_register_job_out_t, NULL, true);
    REGISTER_RPC(client, "ADM_update_job", ADM_update_job_in_t,
                 ADM_update_job_out_t, NULL, true);
    REGISTER_RPC(client, "ADM_remove_job", ADM_remove_job_in_t,
                 ADM_remove_job_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_register_adhoc_storage",
                 ADM_register_adhoc_storage_in_t,
                 ADM_register_adhoc_storage_out_t, NULL, true);
    REGISTER_RPC(client, "ADM_update_adhoc_storage",
                 ADM_update_adhoc_storage_in_t, ADM_update_adhoc_storage_out_t,
                 NULL, true);
    REGISTER_RPC(client, "ADM_remove_adhoc_storage",
                 ADM_remove_adhoc_storage_in_t, ADM_remove_adhoc_storage_out_t,
                 NULL, true);

    REGISTER_RPC(client, "ADM_deploy_adhoc_storage",
                 ADM_deploy_adhoc_storage_in_t, ADM_deploy_adhoc_storage_out_t,
                 NULL, true);

    REGISTER_RPC(client, "ADM_register_pfs_storage",
                 ADM_register_pfs_storage_in_t, ADM_register_pfs_storage_out_t,
                 NULL, true);
    REGISTER_RPC(client, "ADM_update_pfs_storage", ADM_update_pfs_storage_in_t,
                 ADM_update_pfs_storage_out_t, NULL, true);
    REGISTER_RPC(client, "ADM_remove_pfs_storage", ADM_remove_pfs_storage_in_t,
                 ADM_remove_pfs_storage_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_input", ADM_input_in_t, ADM_input_out_t, NULL,
                 true);


    REGISTER_RPC(client, "ADM_output", ADM_output_in_t, ADM_output_out_t, NULL,
                 true);

    REGISTER_RPC(client, "ADM_inout", ADM_inout_in_t, ADM_inout_out_t, NULL,
                 true);

    REGISTER_RPC(client, "ADM_adhoc_context", ADM_adhoc_context_in_t,
                 ADM_adhoc_context_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_adhoc_context_id", ADM_adhoc_context_id_in_t,
                 ADM_adhoc_context_id_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_adhoc_nodes", ADM_adhoc_nodes_in_t,
                 ADM_adhoc_nodes_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_adhoc_walltime", ADM_adhoc_walltime_in_t,
                 ADM_adhoc_walltime_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_adhoc_access", ADM_adhoc_access_in_t,
                 ADM_adhoc_access_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_adhoc_distribution", ADM_adhoc_distribution_in_t,
                 ADM_adhoc_distribution_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_adhoc_background_flush",
                 ADM_adhoc_background_flush_in_t,
                 ADM_adhoc_background_flush_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_in_situ_ops", ADM_in_situ_ops_in_t,
                 ADM_in_situ_ops_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_in_transit_ops", ADM_in_transit_ops_in_t,
                 ADM_in_transit_ops_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_transfer_datasets", ADM_transfer_datasets_in_t,
                 ADM_transfer_datasets_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_set_dataset_information",
                 ADM_set_dataset_information_in_t,
                 ADM_set_dataset_information_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_set_io_resources", ADM_set_io_resources_in_t,
                 ADM_set_io_resources_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_get_transfer_priority",
                 ADM_get_transfer_priority_in_t,
                 ADM_get_transfer_priority_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_set_transfer_priority",
                 ADM_set_transfer_priority_in_t,
                 ADM_set_transfer_priority_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_cancel_transfer", ADM_cancel_transfer_in_t,
                 ADM_cancel_transfer_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_get_pending_transfers",
                 ADM_get_pending_transfers_in_t,
                 ADM_get_pending_transfers_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_set_qos_constraints",
                 ADM_set_qos_constraints_in_t, ADM_set_qos_constraints_out_t,
                 NULL, true);

    REGISTER_RPC(client, "ADM_get_qos_constraints",
                 ADM_get_qos_constraints_in_t, ADM_get_qos_constraints_out_t,
                 NULL, true);

    REGISTER_RPC(client, "ADM_define_data_operation",
                 ADM_define_data_operation_in_t,
                 ADM_define_data_operation_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_connect_data_operation",
                 ADM_connect_data_operation_in_t,
                 ADM_connect_data_operation_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_finalize_data_operation",
                 ADM_finalize_data_operation_in_t,
                 ADM_finalize_data_operation_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_link_transfer_to_data_operation",
                 ADM_link_transfer_to_data_operation_in_t,
                 ADM_link_transfer_to_data_operation_out_t, NULL, true);

    REGISTER_RPC(client, "ADM_get_statistics", ADM_get_statistics_in_t,
                 ADM_get_statistics_out_t, NULL, true);
}
#endif

} // namespace


namespace scord {

void
ping(const server& srv) {
    if(const auto rv = detail::ping(srv); !rv) {
        throw std::runtime_error(
                fmt::format("ADM_ping() error: {}", rv.message()));
    }
}

job_info
query(const server& srv, slurm_job_id id) {
    return detail::query(srv, id)
            .or_else([](auto ec) {
                throw std::runtime_error(
                        fmt::format("ADM_query() error: {}", ec.message()));
            })
            .value();
}

scord::job
register_job(const server& srv, const job::resources& resources,
             const job::requirements& job_requirements,
             scord::slurm_job_id slurm_job_id) {

    const auto rv = detail::register_job(srv, resources, job_requirements,
                                         slurm_job_id);

    if(!rv) {
        throw std::runtime_error(fmt::format("ADM_register_job() error: {}",
                                             rv.error().message()));
    }

    return rv.value();
}

void
update_job(const server& srv, const job& job,
           const job::resources& job_resources) {

    const auto ec = detail::update_job(srv, job, job_resources);

    if(!ec) {
        throw std::runtime_error(
                fmt::format("ADM_update_job() error: {}", ec.message()));
    }
}

void
remove_job(const server& srv, const job& job) {

    const auto ec = detail::remove_job(srv, job);

    if(!ec) {
        throw std::runtime_error(
                fmt::format("ADM_remove_job() error: {}", ec.message()));
    }
}

scord::adhoc_storage
register_adhoc_storage(const server& srv, const std::string& name,
                       enum adhoc_storage::type type,
                       const adhoc_storage::ctx& ctx,
                       const adhoc_storage::resources& resources) {

    const auto rv =
            detail::register_adhoc_storage(srv, name, type, ctx, resources);

    if(!rv) {
        throw std::runtime_error(
                fmt::format("ADM_register_adhoc_storage() error: {}",
                            rv.error().message()));
    }

    return rv.value();
}

void
update_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage,
                     const adhoc_storage::resources& new_resources) {
    const auto ec =
            detail::update_adhoc_storage(srv, adhoc_storage, new_resources);

    if(!ec) {
        throw std::runtime_error(fmt::format(
                "ADM_update_adhoc_storage() error: {}", ec.message()));
    }
}

void
remove_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage) {

    const auto ec = detail::remove_adhoc_storage(srv, adhoc_storage);

    if(!ec) {
        throw std::runtime_error(fmt::format(
                "ADM_remove_adhoc_storage() error: {}", ec.message()));
    }
}

std::string
deploy_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage) {
    return detail::deploy_adhoc_storage(srv, adhoc_storage)
            .or_else([](auto ec) {
                throw std::runtime_error(fmt::format(
                        "ADM_deploy_adhoc_storage() error: {}", ec.message()));
            })
            .transform([](auto&& path) { return path.string(); })
            .value();
}

void
terminate_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage) {

    const auto ec = detail::terminate_adhoc_storage(srv, adhoc_storage);

    if(!ec) {
        throw std::runtime_error(fmt::format(
                "ADM_deploy_adhoc_storage() error: {}", ec.message()));
    }
}

scord::pfs_storage
register_pfs_storage(const server& srv, const std::string& name,
                     enum pfs_storage::type type, const pfs_storage::ctx& ctx) {

    const auto rv = detail::register_pfs_storage(srv, name, type, ctx);

    if(!rv) {
        throw std::runtime_error(fmt::format(
                "ADM_register_pfs_storage() error: {}", rv.error().message()));
    }

    return rv.value();
}


void
update_pfs_storage(const server& srv, const pfs_storage& pfs_storage,
                   const scord::pfs_storage::ctx& pfs_storage_ctx) {

    const auto ec =
            detail::update_pfs_storage(srv, pfs_storage, pfs_storage_ctx);

    if(!ec) {
        throw std::runtime_error(fmt::format(
                "ADM_update_pfs_storage() error: {}", ec.message()));
    }
}

void
remove_pfs_storage(const server& srv, const pfs_storage& pfs_storage) {

    const auto ec = detail::remove_pfs_storage(srv, pfs_storage);

    if(!ec) {
        throw std::runtime_error(fmt::format(
                "ADM_remove_pfs_storage() error: {}", ec.message()));
    }
}

scord::transfer
transfer_datasets(const server& srv, const job& job,
                  const std::vector<dataset>& sources,
                  const std::vector<dataset>& targets,
                  const std::vector<qos::limit>& limits,
                  transfer::mapping mapping) {

    const auto rv = detail::transfer_datasets(srv, job, sources, targets,
                                              limits, mapping);

    if(!rv) {
        throw std::runtime_error(fmt::format(
                "ADM_transfer_datasets() error: {}", ADM_strerror(rv.error())));
    }

    return rv.value();
}


ADM_return_t
set_dataset_information(const server& srv, ADM_job_t job, ADM_dataset_t target,
                        ADM_dataset_info_t info) {
    (void) srv;
    (void) job;
    (void) target;
    (void) info;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_set_dataset_information(...)");

    ADM_set_dataset_information_in_t in{};
    ADM_set_dataset_information_out_t out;

    in.info = "info";

    const auto rpc = endp.call("ADM_set_dataset_information", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_set_dataset_information() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_set_dataset_information() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
set_io_resources(const server& srv, ADM_job_t job, ADM_adhoc_storage_t tier,
                 ADM_adhoc_resources_t resources) {
    (void) srv;
    (void) job;
    (void) tier;
    (void) resources;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_set_io_resources(...)");

    ADM_set_io_resources_in_t in{};
    ADM_set_io_resources_out_t out;

    in.resources = "resources";

    const auto rpc = endp.call("ADM_set_io_resources", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_set_io_resources() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_set_io_resources() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
get_transfer_priority(const server& srv, ADM_job_t job, ADM_transfer_t transfer,
                      ADM_transfer_priority_t* priority) {
    (void) srv;
    (void) job;
    (void) transfer;
    (void) priority;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_get_transfer_priority(...)");

    ADM_get_transfer_priority_in_t in{};
    ADM_get_transfer_priority_out_t out;

    const auto rpc = endp.call("ADM_get_transfer_priority", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_get_transfer_priority() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_get_transfer_priority() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
set_transfer_priority(const server& srv, ADM_job_t job, ADM_transfer_t transfer,
                      int incr) {
    (void) srv;
    (void) job;
    (void) transfer;
    (void) incr;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_set_transfer_priority(...)");

    ADM_set_transfer_priority_in_t in{};
    ADM_set_transfer_priority_out_t out;

    const auto rpc = endp.call("ADM_set_transfer_priority", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_set_transfer_priority() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_set_transfer_priority() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
cancel_transfer(const server& srv, ADM_job_t job, ADM_transfer_t transfer) {

    (void) srv;
    (void) job;
    (void) transfer;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_cancel_transfer(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_cancel_transfer_in_t in{42};
    ADM_cancel_transfer_out_t out;

    const auto rpc = endp.call("ADM_cancel_transfer", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_cancel_transfer() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_cancel_transfer() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
get_pending_transfers(const server& srv, ADM_job_t job,
                      ADM_transfer_t** pending_transfers) {
    (void) srv;
    (void) job;
    (void) pending_transfers;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_get_pending_transfers(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_get_pending_transfers_in_t in{};
    ADM_get_pending_transfers_out_t out;

    const auto rpc = endp.call("ADM_get_pending_transfers", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_get_pending_transfers() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_get_pending_transfers() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
set_qos_constraints(const server& srv, ADM_job_t job, ADM_qos_entity_t entity,
                    ADM_qos_limit_t limit) {
    (void) srv;
    (void) job;
    (void) entity;
    (void) limit;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_set_qos_constraints(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_set_qos_constraints_in_t in{};
    ADM_set_qos_constraints_out_t out;

    in.scope = "dataset";
    in.qos_class = "class";
    in.class_value = "value";

    const auto rpc = endp.call("ADM_set_qos_constraints", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_set_qos_constraints() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_set_qos_constraints() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
get_qos_constraints(const server& srv, ADM_job_t job, ADM_qos_entity_t entity,
                    ADM_qos_limit_t** limits) {
    (void) srv;
    (void) job;
    (void) entity;
    (void) limits;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_get_qos_constraints(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_get_qos_constraints_in_t in{};
    ADM_get_qos_constraints_out_t out;

    in.scope = "dataset";

    const auto rpc = endp.call("ADM_get_qos_constraints", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_get_qos_constraints() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_get_qos_constraints() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
define_data_operation(const server& srv, ADM_job_t job, const char* path,
                      ADM_data_operation_t* op, va_list args) {
    (void) srv;
    (void) job;
    (void) path;
    (void) op;
    (void) args;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_define_data_operation(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_define_data_operation_in_t in{};
    ADM_define_data_operation_out_t out;

    in.path = path;
    in.operation_id = 1;
    in.arguments = "argument1 argument2";

    const auto rpc = endp.call("ADM_define_data_operation", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_define_data_operation() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_define_data_operation() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
connect_data_operation(const server& srv, ADM_job_t job, ADM_dataset_t input,
                       ADM_dataset_t output, bool should_stream, va_list args) {
    (void) srv;
    (void) job;
    (void) input;
    (void) output;
    (void) should_stream;
    (void) args;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_connect_data_operation(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_connect_data_operation_in_t in{};
    ADM_connect_data_operation_out_t out;

    in.input = "/tmp";
    in.arguments = "argument1 argument2";

    const auto rpc = endp.call("ADM_connect_data_operation", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_connect_data_operation() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_connect_data_operation() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
finalize_data_operation(const server& srv, ADM_job_t job,
                        ADM_data_operation_t op,
                        ADM_data_operation_status_t* status) {
    (void) srv;
    (void) job;
    (void) op;
    (void) status;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_finalize_data_operation(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_finalize_data_operation_in_t in{};
    ADM_finalize_data_operation_out_t out;

    const auto rpc = endp.call("ADM_finalize_data_operation", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_finalize_data_operation() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_finalize_data_operation() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
link_transfer_to_data_operation(const server& srv, ADM_job_t job,
                                ADM_data_operation_t op,
                                ADM_transfer_t transfer, bool should_stream,
                                va_list args) {
    (void) srv;
    (void) job;
    (void) op;
    (void) transfer;
    (void) should_stream;
    (void) args;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_link_transfer_to_data_operation(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_link_transfer_to_data_operation_in_t in{};
    ADM_link_transfer_to_data_operation_out_t out;

    in.arguments = "argument1 argument2";

    const auto rpc =
            endp.call("ADM_link_transfer_to_data_operation", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_link_transfer_to_data_operation() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_link_transfer_to_data_operation() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

ADM_return_t
get_statistics(const server& srv, ADM_job_t job, ADM_job_stats_t** stats) {
    (void) srv;
    (void) job;
    (void) stats;

#if 0
    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_get_statistics(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_get_statistics_in_t in{};
    ADM_get_statistics_out_t out;

    const auto rpc = endp.call("ADM_get_statistics", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_get_statistics() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_get_statistics() = {}", ADM_SUCCESS);
#endif
    return ADM_SUCCESS;
}

} // namespace scord
