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
#include <net/utilities.hpp>
#include <cargo/cargo.hpp>
#include "rpc_server.hpp"
#include <abt_cxx/shared_mutex.hpp>

template <typename T, typename E>
constexpr std::optional<T>
value_or_none(tl::expected<T, E>&& e) {
    if(e.has_value()) {
        return e.value();
    }
    return std::nullopt;
}

using namespace std::literals;

namespace {
cargo::dataset
dataset_process(std::string id) {

    cargo::dataset::type type = cargo::dataset::type::posix;
    if(id.find("lustre:") != std::string::npos) {
        id = id.substr(strlen("lustre:"));
        type = cargo::dataset::type::parallel;
    } else if(id.find("gekkofs:") != std::string::npos) {
        id = id.substr(strlen("gekkofs:"));
        type = cargo::dataset::type::posix;
    } else if(id.find("hercules:") != std::string::npos) {
        id = id.substr(strlen("hercules:"));
        type = cargo::dataset::type::hercules;
    } else if(id.find("expand:") != std::string::npos) {
        id = id.substr(strlen("expand:"));
        type = cargo::dataset::type::expand;
    } else if(id.find("dataclay:") != std::string::npos) {
        id = id.substr(strlen("dataclay:"));
        type = cargo::dataset::type::dataclay;
    } else
        type = cargo::dataset::type::posix;

    return cargo::dataset{id, type};
}
} // namespace
namespace scord {

rpc_server::rpc_server(std::string name, std::string address, bool daemonize,
                       std::filesystem::path rundir, std::string redis_address)
    : server::server(std::move(name), std::move(address), std::move(daemonize),
                     std::move(rundir)),
      provider::provider(m_network_engine, 0),
      m_scheduler_ess(thallium::xstream::create()),
      m_scheduler_ult(
              m_scheduler_ess->make_thread([this]() { scheduler_update(); })),
      m_redis_address(std::move(redis_address)) {


#define EXPAND(rpc_name) "ADM_" #rpc_name##s, &rpc_server::rpc_name

    provider::define(EXPAND(ping));
    provider::define(EXPAND(query));
    provider::define(EXPAND(register_job));
    provider::define(EXPAND(update_job));
    provider::define(EXPAND(remove_job));
    provider::define(EXPAND(register_adhoc_storage));
    provider::define(EXPAND(update_adhoc_storage));
    provider::define(EXPAND(remove_adhoc_storage));
    provider::define(EXPAND(deploy_adhoc_storage));
    provider::define(EXPAND(terminate_adhoc_storage));
    provider::define(EXPAND(register_pfs_storage));
    provider::define(EXPAND(update_pfs_storage));
    provider::define(EXPAND(remove_pfs_storage));
    provider::define(EXPAND(transfer_datasets));

#undef EXPAND
    m_network_engine.push_prefinalize_callback([this]() {
        m_scheduler_ult->join();
        m_scheduler_ult = thallium::managed<thallium::thread>{};
        m_scheduler_ess->join();
        m_scheduler_ess = thallium::managed<thallium::xstream>{};
    });
}


#define RPC_NAME() ("ADM_"s + __FUNCTION__)

void
rpc_server::init_redis() {

    try {
        m_redis = sw::redis::Redis(m_redis_address);
        // Try the connection
        m_redis.value().dbsize();
    } catch(const sw::redis::Error& e) {
        LOGGER_CRITICAL("Redis not initialized at address {}", m_redis_address);
        m_redis = std::nullopt;
    }
}
void
rpc_server::ping(const network::request& req) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{}}", rpc);

    const auto resp = generic_response{rpc.id(), scord::error_code::success};

    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc,
                scord::error_code::success);

    req.respond(resp);
}

void
rpc_server::query(const network::request& req, slurm_job_id job_id) {

    using network::get_address;
    using network::rpc_info;
    using response_type = network::response_with_value<scord::job_info>;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{slurm_job_id: {}}}", rpc, job_id);

    const auto rv =
            m_job_manager.find_by_slurm_id(job_id)
                    .or_else([&](auto&& ec) {
                        LOGGER_ERROR("Error retrieving job metadata: {}", ec);
                    })
                    .and_then([&](auto&& job_metadata_ptr)
                                      -> tl::expected<job_info, error_code> {
                        if(!job_metadata_ptr->resources()) {
                            return tl::make_unexpected(
                                    error_code::no_resources);
                        }
                        return job_info{
                                job_metadata_ptr->adhoc_storage_metadata()
                                        ->controller_address(),
                                job_metadata_ptr->io_procs()};
                    });

    const response_type resp =
            rv ? response_type{rpc.id(), error_code::success, rv.value()}
               : response_type{rpc.id(), rv.error()};

    LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                "rpc {:<} body: {{retval: {}, job_info: {}}}", rpc,
                resp.error_code(), resp.value_or_none());

    req.respond(resp);
}

void
rpc_server::register_job(const network::request& req,
                         const scord::job::resources& job_resources,
                         const scord::job::requirements& job_requirements,
                         scord::slurm_job_id slurm_id) {

    using network::get_address;
    using network::response_with_id;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{job_resources: {}, job_requirements: {}, "
                "slurm_id: {}}}",
                rpc, job_resources, job_requirements, slurm_id);

    scord::error_code ec;
    std::optional<scord::job_id> job_id;

    std::shared_ptr<internal::adhoc_storage_metadata> adhoc_metadata_ptr;

    // If the job requires an adhoc storage instance, find the appropriate
    // adhoc_storage metadata so that we can associate it with the job_metadata
    // we are about to create
    if(job_requirements.adhoc_storage()) {
        const auto adhoc_id = job_requirements.adhoc_storage()->id();
        if(const auto am_result = m_adhoc_manager.find(adhoc_id);
           am_result.has_value()) {
            adhoc_metadata_ptr = am_result.value();
        } else {
            LOGGER_ERROR(
                    "rpc id: {} error_msg: \"Error finding adhoc_storage: {}\"",
                    rpc.id(), am_result.error());
            ec = am_result.error();
        }

        if(!ec) {
            goto respond;
        }
    }

    if(const auto jm_result = m_job_manager.create(
               slurm_id, job_resources, job_requirements, adhoc_metadata_ptr);
       jm_result.has_value()) {

        const auto& job_metadata_ptr = jm_result.value();

        // if the job requires an adhoc storage instance, inform the appropriate
        // adhoc_storage instance (if registered)
        if(adhoc_metadata_ptr) {
            adhoc_metadata_ptr->add_client_info(job_metadata_ptr);
        }

        job_id = job_metadata_ptr->job().id();

        if(m_redis and job_id) {
            const auto adhoc_id = job_requirements.adhoc_storage()->id();
            auto ec = m_adhoc_manager.find(adhoc_id);
            const auto timestamp =
                    std::chrono::system_clock::now().time_since_epoch().count();
            auto name = ec->get()->adhoc_storage().name();
            std::string type =
                    fmt::format("{}", ec->get()->adhoc_storage().type());

            std::unordered_map<std::string, std::string> m = {
                    {"timestamp", std::to_string(timestamp)},
                    {"job_id", std::to_string(job_id.value())},
                    {"AdhocID", std::to_string(adhoc_id)},
                    {"AdhocUUID", ec->get()->uuid()},
                    {"AdhocName", name},
                    {"Type", type},     // Lustre // Gekko
                    {"Deployed", "No"}, // No // Yes
                    {"StartTime", ""},
                    {"EndTime", ""}, // Or Running
                    {"Policies", ""} //

            };

            m_redis.value().hmset(std::to_string(slurm_id), m.begin(),
                                  m.end());
        }

    } else {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error creating job: {}\"",
                     rpc.id(), jm_result.error());
        ec = jm_result.error();
    }

respond:
    const auto resp = response_with_id{rpc.id(), ec, job_id};

    LOGGER_INFO("rpc {:<} body: {{retval: {}, job_id: {}}}", rpc, ec, job_id);

    req.respond(resp);
}

void
rpc_server::update_job(const network::request& req, scord::job_id job_id,
                       const scord::job::resources& new_resources) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{job_id: {}, new_resources: {}}}", rpc, job_id,
                new_resources);

    const auto ec = m_job_manager.update(job_id, new_resources);

    if(!ec) {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error updating job: {}\"",
                     rpc.id(), ec);
    }

    const auto resp = generic_response{rpc.id(), ec};

    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc, ec);

    req.respond(resp);
}

void
rpc_server::remove_job(const network::request& req, scord::job_id job_id) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{job_id: {}}}", rpc, job_id);

    scord::error_code ec;
    const auto jm_result = m_job_manager.remove(job_id);

    if(jm_result) {
        // if the job was using an adhoc storage instance, inform the
        // appropriate adhoc_storage that the job is no longer its client
        const auto& job_metadata_ptr = jm_result.value();

        if(const auto adhoc_storage =
                   job_metadata_ptr->requirements()->adhoc_storage();
           adhoc_storage.has_value()) {
            ec = m_adhoc_manager.remove_client_info(adhoc_storage->id());
        }
    } else {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error removing job: {}\"",
                     rpc.id(), job_id);
        ec = jm_result.error();
    }

    const auto resp = generic_response{rpc.id(), ec};

    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc, ec);

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
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{name: {:?}, type: {}, adhoc_ctx: {}, "
                "adhoc_resources: {}}}",
                rpc, name, type, ctx, resources);

    scord::error_code ec;
    std::optional<std::uint64_t> adhoc_id;

    if(const auto am_result =
               m_adhoc_manager.create(type, name, ctx, resources);
       am_result.has_value()) {
        const auto& adhoc_metadata_ptr = am_result.value();
        adhoc_id = adhoc_metadata_ptr->adhoc_storage().id();
    } else {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error creating adhoc_storage: "
                     "{}\"",
                     rpc.id(), am_result.error());
        ec = am_result.error();
    }

    const auto resp = response_with_id{rpc.id(), ec, adhoc_id};

    LOGGER_INFO("rpc {:<} body: {{retval: {}, adhoc_id: {}}}", rpc, ec,
                adhoc_id);

    req.respond(resp);
}

void
rpc_server::update_adhoc_storage(
        const network::request& req, std::uint64_t adhoc_id,
        const scord::adhoc_storage::resources& new_resources) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{adhoc_id: {}, new_resources: {}}}", rpc,
                adhoc_id, new_resources);


    const auto pre_ec = m_adhoc_manager.find(adhoc_id);

    if(!pre_ec) {
        LOGGER_ERROR(
                "rpc id: {} error_msg: \"Error updating adhoc_storage: {}\"",
                rpc.id(), scord::error_code::no_such_entity);
    }

    const auto old_resources_size = pre_ec.value()
                                            .get()
                                            ->adhoc_storage()
                                            .get_resources()
                                            .nodes()
                                            .size();

    const auto ec = m_adhoc_manager.update(adhoc_id, new_resources);

    if(!ec) {
        LOGGER_ERROR(
                "rpc id: {} error_msg: \"Error updating adhoc_storage: {}\"",
                rpc.id(), ec);
    }

    bool expand = new_resources.nodes().size() > old_resources_size;

    /**
     * @brief Helper lambda to contact the adhoc controller and prompt it to
     * update an adhoc storage instance
     * @param adhoc_storage The relevant `adhoc_storage` object with
     * information about the instance to deploy.
     * @return
     */
    const auto update_helper = [&](const auto& adhoc_metadata_ptr)
            -> tl::expected<error_code, error_code> {
        assert(adhoc_metadata_ptr);
        const auto adhoc_storage = adhoc_metadata_ptr->adhoc_storage();
        const auto endp = lookup(adhoc_storage.context().controller_address());

        if(!endp) {
            LOGGER_ERROR("endpoint lookup failed");
            return tl::make_unexpected(scord::error_code::snafu);
        }

        // const auto child_rpc =
        //       rpc.add_child(adhoc_storage.context().controller_address());

        auto name = "ADM_expand_adhoc_storage";
        if(!expand) {
            name = "ADM_shrink_adhoc_storage";
        }

        const auto child_rpc = rpc_info::create(
                name, adhoc_storage.context().controller_address());

        LOGGER_INFO("rpc {:<} body: {{uuid: {:?}, type: {}, resources: {}}}",
                    child_rpc, adhoc_metadata_ptr->uuid(), adhoc_storage.type(),
                    adhoc_storage.get_resources());

        if(const auto call_rv = endp->call(
                   child_rpc.name(), adhoc_metadata_ptr->uuid(),
                   adhoc_storage.type(), adhoc_storage.get_resources());
           call_rv.has_value()) {

            const network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", child_rpc,
                        resp.error_code(), resp.op_id());

            return resp.error_code();
        }

        LOGGER_ERROR("rpc call failed");
        return tl::make_unexpected(error_code::snafu);
    };

    const auto rv =
            m_adhoc_manager.find(adhoc_id)
                    .or_else([](auto&&) {
                        LOGGER_ERROR("adhoc storage instance not found");
                    })
                    .and_then(update_helper);

    const auto resp = generic_response(rpc.id(), rv.value());

    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc, rv.value());

    req.respond(resp);
}

void
rpc_server::remove_adhoc_storage(const network::request& req,
                                 std::uint64_t adhoc_id) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{adhoc_id: {}}}", rpc, adhoc_id);

    scord::error_code ec = m_adhoc_manager.remove(adhoc_id);

    if(!ec) {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error removing job: {}\"",
                     rpc.id(), adhoc_id);
    }

    const auto resp = generic_response{rpc.id(), ec};

    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc, ec);

    req.respond(resp);
}

void
rpc_server::deploy_adhoc_storage(const network::request& req,
                                 std::uint64_t adhoc_id) {

    using network::get_address;
    using network::response_with_value;
    using network::rpc_info;

    using response_type = response_with_value<std::filesystem::path>;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{adhoc_id: {}}}", rpc, adhoc_id);

    /**
     * @brief Helper lambda to contact the adhoc controller and prompt it to
     * deploy an adhoc storage instance
     * @param adhoc_storage The relevant `adhoc_storage` object with
     * information about the instance to deploy.
     * @return
     */
    const auto deploy_helper = [&](const auto& adhoc_metadata_ptr)
            -> tl::expected<std::filesystem::path, error_code> {
        assert(adhoc_metadata_ptr);
        const auto adhoc_storage = adhoc_metadata_ptr->adhoc_storage();
        const auto endp = lookup(adhoc_storage.context().controller_address());

        if(!endp) {
            LOGGER_ERROR("endpoint lookup failed");
            return tl::make_unexpected(error_code::snafu);
        }

        const auto child_rpc =
                rpc.add_child(adhoc_storage.context().controller_address());

        LOGGER_INFO("rpc {:<} body: {{uuid: {:?}, type: {}, resources: {}}}",
                    child_rpc, adhoc_metadata_ptr->uuid(), adhoc_storage.type(),
                    adhoc_storage.get_resources());

        if(const auto call_rv = endp->call(
                   rpc.name(), adhoc_metadata_ptr->uuid(), adhoc_storage.type(),
                   adhoc_storage.get_resources());
           call_rv.has_value()) {

            const response_type resp{call_rv.value()};

            LOGGER_EVAL(
                    resp.error_code(), INFO, ERROR,
                    "rpc {:>} body: {{retval: {}, adhoc_dir: {}}} [op_id: {}]",
                    child_rpc, resp.error_code(), resp.value_or({}),
                    resp.op_id());

            if(const auto ec = resp.error_code(); !ec) {
                return tl::make_unexpected(ec);
            }
            return resp.value();
        }

        LOGGER_ERROR("rpc call failed");
        return tl::make_unexpected(error_code::snafu);
    };

    const auto rv =
            m_adhoc_manager.find(adhoc_id)
                    .or_else([](auto&&) {
                        LOGGER_ERROR("adhoc storage instance not found");
                    })
                    .and_then(deploy_helper);

    const response_type resp{rpc.id(),
                             rv.has_value() ? error_code::success : rv.error(),
                             rv.value_or(std::filesystem::path{})};

    LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                "rpc {:<} body: {{retval: {}, adhoc_dir: {}}}", rpc,
                resp.error_code(), resp.value());

    if(m_redis and resp.error_code() == error_code::success) {
        const auto job_id = m_adhoc_manager.find(adhoc_id)
                                    .value()
                                    .get()
                                    ->client_info()
                                    .get()
                                    ->job()
                                    .id();

        const auto timestamp =
                std::chrono::system_clock::now().time_since_epoch().count();

        std::unordered_map<std::string, std::string> m = {
                {"Deployed", "Yes"},
                {"StartTime", std::to_string(timestamp)},
                {"EndTime", "Running"}};

        m_redis.value().hmset(std::to_string(job_id), m.begin(), m.end());
    }
    req.respond(resp);
}

void
rpc_server::terminate_adhoc_storage(const network::request& req,
                                    std::uint64_t adhoc_id) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    using response_type = generic_response;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{adhoc_id: {}}}", rpc, adhoc_id);

    /**
     * @brief Helper lambda to contact the adhoc controller and prompt it to
     * terminate an adhoc storage instance
     * @param adhoc_storage The relevant `adhoc_storage` object with
     * information about the instance to terminate.
     * @return
     */
    const auto terminate_helper =
            [&](const auto& adhoc_metadata_ptr) -> error_code {
        assert(adhoc_metadata_ptr);
        const auto adhoc_storage = adhoc_metadata_ptr->adhoc_storage();
        const auto endp = lookup(adhoc_storage.context().controller_address());

        if(!endp) {
            LOGGER_ERROR("endpoint lookup failed");
            return error_code::snafu;
        }

        const auto child_rpc =
                rpc.add_child(adhoc_storage.context().controller_address());

        LOGGER_INFO("rpc {:<} body: {{uuid: {:?}, type: {}}}", child_rpc,
                    adhoc_metadata_ptr->uuid(), adhoc_storage.type());

        if(const auto call_rv =
                   endp->call(rpc.name(), adhoc_metadata_ptr->uuid(),
                              adhoc_storage.type());
           call_rv.has_value()) {

            const response_type resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", child_rpc,
                        resp.error_code(), resp.op_id());

            return resp.error_code();
        }

        LOGGER_ERROR("rpc call failed");
        return error_code::snafu;
    };

    const error_code ec =
            m_adhoc_manager.find(adhoc_id)
                    .or_else([](auto&&) {
                        LOGGER_ERROR("adhoc storage instance not found");
                    })
                    .transform(terminate_helper)
                    .value();

    const auto resp = response_type{rpc.id(), ec};
    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc,
                scord::error_code::success);


    if(m_redis and resp.error_code() == error_code::success) {
        const auto job_id = m_adhoc_manager.find(adhoc_id)
                                    .value()
                                    .get()
                                    ->client_info()
                                    .get()
                                    ->job()
                                    .id();

        const auto timestamp =
                std::chrono::system_clock::now().time_since_epoch().count();

        std::unordered_map<std::string, std::string> m = {
                {"Deployed", "Yes"},
                {"EndTime", std::to_string(timestamp)}};
        m_redis.value().hmset(std::to_string(job_id), m.begin(), m.end());
    }

    req.respond(resp);
}

void
rpc_server::register_pfs_storage(const network::request& req,
                                 const std::string& name,
                                 enum scord::pfs_storage::type type,
                                 const scord::pfs_storage::ctx& ctx) {

    using network::get_address;
    using network::response_with_id;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{name: {:?}, type: {}, pfs_ctx: {}}}", rpc,
                name, type, ctx);

    scord::error_code ec;
    std::optional<std::uint64_t> pfs_id = 0;

    if(const auto pm_result = m_pfs_manager.create(type, name, ctx);
       pm_result.has_value()) {
        const auto& adhoc_metadata_ptr = pm_result.value();
        pfs_id = adhoc_metadata_ptr->pfs_storage().id();
    } else {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error creating pfs_storage: {}\"",
                     rpc.id(), pm_result.error());
        ec = pm_result.error();
    }

    const auto resp = response_with_id{rpc.id(), ec, pfs_id};

    LOGGER_INFO("rpc {:<} body: {{retval: {}, pfs_id: {}}}", rpc, ec, pfs_id);

    req.respond(resp);
}

void
rpc_server::update_pfs_storage(const network::request& req,
                               std::uint64_t pfs_id,
                               const scord::pfs_storage::ctx& new_ctx) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{pfs_id: {}, new_ctx: {}}}", rpc, pfs_id,
                new_ctx);

    const auto ec = m_pfs_manager.update(pfs_id, new_ctx);

    if(!ec) {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error updating pfs_storage: {}\"",
                     rpc.id(), ec);
    }

    const auto resp = generic_response{rpc.id(), ec};

    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc, ec);

    req.respond(resp);
}

void
rpc_server::remove_pfs_storage(const network::request& req,
                               std::uint64_t pfs_id) {

    using network::generic_response;
    using network::get_address;
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{pfs_id: {}}}", rpc, pfs_id);

    scord::error_code ec = m_pfs_manager.remove(pfs_id);

    if(!ec) {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error removing pfs storage: {}\"",
                     rpc.id(), pfs_id);
    }

    const auto resp = generic_response{rpc.id(), ec};

    LOGGER_INFO("rpc {:<} body: {{retval: {}}}", rpc, ec);

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
    using network::rpc_info;

    const auto rpc = rpc_info::create(RPC_NAME(), get_address(req));

    LOGGER_INFO("rpc {:>} body: {{job_id: {}, sources: {}, targets: {}, "
                "limits: {}, mapping: {}}}",
                rpc, job_id, sources, targets, limits, mapping);

    const auto jm_result = m_job_manager.find(job_id);

    if(!jm_result) {
        LOGGER_ERROR("rpc id: {} error_msg: \"Error finding job: {}\"",
                     rpc.id(), job_id);
        const auto resp = response_with_id{rpc.id(), jm_result.error()};
        LOGGER_ERROR("rpc {:<} body: {{retval: {}}}", rpc, resp.error_code());
        req.respond(resp);
        return;
    }

    const auto& job_metadata_ptr = jm_result.value();

    if(!job_metadata_ptr->adhoc_storage_metadata()) {
        LOGGER_ERROR("rpc id: {} error_msg: \"Job has no adhoc storage\"",
                     rpc.id(), job_id);
        const auto resp = response_with_id{rpc.id(), error_code::no_resources};
        LOGGER_ERROR("rpc {:<} body: {{retval: {}}}", rpc, resp.error_code());
        req.respond(resp);
        return;
    }

    const auto data_stager_address =
            job_metadata_ptr->adhoc_storage_metadata()->data_stager_address();

    // Transform the `scord::dataset`s into `cargo::dataset`s and contact the
    // Cargo service associated with the job's adhoc storage instance to
    // execute the transfers.
    cargo::server srv{data_stager_address};

    std::vector<cargo::dataset> inputs;
    std::vector<cargo::dataset> outputs;

    // TODO: check type of storage tier to enable parallel transfers
    std::transform(sources.cbegin(), sources.cend(), std::back_inserter(inputs),
                   [](const auto& src) { return ::dataset_process(src.id()); });

    std::transform(targets.cbegin(), targets.cend(),
                   std::back_inserter(outputs),
                   [](const auto& tgt) { return ::dataset_process(tgt.id()); });

    const auto cargo_tx = cargo::transfer_datasets(srv, inputs, outputs);

    // Register the transfer into the `tranfer_manager`.
    // We embed the generated `cargo::transfer` object into
    // scord's `transfer_metadata` so that we can later query the Cargo
    // service for the transfer's status.
    const auto rv =
            m_transfer_manager.create(cargo_tx, limits)
                    .or_else([&](auto&& ec) {
                        LOGGER_ERROR("rpc id: {} error_msg: \"Error creating "
                                     "transfer: {}\"",
                                     rpc.id(), ec);
                    })
                    .and_then([&](auto&& transfer_metadata_ptr)
                                      -> tl::expected<transfer_id, error_code> {
                        return transfer_metadata_ptr->id();
                    });

    const auto resp =
            rv ? response_with_id{rpc.id(), error_code::success, rv.value()}
               : response_with_id{rpc.id(), rv.error()};

    LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                "rpc {:<} body: {{retval: {}, tx_id: {}}}", rpc,
                resp.error_code(), resp.value_or_none());
    req.respond(resp);
}


/* Scheduling is done each 0.5 s*/
void
rpc_server::scheduler_update() {
    std::vector<std::pair<std::string, int>> return_set;
    const auto threshold = 0.1f;
    while(!m_shutting_down) {
        thallium::thread::self().sleep(m_network_engine, 500);
        m_transfer_manager.lock();
        const auto transfer = m_transfer_manager.transfer();
        std::vector<scord::transfer_id> v_ids;
        for(const auto& tr_unit : transfer) {
            const auto tr_info = tr_unit.second.get();

            // Contact for transfer status
            const auto status = tr_info->transfer().status();

            switch(status.state()) {
                case cargo::transfer_state::completed:
                    LOGGER_INFO("Completed");
                    v_ids.push_back(tr_unit.first);
                    continue;
                    break;
                case cargo::transfer_state::failed:
                    LOGGER_INFO("Failed");
                    v_ids.push_back(tr_unit.first);
                    continue;
                    break;
                case cargo::transfer_state::pending:
                    continue;
                    break;
                case cargo::transfer_state::running:
                    break;
            }

            tr_info->update(status.bw());
            auto bw = tr_info->measured_bandwidth();
            uint64_t qos = 0;
            try {
                qos = tr_info->qos().front().value();
            } catch(const std::exception& e) {
                continue;
            }

            if(bw == -1) {
                continue;
            }

            if(bw + bw * threshold > qos) {
                // Send decrease / slow signal to cargo
                tr_info->transfer().bw_control(+1);
            } else if(bw - bw * threshold < qos) {
                // Send increase / speed up signal to cargo
                tr_info->transfer().bw_control(-1);
            }
        }
        m_transfer_manager.unlock();

        // Remove all failed/done transfers
        for(const auto id : v_ids) {
            m_transfer_manager.remove(id);
        }
    }
}


} // namespace scord
