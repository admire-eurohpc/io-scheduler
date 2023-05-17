/******************************************************************************
 * Copyright 2021-2023, Barcelona Supercomputing Center (BSC), Spain
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

#include <scord/types.hpp>
#include <net/request.hpp>
#include <net/endpoint.hpp>
#include <net/serialization.hpp>
#include "rpc_server.hpp"

using namespace std::literals;

struct remote_procedure {
    static std::uint64_t
    new_id() {
        static std::atomic_uint64_t current_id;
        return current_id++;
    }
};

namespace scord {

rpc_server::rpc_server(std::string name, std::string address, bool daemonize,
                       std::filesystem::path rundir)
    : server::server(std::move(name), std::move(address), std::move(daemonize),
                     std::move(rundir)),
      provider::provider(m_network_engine, 0) {

#define EXPAND(rpc_name) "ADM_" #rpc_name##s, &rpc_server::rpc_name

    provider::define(EXPAND(ping));
    provider::define(EXPAND(register_job));
    provider::define(EXPAND(update_job));
    provider::define(EXPAND(remove_job));
    provider::define(EXPAND(register_adhoc_storage));
    provider::define(EXPAND(update_adhoc_storage));
    provider::define(EXPAND(remove_adhoc_storage));
    provider::define(EXPAND(deploy_adhoc_storage));
    provider::define(EXPAND(tear_down_adhoc_storage));
    provider::define(EXPAND(register_pfs_storage));
    provider::define(EXPAND(update_pfs_storage));
    provider::define(EXPAND(remove_pfs_storage));
    provider::define(EXPAND(transfer_datasets));

#undef EXPAND
}

void
rpc_server::ping(const network::request& req) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)));

    const auto resp = generic_response{rpc_id, scord::error_code::success};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                scord::error_code::success);

    req.respond(resp);
}

void
rpc_server::register_job(const network::request& req,
                         const scord::job::resources& job_resources,
                         const scord::job::requirements& job_requirements,
                         scord::slurm_job_id slurm_id) {

    using network::get_address;
    using network::response_with_id;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{job_resources: {}, job_requirements: {}, slurm_id: "
                "{}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                job_resources, job_requirements, slurm_id);

    scord::error_code ec;
    std::optional<scord::job_id> job_id;

    if(const auto jm_result =
               m_job_manager.create(slurm_id, job_resources, job_requirements);
       jm_result.has_value()) {

        const auto& job_info = jm_result.value();

        // if the job requires an adhoc storage instance, inform the appropriate
        // adhoc_storage instance (if registered)
        if(job_requirements.adhoc_storage()) {
            const auto adhoc_id = job_requirements.adhoc_storage()->id();
            ec = m_adhoc_manager.add_client_info(adhoc_id, job_info);

            if(!ec) {
                goto respond;
            }
        }

        job_id = job_info->job().id();
    } else {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error creating job: {}\"", rpc_id,
                     jm_result.error());
        ec = jm_result.error();
    }

respond:
    const auto resp = response_with_id{rpc_id, ec, job_id};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}, job_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec, job_id);

    req.respond(resp);
}

void
rpc_server::update_job(const network::request& req, scord::job_id job_id,
                       const scord::job::resources& new_resources) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{job_id: {}, new_resources: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                job_id, new_resources);

    const auto ec = m_job_manager.update(job_id, new_resources);

    if(!ec) {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error updating job: {}\"", rpc_id,
                     ec);
    }

    const auto resp = generic_response{rpc_id, ec};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec);

    req.respond(resp);
}

void
rpc_server::remove_job(const network::request& req, scord::job_id job_id) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{job_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                job_id);

    scord::error_code ec;
    const auto jm_result = m_job_manager.remove(job_id);

    if(jm_result) {
        // if the job was using an adhoc storage instance, inform the
        // appropriate adhoc_storage that the job is no longer its client
        const auto& job_info = jm_result.value();

        if(const auto adhoc_storage = job_info->requirements()->adhoc_storage();
           adhoc_storage.has_value()) {
            ec = m_adhoc_manager.remove_client_info(adhoc_storage->id());
        }
    } else {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error removing job: {}\"", rpc_id,
                     job_id);
        ec = jm_result.error();
    }

    const auto resp = generic_response{rpc_id, ec};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec);

    req.respond(resp);
}

void
rpc_server::register_adhoc_storage(
        const network::request& req, const std::string& name,
        enum scord::adhoc_storage::type type,
        const scord::adhoc_storage::ctx& ctx,
        const scord::adhoc_storage::resources& resources) {

    using network::get_address;
    using network::response_with_id;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{name: {}, type: {}, adhoc_ctx: {}, "
                "adhoc_resources: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                name, type, ctx, resources);

    scord::error_code ec;
    std::optional<std::uint64_t> adhoc_id;

    if(const auto am_result =
               m_adhoc_manager.create(type, name, ctx, resources);
       am_result.has_value()) {
        const auto& adhoc_storage_info = am_result.value();
        adhoc_id = adhoc_storage_info->adhoc_storage().id();
    } else {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error creating adhoc_storage: "
                     "{}\"",
                     rpc_id, am_result.error());
        ec = am_result.error();
    }

    const auto resp = response_with_id{rpc_id, ec, adhoc_id};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}, adhoc_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec, adhoc_id);

    req.respond(resp);
}

void
rpc_server::update_adhoc_storage(
        const network::request& req, std::uint64_t adhoc_id,
        const scord::adhoc_storage::resources& new_resources) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{adhoc_id: {}, new_resources: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                adhoc_id, new_resources);

    const auto ec = m_adhoc_manager.update(adhoc_id, new_resources);

    if(!ec) {
        LOGGER_ERROR(
                "rpc id: {} error_msg: \"Error updating adhoc_storage: {}\"",
                rpc_id, ec);
    }

    const auto resp = generic_response{rpc_id, ec};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec);

    req.respond(resp);
}

void
rpc_server::remove_adhoc_storage(const network::request& req,
                                 std::uint64_t adhoc_id) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{adhoc_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                adhoc_id);

    scord::error_code ec = m_adhoc_manager.remove(adhoc_id);

    if(!ec) {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error removing job: {}\"", rpc_id,
                     adhoc_id);
    }

    const auto resp = generic_response{rpc_id, ec};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec);

    req.respond(resp);
}

void
rpc_server::deploy_adhoc_storage(const network::request& req,
                                 std::uint64_t adhoc_id) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{adhoc_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                adhoc_id);

    auto ec = scord::error_code::success;

    // contact the adhoc controller and ask it to deploy the adhoc storage
    if(const auto am_result = m_adhoc_manager.find(adhoc_id);
       am_result.has_value()) {
        const auto& adhoc_info = am_result.value();
        const auto adhoc_storage = adhoc_info->adhoc_storage();

        if(const auto lookup_rv =
                   lookup(adhoc_storage.context().controller_address());
           lookup_rv.has_value()) {
            const auto& endp = lookup_rv.value();

            LOGGER_INFO("rpc id: {} name: {} from: {} => "
                        "body: {{type: {}, ctx: {}, resources: {}}}",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(self_address()), adhoc_storage.type(),
                        adhoc_storage.context(), adhoc_storage.get_resources());

            if(const auto call_rv = endp.call(
                       "ADM_"s + __FUNCTION__, adhoc_storage.type(),
                       adhoc_storage.context(), adhoc_storage.get_resources());
               call_rv.has_value()) {

                const network::generic_response resp{call_rv.value()};
                ec = resp.error_code();

                LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                            "rpc id: {} name: {} from: {} <= "
                            "body: {{retval: {}}} [op_id: {}]",
                            rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                            std::quoted(endp.address()), ec, resp.op_id());
            }
        }
    } else {
        ec = am_result.error();
        LOGGER_ERROR("rpc id: {} name: {} to: {} <= "
                     "body: {{retval: {}}}",
                     rpc_id, std::quoted(rpc_name),
                     std::quoted(get_address(req)), ec);
    }

    const auto resp = generic_response{rpc_id, ec};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec);

    req.respond(resp);
}

void
rpc_server::tear_down_adhoc_storage(const network::request& req,
                                    std::uint64_t adhoc_id) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{adhoc_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                adhoc_id);

    // TODO: actually tear down the adhoc storage instance

    const auto resp = generic_response{rpc_id, scord::error_code::success};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                scord::error_code::success);

    req.respond(resp);
}

void
rpc_server::register_pfs_storage(const network::request& req,
                                 const std::string& name,
                                 enum scord::pfs_storage::type type,
                                 const scord::pfs_storage::ctx& ctx) {

    using network::get_address;
    using network::response_with_id;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{name: {}, type: {}, pfs_ctx: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                name, type, ctx);

    scord::error_code ec;
    std::optional<std::uint64_t> pfs_id = 0;

    if(const auto pm_result = m_pfs_manager.create(type, name, ctx);
       pm_result.has_value()) {
        const auto& adhoc_storage_info = pm_result.value();
        pfs_id = adhoc_storage_info->pfs_storage().id();
    } else {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error creating pfs_storage: {}\"",
                     rpc_id, pm_result.error());
        ec = pm_result.error();
    }

    const auto resp = response_with_id{rpc_id, ec, pfs_id};

    LOGGER_INFO("rpc id: {} name: {} to: {} => "
                "body: {{retval: {}, pfs_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec, pfs_id);

    req.respond(resp);
}

void
rpc_server::update_pfs_storage(const network::request& req,
                               std::uint64_t pfs_id,
                               const scord::pfs_storage::ctx& new_ctx) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{pfs_id: {}, new_ctx: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                pfs_id, new_ctx);

    const auto ec = m_pfs_manager.update(pfs_id, new_ctx);

    if(!ec) {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error updating pfs_storage: {}\"",
                     rpc_id, ec);
    }

    const auto resp = generic_response{rpc_id, ec};

    LOGGER_INFO("rpc id: {} name: {} to: {} => "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec);

    req.respond(resp);
}

void
rpc_server::remove_pfs_storage(const network::request& req,
                               std::uint64_t pfs_id) {

    using network::generic_response;
    using network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{pfs_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                pfs_id);

    scord::error_code ec = m_pfs_manager.remove(pfs_id);

    if(!ec) {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error removing pfs storage: {}\"",
                     rpc_id, pfs_id);
    }

    const auto resp = generic_response{rpc_id, ec};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec);

    req.respond(resp);
}

void
rpc_server::transfer_datasets(const network::request& req, scord::job_id job_id,
                              const std::vector<scord::dataset>& sources,
                              const std::vector<scord::dataset>& targets,
                              const std::vector<scord::qos::limit>& limits,
                              enum scord::transfer::mapping mapping) {

    using network::get_address;
    using network::response_with_id;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO(
            "rpc id: {} name: {} from: {} => "
            "body: {{job_id: {}, sources: {}, targets: {}, limits: {}, mapping: {}}}",
            rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
            job_id, sources, targets, limits, mapping);

    scord::error_code ec;

    std::optional<std::uint64_t> tx_id;

    // TODO: generate a global ID for the transfer and contact Cargo to
    // actually request it
    tx_id = 42;

    const auto resp = response_with_id{rpc_id, ec, tx_id};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}, tx_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec, tx_id);

    req.respond(resp);
}

} // namespace scord
