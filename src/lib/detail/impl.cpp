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
#include <net/client.hpp>
#include <net/endpoint.hpp>
#include <net/request.hpp>
#include <net/serialization.hpp>
#include <scord/types.hpp>
#include "impl.hpp"

using namespace std::literals;

namespace api {

struct remote_procedure {
    static std::uint64_t
    new_id() {
        static std::atomic_uint64_t current_id;
        return current_id++;
    }
};

} // namespace api

namespace scord::detail {

scord::error_code
ping(const server& srv) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()));

        if(const auto call_rv = endp.call("ADM_"s + __FUNCTION__);
           call_rv.has_value()) {

            const scord::network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

tl::expected<scord::job, scord::error_code>
register_job(const server& srv, const job::resources& job_resources,
             const job::requirements& job_requirements,
             scord::slurm_job_id slurm_id) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO(
                "rpc id: {} name: {} from: {} => "
                "body: {{job_resources: {}, job_requirements: {}, slurm_id: "
                "{}}}",
                rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                std::quoted(rpc_client.self_address()), job_resources,
                job_requirements, slurm_id);

        if(const auto call_rv = endp.call("ADM_"s + __FUNCTION__, job_resources,
                                          job_requirements, slurm_id);
           call_rv.has_value()) {

            const scord::network::response_with_id resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}, job_id: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.value(), resp.op_id());

            if(const auto ec = resp.error_code(); !ec) {
                return tl::make_unexpected(resp.error_code());
            }

            return scord::job{resp.value(), slurm_id};
        }
    }

    LOGGER_ERROR("rpc call failed");
    return tl::make_unexpected(scord::error_code::other);
}

scord::error_code
update_job(const server& srv, const job& job,
           const job::resources& new_resources) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{job_id: {}, new_resources: {}}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()), job.id(),
                    new_resources);

        if(const auto& call_rv =
                   endp.call("ADM_"s + __FUNCTION__, job.id(), new_resources);
           call_rv.has_value()) {

            const scord::network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.op_id());
            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

scord::error_code
remove_job(const server& srv, const job& job) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{job_id: {}}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()), job.id());

        if(const auto& call_rv = endp.call("ADM_"s + __FUNCTION__, job.id());
           call_rv.has_value()) {

            const scord::network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.op_id());
            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

tl::expected<scord::adhoc_storage, scord::error_code>
register_adhoc_storage(const server& srv, const std::string& name,
                       enum adhoc_storage::type type,
                       const adhoc_storage::ctx& ctx,
                       const adhoc_storage::resources& resources) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{name: {}, type: {}, adhoc_ctx: {}, "
                    "adhoc_resources: {}}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()), name, type, ctx,
                    resources);

        if(const auto& call_rv = endp.call("ADM_"s + __FUNCTION__, name, type,
                                           ctx, resources);
           call_rv.has_value()) {

            const scord::network::response_with_id resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}, adhoc_id: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.value(), resp.op_id());

            if(const auto ec = resp.error_code(); !ec) {
                return tl::make_unexpected(ec);
            }

            return scord::adhoc_storage{type, name, resp.value(), ctx,
                                        resources};
        }
    }

    LOGGER_ERROR("rpc call failed");
    return tl::make_unexpected(scord::error_code::other);
}

scord::error_code
update_adhoc_storage(const server& srv, const adhoc_storage::ctx& new_ctx,
                     const adhoc_storage& adhoc_storage) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{adhoc_id: {}, new_ctx: {}}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()), adhoc_storage.id(),
                    new_ctx);

        if(const auto& call_rv = endp.call("ADM_"s + __FUNCTION__,
                                           adhoc_storage.id(), new_ctx);
           call_rv.has_value()) {

            const scord::network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

scord::error_code
remove_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{adhoc_id: {}}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()), adhoc_storage.id());

        if(const auto& call_rv =
                   endp.call("ADM_"s + __FUNCTION__, adhoc_storage.id());
           call_rv.has_value()) {

            const scord::network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

tl::expected<scord::pfs_storage, scord::error_code>
register_pfs_storage(const server& srv, const std::string& name,
                     enum pfs_storage::type type, const pfs_storage::ctx& ctx) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{name: {}, type: {}, pfs_ctx: {}}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()), name, type, ctx);

        if(const auto& call_rv =
                   endp.call("ADM_"s + __FUNCTION__, name, type, ctx);
           call_rv.has_value()) {

            const scord::network::response_with_id resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}, pfs_id: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.value(), resp.op_id());

            if(const auto ec = resp.error_code(); !ec) {
                return tl::make_unexpected(ec);
            }

            return scord::pfs_storage{type, name, resp.value(), ctx};
        }
    }

    LOGGER_ERROR("rpc call failed");
    return tl::make_unexpected(scord::error_code::other);
}

scord::error_code
update_pfs_storage(const server& srv, const pfs_storage& pfs_storage,
                   const scord::pfs_storage::ctx& new_ctx) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{pfs_id: {}, new_ctx: {}}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()), pfs_storage.id(),
                    new_ctx);

        if(const auto& call_rv =
                   endp.call("ADM_"s + __FUNCTION__, pfs_storage.id(), new_ctx);
           call_rv.has_value()) {

            const scord::network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

scord::error_code
remove_pfs_storage(const server& srv, const pfs_storage& pfs_storage) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{pfs_id: {}}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()), pfs_storage.id());

        if(const auto& call_rv =
                   endp.call("ADM_"s + __FUNCTION__, pfs_storage.id());
           call_rv.has_value()) {

            const scord::network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

scord::error_code
deploy_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{adhoc_id: {}}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()), adhoc_storage.id());

        if(const auto& call_rv =
                   endp.call("ADM_"s + __FUNCTION__, adhoc_storage.id());
           call_rv.has_value()) {

            const scord::network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

tl::expected<transfer, error_code>
transfer_datasets(const server& srv, const job& job,
                  const std::vector<dataset>& sources,
                  const std::vector<dataset>& targets,
                  const std::vector<qos::limit>& limits,
                  transfer::mapping mapping) {

    scord::network::client rpc_client{srv.protocol()};

    const auto rpc_id = ::api::remote_procedure::new_id();

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc id: {} name: {} from: {} => "
                    "body: {{job_id: {}, sources: {}, targets: {}, limits: {}, "
                    "mapping: {}}}",
                    rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                    std::quoted(rpc_client.self_address()), job.id(), sources,
                    targets, limits, mapping);

        if(const auto& call_rv = endp.call("ADM_"s + __FUNCTION__, job.id(),
                                           sources, targets, limits, mapping);
           call_rv.has_value()) {

            const scord::network::response_with_id resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc id: {} name: {} from: {} <= "
                        "body: {{retval: {}, tx_id: {}}} [op_id: {}]",
                        rpc_id, std::quoted("ADM_"s + __FUNCTION__),
                        std::quoted(endp.address()), resp.error_code(),
                        resp.value(), resp.op_id());

            if(const auto ec = resp.error_code(); !ec) {
                return tl::make_unexpected(ec);
            }

            return scord::transfer{resp.value()};
        }
    }

    LOGGER_ERROR("rpc call failed");
    return tl::make_unexpected(scord::error_code::other);
}

} // namespace scord::detail
