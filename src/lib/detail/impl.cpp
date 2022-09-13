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

#include <tl/expected.hpp>
#include <net/engine.hpp>
#include <net/proto/rpc_types.h>
#include <api/convert.hpp>
#include <admire_types.hpp>
#include "impl.hpp"

void
rpc_registration_cb(scord::network::rpc_client* client) {

    REGISTER_RPC(client, "ADM_ping", void, void, NULL, false);

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

    REGISTER_RPC(client, "ADM_transfer_dataset", ADM_transfer_dataset_in_t,
                 ADM_transfer_dataset_out_t, NULL, true);

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

namespace admire::detail {

admire::error_code
ping(const server& srv) {

    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("ADM_ping()");
    const auto rpc = endp.call("ADM_ping");

    LOGGER_INFO("ADM_register_job() = {}", ADM_SUCCESS);
    return ADM_SUCCESS;
}

tl::expected<admire::job, admire::error_code>
register_job(const admire::server& srv, const admire::job_requirements& reqs) {

    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("RPC (ADM_{}) => {{job_requirements: {}}}", __FUNCTION__, reqs);

    auto rpc_reqs = api::convert(reqs);

    ADM_register_job_in_t in{*rpc_reqs.get()};
    ADM_register_job_out_t out;

    const auto rpc = endp.call("ADM_register_job", &in, &out);

    if(out.retval < 0) {
        LOGGER_ERROR("RPC (ADM_{}) <= {}", __FUNCTION__, out.retval);
        return tl::make_unexpected(static_cast<admire::error_code>(out.retval));
    }

    const admire::job job = api::convert(out.job);

    LOGGER_INFO("RPC (ADM_{}) <= {{retval: {}, job: {}}}", __FUNCTION__,
                ADM_SUCCESS, job.id());

    return job;
}

admire::error_code
update_job(const server& srv, const job& job, const job_requirements& reqs) {

    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("RPC (ADM_{}): {{job: {}, job_requirements: {}}}", __FUNCTION__,
                job, reqs);

    const auto rpc_job = api::convert(job);
    const auto rpc_reqs = api::convert(reqs);

    ADM_update_job_in_t in{rpc_job.get(), *rpc_reqs.get()};
    ADM_update_job_out_t out;

    const auto rpc = endp.call("ADM_update_job", &in, &out);


    if(out.retval < 0) {
        const auto retval = static_cast<admire::error_code>(out.retval);
        LOGGER_ERROR("RPC (ADM_{}) <= {{retval: {}}}", __FUNCTION__, retval);
        return retval;
    }

    LOGGER_INFO("RPC (ADM_{}) <= {{retval: {}}}", __FUNCTION__, ADM_SUCCESS);
    return ADM_SUCCESS;
}

admire::error_code
remove_job(const server& srv, const job& job) {

    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("RPC (ADM_{}) => {{job: {}}}", __FUNCTION__, job);

    const auto rpc_job = api::convert(job);

    ADM_remove_job_in_t in{rpc_job.get()};
    ADM_remove_job_out_t out;

    const auto rpc = endp.call("ADM_remove_job", &in, &out);

    if(out.retval < 0) {
        const auto retval = static_cast<admire::error_code>(out.retval);
        LOGGER_ERROR("RPC (ADM_{}) <= {{retval: {}}}", __FUNCTION__, retval);
        return retval;
    }

    LOGGER_INFO("RPC (ADM_{}) <= {{retval: {}}}", __FUNCTION__, ADM_SUCCESS);
    return ADM_SUCCESS;
}

tl::expected<transfer, error_code>
transfer_dataset(const server& srv, const job& job,
                 const std::vector<dataset>& sources,
                 const std::vector<dataset>& targets,
                 const std::vector<qos::limit>& limits,
                 transfer::mapping mapping) {

    scord::network::rpc_client rpc_client{srv.protocol(), rpc_registration_cb};

    auto endp = rpc_client.lookup(srv.address());

    LOGGER_INFO("RPC (ADM_{}) => {{job: {}, sources: {}, targets: {}, "
                "limits: {}, mapping: {}}}",
                __FUNCTION__, job, sources, targets, limits, mapping);

    const auto rpc_job = api::convert(job);
    const auto rpc_sources = api::convert(sources);
    const auto rpc_targets = api::convert(targets);
    const auto rpc_qos_limits = api::convert(limits);

    ADM_transfer_dataset_in_t in{rpc_job.get(), rpc_sources.get(),
                                 rpc_targets.get(), rpc_qos_limits.get(),
                                 static_cast<ADM_transfer_mapping_t>(mapping)};
    ADM_transfer_dataset_out_t out;

    [[maybe_unused]] const auto rpc =
            endp.call("ADM_transfer_dataset", &in, &out);

    if(out.retval < 0) {
        LOGGER_ERROR("RPC (ADM_{}) <= {{retval: {}}}", __FUNCTION__,
                     out.retval);
        return tl::make_unexpected(static_cast<admire::error_code>(out.retval));
    }

    const admire::transfer tx = api::convert(out.tx);

    LOGGER_INFO("RPC (ADM_{}) <= {{retval: {}, transfer: {}}}", __FUNCTION__,
                ADM_SUCCESS, tx);
    return tx;
}


} // namespace admire::detail
