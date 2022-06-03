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

#include <admire.hpp>
#include <engine.hpp>
#include <logger.hpp>


namespace {

void
init_library() __attribute__((constructor));

void
init_logger();

void
init_library() {
    init_logger();
}

/** Logging for the library */
void
init_logger() {
    // for now, just create a simple console logger
    scord::logger::create_global_logger("libadm_iosched", "console color");
}

} // namespace


namespace admire {

ADM_return_t
register_job(const server& srv, ADM_job_requirements_t reqs,
             ADM_job_handle_t* job) {
    (void) srv;
    (void) reqs;
    (void) job;

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_register_job(...)");

    ADM_register_job_in_t in{};
    ADM_register_job_out_t out;

    endp.call("ADM_register_job", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_register_job() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_register_job() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
update_job(const server& srv, ADM_job_handle_t job,
           ADM_job_requirements_t reqs) {
    (void) srv;
    (void) job;
    (void) reqs;

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_update_job(...)");

    ADM_update_job_in_t in{};
    ADM_update_job_out_t out;

    endp.call("ADM_update_job", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_update_job() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_update_job() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
remove_job(const server& srv, ADM_job_handle_t job) {
    (void) srv;
    (void) job;

    return ADM_OTHER_ERROR;
}

ADM_return_t
register_adhoc_storage(const server& srv, ADM_job_handle_t job,
                       ADM_adhoc_context_t ctx,
                       ADM_adhoc_storage_handle_t* adhoc_handle) {
    (void) srv;
    (void) job;
    (void) ctx;
    (void) adhoc_handle;

    return ADM_OTHER_ERROR;
}

ADM_return_t
update_adhoc_storage(const server& srv, ADM_job_handle_t job,
                     ADM_adhoc_context_t ctx,
                     ADM_adhoc_storage_handle_t adhoc_handle) {
    (void) srv;
    (void) job;
    (void) ctx;
    (void) adhoc_handle;

    return ADM_OTHER_ERROR;
}

ADM_return_t
remove_adhoc_storage(const server& srv, ADM_job_handle_t job,
                     ADM_adhoc_storage_handle_t adhoc_handle) {
    (void) srv;
    (void) job;
    (void) adhoc_handle;

    return ADM_OTHER_ERROR;
}

ADM_return_t
deploy_adhoc_storage(const server& srv, ADM_job_handle_t job,
                     ADM_adhoc_storage_handle_t adhoc_handle) {
    (void) srv;
    (void) job;
    (void) adhoc_handle;

    return ADM_OTHER_ERROR;
}

ADM_return_t
transfer_dataset(const server& srv, ADM_job_handle_t job,
                 ADM_dataset_handle_t** sources, ADM_dataset_handle_t** targets,
                 ADM_limit_t** limits, ADM_tx_mapping_t mapping,
                 ADM_transfer_handle_t* tx_handle) {
    (void) srv;
    (void) job;
    (void) sources;
    (void) targets;
    (void) limits;
    (void) mapping;
    (void) tx_handle;

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_transfer_dataset(...)");

    ADM_transfer_dataset_in_t in{};
    ADM_transfer_dataset_out_t out;

    endp.call("ADM_transfer_dataset", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_transfer_dataset() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_transfer_dataset() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
set_dataset_information(const server& srv, ADM_job_handle_t job,
                        ADM_dataset_handle_t target, ADM_dataset_info_t info) {
    (void) srv;
    (void) job;
    (void) target;
    (void) info;

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_set_dataset_information(...)");

    ADM_set_dataset_information_in_t in{};
    ADM_set_dataset_information_out_t out;

    endp.call("ADM_set_dataset_information", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_set_dataset_information() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_set_dataset_information() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
set_io_resources(const server& srv, ADM_job_handle_t job,
                 ADM_storage_handle_t tier, ADM_storage_resources_t resources) {
    (void) srv;
    (void) job;
    (void) tier;
    (void) resources;

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_set_io_resources(...)");

    ADM_set_io_resources_in_t in{};
    ADM_set_io_resources_out_t out;

    endp.call("ADM_set_io_resources", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_set_io_resources() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_set_io_resources() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
get_transfer_priority(const server& srv, ADM_job_handle_t job,
                      ADM_transfer_handle_t tx_handle,
                      ADM_transfer_priority_t* priority) {
    (void) srv;
    (void) job;
    (void) tx_handle;
    (void) priority;

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_get_transfer_priority(...)");

    ADM_get_transfer_priority_in_t in{};
    ADM_get_transfer_priority_out_t out;

    endp.call("ADM_get_transfer_priority", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_get_transfer_priority() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_get_transfer_priority() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
set_transfer_priority(const server& srv, ADM_job_handle_t job,
                      ADM_transfer_handle_t tx_handle, int incr) {
    (void) srv;
    (void) job;
    (void) tx_handle;
    (void) incr;

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_set_transfer_priority(...)");

    ADM_set_transfer_priority_in_t in{};
    ADM_set_transfer_priority_out_t out;

    endp.call("ADM_set_transfer_priority", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_set_transfer_priority() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_set_transfer_priority() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
cancel_transfer(const server& srv, ADM_job_handle_t job,
                ADM_transfer_handle_t tx_handle) {

    (void) job;
    (void) tx_handle;

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_cancel_transfer(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_cancel_transfer_in_t in{42};
    ADM_cancel_transfer_out_t out;

    endp.call("ADM_cancel_transfer", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_cancel_transfer() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_cancel_transfer() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
get_pending_transfers(const server& srv, ADM_job_handle_t job,
                      ADM_transfer_handle_t** pending_transfers) {
    (void) srv;
    (void) job;
    (void) pending_transfers;

    scord::network::rpc_client rpc_client{srv.m_protocol};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_get_pending_transfers(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_get_pending_transfers_in_t in{};
    ADM_get_pending_transfers_out_t out;

    endp.call("ADM_get_pending_transfers", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_get_pending_transfers() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_get_pending_transfers() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
set_qos_constraints(const server& srv, ADM_job_handle_t job,
                    ADM_limit_t limit) {
    (void) srv;
    (void) job;
    (void) limit;

    scord::network::rpc_client rpc_client{srv.m_protocol};

    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_set_qos_constraints(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_set_qos_constraints_in_t in{};
    ADM_set_qos_constraints_out_t out;

    endp.call("ADM_set_qos_constraints", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_set_qos_constraints() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_set_qos_constraints() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
get_qos_constraints(const server& srv, ADM_job_handle_t job,
                    ADM_qos_scope_t scope, ADM_qos_entity_t entity,
                    ADM_limit_t** limits) {
    (void) srv;
    (void) job;
    (void) scope;
    (void) entity;
    (void) limits;

    scord::network::rpc_client rpc_client{srv.m_protocol};

    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_get_qos_constraints(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_get_qos_constraints_in_t in{};
    ADM_get_qos_constraints_out_t out;

    endp.call("ADM_get_qos_constraints", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_get_qos_constraints() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_get_qos_constraints() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
define_data_operation(const server& srv, ADM_job_handle_t job, const char* path,
                      ADM_data_operation_handle_t* op, va_list args) {
    (void) srv;
    (void) job;
    (void) path;
    (void) op;
    (void) args;

    scord::network::rpc_client rpc_client{srv.m_protocol};

    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_define_data_operation(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_define_data_operation_in_t in{};
    ADM_define_data_operation_out_t out;

    endp.call("ADM_define_data_operation", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_define_data_operation() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_define_data_operation() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
connect_data_operation(const server& srv, ADM_job_handle_t job,
                       ADM_dataset_handle_t input, ADM_dataset_handle_t output,
                       bool should_stream, va_list args) {
    (void) srv;
    (void) job;
    (void) input;
    (void) output;
    (void) should_stream;
    (void) args;

    scord::network::rpc_client rpc_client{srv.m_protocol};

    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_connect_data_operation(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_connect_data_operation_in_t in{};
    ADM_connect_data_operation_out_t out;

    endp.call("ADM_connect_data_operation", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_connect_data_operation() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_connect_data_operation() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
finalize_data_operation(const server& srv, ADM_job_handle_t job,
                        ADM_data_operation_handle_t op,
                        ADM_data_operation_status_t* status) {
    (void) srv;
    (void) job;
    (void) op;
    (void) status;

    scord::network::rpc_client rpc_client{srv.m_protocol};

    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_finalize_data_operation(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_finalize_data_operation_in_t in{};
    ADM_finalize_data_operation_out_t out;

    endp.call("ADM_finalize_data_operation", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_finalize_data_operation() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_finalize_data_operation() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
link_transfer_to_data_operation(const server& srv, ADM_job_handle_t job,
                                ADM_data_operation_handle_t op,
                                bool should_stream, va_list args) {
    (void) srv;
    (void) job;
    (void) op;
    (void) should_stream;
    (void) args;

    scord::network::rpc_client rpc_client{srv.m_protocol};

    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_link_transfer_to_data_operation(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_link_transfer_to_data_operation_in_t in{};
    ADM_link_transfer_to_data_operation_out_t out;

    endp.call("ADM_link_transfer_to_data_operation", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_link_transfer_to_data_operation() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_link_transfer_to_data_operation() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

ADM_return_t
get_statistics(const server& srv, ADM_job_handle_t job,
               ADM_job_stats_t** stats) {
    (void) srv;
    (void) job;
    (void) stats;

    scord::network::rpc_client rpc_client{srv.m_protocol};

    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(srv.m_address);

    LOGGER_INFO("ADM_get_statistics(...)");

    // FIXME: change RPC fields to ADM_transfer_handle_t
    ADM_get_statistics_in_t in{};
    ADM_get_statistics_out_t out;

    endp.call("ADM_get_statistics", &in, &out);

    if(out.ret < 0) {
        LOGGER_ERROR("ADM_get_statistics() = {}", out.ret);
        return static_cast<ADM_return_t>(out.ret);
    }

    LOGGER_INFO("ADM_get_statistics() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

} // namespace admire
