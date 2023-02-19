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
#include <net/request.hpp>
#include "rpc_handlers.hpp"
#include "job_manager.hpp"
#include "adhoc_storage_manager.hpp"
#include "pfs_storage_manager.hpp"

// Process running
#include <unistd.h>
#include <sys/wait.h>

using namespace std::literals;

struct remote_procedure {
    static std::uint64_t
    new_id() {
        static std::atomic_uint64_t current_id;
        return current_id++;
    }
};

namespace scord::network::handlers {

void
ping(const scord::network::request& req) {

    using scord::network::generic_response;
    using scord::network::get_address;

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
register_job(const scord::network::request& req,
             const scord::job::resources& job_resources,
             const scord::job::requirements& job_requirements,
             scord::slurm_job_id slurm_id) {

    using scord::network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{job_resources: {}, job_requirements: {}, slurm_id: "
                "{}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                job_resources, job_requirements, slurm_id);

    scord::error_code ec;
    std::optional<scord::job_id> job_id;
    auto& jm = scord::job_manager::instance();

    if(const auto jm_result =
               jm.create(slurm_id, job_resources, job_requirements);
       jm_result.has_value()) {

        const auto& job_info = jm_result.value();

        // if the job requires an adhoc storage instance, inform the appropriate
        // adhoc_storage instance (if registered)
        if(job_requirements.adhoc_storage()) {
            const auto adhoc_id = job_requirements.adhoc_storage()->id();
            auto& adhoc_manager = scord::adhoc_storage_manager::instance();
            ec = adhoc_manager.add_client_info(adhoc_id, job_info);
        }

        job_id = job_info->job().id();
    } else {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error creating job: {}\"", rpc_id,
                     jm_result.error());
        ec = jm_result.error();
    }

    const auto resp = response_with_id{rpc_id, ec, job_id};

    LOGGER_INFO("rpc id: {} name: {} to: {} <= "
                "body: {{retval: {}, job_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                ec, job_id);

    req.respond(resp);
}

void
update_job(const request& req, scord::job_id job_id,
           const scord::job::resources& new_resources) {

    using scord::network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{job_id: {}, new_resources: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                job_id, new_resources);

    auto& jm = scord::job_manager::instance();
    const auto ec = jm.update(job_id, new_resources);

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
remove_job(const request& req, scord::job_id job_id) {

    using scord::network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{job_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                job_id);

    scord::error_code ec;
    auto& jm = scord::job_manager::instance();
    const auto jm_result = jm.remove(job_id);

    if(jm_result) {
        // if the job was using an adhoc storage instance, inform the
        // appropriate adhoc_storage that the job is no longer its client
        const auto& job_info = jm_result.value();

        if(const auto adhoc_storage = job_info->requirements()->adhoc_storage();
           adhoc_storage.has_value()) {
            auto& adhoc_manager = scord::adhoc_storage_manager::instance();
            ec = adhoc_manager.remove_client_info(adhoc_storage->id());
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
register_adhoc_storage(const request& req, const std::string& name,
                       enum scord::adhoc_storage::type type,
                       const scord::adhoc_storage::ctx& ctx) {

    using scord::network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{name: {}, type: {}, adhoc_ctx: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                name, type, ctx);

    scord::error_code ec;
    std::optional<std::uint64_t> adhoc_id;
    auto& adhoc_manager = scord::adhoc_storage_manager::instance();

    if(const auto am_result = adhoc_manager.create(type, name, ctx);
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
update_adhoc_storage(const request& req, std::uint64_t adhoc_id,
                     const scord::adhoc_storage::ctx& new_ctx) {

    using scord::network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{adhoc_id: {}, new_ctx: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                adhoc_id, new_ctx);

    auto& adhoc_manager = scord::adhoc_storage_manager::instance();
    const auto ec = adhoc_manager.update(adhoc_id, new_ctx);

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
remove_adhoc_storage(const request& req, std::uint64_t adhoc_id) {

    using scord::network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{adhoc_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                adhoc_id);

    auto& adhoc_manager = scord::adhoc_storage_manager::instance();
    scord::error_code ec = adhoc_manager.remove(adhoc_id);

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
deploy_adhoc_storage(const request& req, std::uint64_t adhoc_id) {

    using scord::network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{adhoc_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                adhoc_id);

    auto ec = scord::error_code::success;
    auto& adhoc_manager = scord::adhoc_storage_manager::instance();

    if(const auto am_result = adhoc_manager.find(adhoc_id);
       am_result.has_value()) {
        const auto& storage_info = am_result.value();
        const auto adhoc_storage = storage_info->adhoc_storage();

        if(adhoc_storage.type() == scord::adhoc_storage::type::gekkofs) {
            const auto adhoc_ctx = adhoc_storage.context();
            /* Number of nodes */
            const std::string nodes =
                    std::to_string(adhoc_ctx.resources().nodes().size());

            /* Walltime */
            const std::string walltime = std::to_string(adhoc_ctx.walltime());

            /* Launch script */
            switch(const auto pid = fork()) {
                case 0: {
                    std::vector<const char*> args;
                    args.push_back("gkfs");
                    // args.push_back("-c");
                    // args.push_back("gkfs.conf");
                    args.push_back("-n");
                    args.push_back(nodes.c_str());
                    // args.push_back("-w");
                    // args.push_back(walltime.c_str());
                    args.push_back("--srun");
                    args.push_back("start");
                    args.push_back(NULL);
                    std::vector<const char*> env;
                    env.push_back(NULL);

                    execvpe("gkfs", const_cast<char* const*>(args.data()),
                            const_cast<char* const*>(env.data()));
                    LOGGER_INFO(
                            "ADM_deploy_adhoc_storage() script didn't execute");
                    exit(EXIT_FAILURE);
                    break;
                }
                case -1: {
                    ec = scord::error_code::other;
                    LOGGER_ERROR("rpc id: {} name: {} to: {} <= "
                                 "body: {{retval: {}}}",
                                 rpc_id, std::quoted(rpc_name),
                                 std::quoted(get_address(req)), ec);
                    break;
                }
                default: {
                    int wstatus = 0;
                    pid_t retwait = waitpid(pid, &wstatus, 0);
                    if(retwait == -1) {
                        LOGGER_ERROR(
                                "rpc id: {} error_msg: \"Error waitpid code: {}\"",
                                rpc_id, retwait);
                        ec = scord::error_code::other;
                    } else {
                        if(WEXITSTATUS(wstatus) != 0) {
                            ec = scord::error_code::other;
                        } else {
                            ec = scord::error_code::success;
                        }
                    }
                    break;
                }
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
register_pfs_storage(const request& req, const std::string& name,
                     enum scord::pfs_storage::type type,
                     const scord::pfs_storage::ctx& ctx) {

    using scord::network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{name: {}, type: {}, pfs_ctx: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                name, type, ctx);

    scord::error_code ec;
    std::optional<std::uint64_t> pfs_id = 0;
    auto& pfs_manager = scord::pfs_storage_manager::instance();

    if(const auto pm_result = pfs_manager.create(type, name, ctx);
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
update_pfs_storage(const request& req, std::uint64_t pfs_id,
                   const scord::pfs_storage::ctx& new_ctx) {

    using scord::network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{pfs_id: {}, new_ctx: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                pfs_id, new_ctx);

    auto& pfs_manager = scord::pfs_storage_manager::instance();
    const auto ec = pfs_manager.update(pfs_id, new_ctx);

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
remove_pfs_storage(const request& req, std::uint64_t pfs_id) {

    using scord::network::get_address;

    const auto rpc_name = "ADM_"s + __FUNCTION__;
    const auto rpc_id = remote_procedure::new_id();

    LOGGER_INFO("rpc id: {} name: {} from: {} => "
                "body: {{pfs_id: {}}}",
                rpc_id, std::quoted(rpc_name), std::quoted(get_address(req)),
                pfs_id);

    auto& pfs_manager = scord::pfs_storage_manager::instance();
    scord::error_code ec = pfs_manager.remove(pfs_id);

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
transfer_datasets(const request& req, scord::job_id job_id,
                  const std::vector<scord::dataset>& sources,
                  const std::vector<scord::dataset>& targets,
                  const std::vector<scord::qos::limit>& limits,
                  enum scord::transfer::mapping mapping) {

    using scord::network::get_address;

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

} // namespace scord::network::handlers
