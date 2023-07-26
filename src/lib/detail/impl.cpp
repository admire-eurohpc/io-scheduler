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
#include <net/utilities.hpp>
#include <scord/types.hpp>
#include "impl.hpp"

using namespace std::literals;

constexpr auto default_ping_timeout = 4s;

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

#define RPC_NAME() ("ADM_"s + __FUNCTION__)

scord::error_code
ping(const server& srv) {

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{}}", rpc);

        if(const auto call_rv =
                   endp.timed_call(rpc.name(), default_ping_timeout);
           call_rv.has_value()) {

            const network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", rpc,
                        resp.error_code(), resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

tl::expected<scord::job_info, scord::error_code>
query(const server& srv, slurm_job_id job_id) {

    using response_type = network::response_with_value<scord::job_info>;

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{slurm_job_id: {}}}", rpc, job_id);

        if(const auto& call_rv = endp.call(rpc.name(), job_id);
           call_rv.has_value()) {

            const response_type resp{call_rv.value()};

            LOGGER_EVAL(
                    resp.error_code(), INFO, ERROR,
                    "rpc {:>} body: {{retval: {}, job_info: {}}} [op_id: {}]",
                    rpc, resp.error_code(), resp.value(), resp.op_id());

            if(!resp.error_code()) {
                return tl::make_unexpected(resp.error_code());
            }

            return resp.value();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return tl::make_unexpected(scord::error_code::other);
}

tl::expected<scord::job, scord::error_code>
register_job(const server& srv, const job::resources& job_resources,
             const job::requirements& job_requirements,
             scord::slurm_job_id slurm_id) {

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{job_resources: {}, job_requirements: {}, "
                    "slurm_id: {}}}",
                    rpc, job_resources, job_requirements, slurm_id);

        if(const auto call_rv = endp.call(rpc.name(), job_resources,
                                          job_requirements, slurm_id);
           call_rv.has_value()) {

            const network::response_with_id resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}, job_id: {}}} [op_id: {}]",
                        rpc, resp.error_code(), resp.value(), resp.op_id());

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

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{job_id: {}, new_resources: {}}}", rpc,
                    job.id(), new_resources);

        if(const auto& call_rv = endp.call(rpc.name(), job.id(), new_resources);
           call_rv.has_value()) {

            const network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", rpc,
                        resp.error_code(), resp.op_id());
            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

scord::error_code
remove_job(const server& srv, const job& job) {

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{job_id: {}}}", rpc, job.id());

        if(const auto& call_rv = endp.call(rpc.name(), job.id());
           call_rv.has_value()) {

            const network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", rpc,
                        resp.error_code(), resp.op_id());
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

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{name: {}, type: {}, adhoc_ctx: {}, "
                    "adhoc_resources: {}}}",
                    rpc, std::quoted(name), type, ctx, resources);

        if(const auto& call_rv =
                   endp.call(rpc.name(), name, type, ctx, resources);
           call_rv.has_value()) {

            const network::response_with_id resp{call_rv.value()};

            LOGGER_EVAL(
                    resp.error_code(), INFO, ERROR,
                    "rpc {:>} body: {{retval: {}, adhoc_id: {}}} [op_id: {}]",
                    rpc, resp.error_code(), resp.value(), resp.op_id());

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
update_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage,
                     const adhoc_storage::resources& new_resources) {

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{adhoc_id: {}, new_resources: {}}}", rpc,
                    adhoc_storage.id(), new_resources);

        if(const auto& call_rv =
                   endp.call(rpc.name(), adhoc_storage.id(), new_resources);
           call_rv.has_value()) {

            const network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", rpc,
                        resp.error_code(), resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

scord::error_code
remove_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage) {

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{adhoc_id: {}}}", rpc, adhoc_storage.id());

        if(const auto& call_rv = endp.call(rpc.name(), adhoc_storage.id());
           call_rv.has_value()) {

            const network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", rpc,
                        resp.error_code(), resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

tl::expected<scord::pfs_storage, scord::error_code>
register_pfs_storage(const server& srv, const std::string& name,
                     enum pfs_storage::type type, const pfs_storage::ctx& ctx) {

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{name: {}, type: {}, pfs_ctx: {}}}", rpc,
                    std::quoted(name), type, ctx);

        if(const auto& call_rv = endp.call(rpc.name(), name, type, ctx);
           call_rv.has_value()) {

            const network::response_with_id resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}, pfs_id: {}}} [op_id: {}]",
                        rpc, resp.error_code(), resp.value(), resp.op_id());

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

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{pfs_id: {}, new_ctx: {}}}", rpc,
                    pfs_storage.id(), new_ctx);

        if(const auto& call_rv =
                   endp.call(rpc.name(), pfs_storage.id(), new_ctx);
           call_rv.has_value()) {

            const network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", rpc,
                        resp.error_code(), resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

scord::error_code
remove_pfs_storage(const server& srv, const pfs_storage& pfs_storage) {

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{pfs_id: {}}}", rpc, pfs_storage.id());

        if(const auto& call_rv = endp.call(rpc.name(), pfs_storage.id());
           call_rv.has_value()) {

            const network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", rpc,
                        resp.error_code(), resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

tl::expected<std::filesystem::path, scord::error_code>
deploy_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage) {

    using response_type = network::response_with_value<std::filesystem::path>;

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{adhoc_id: {}}}", rpc, adhoc_storage.id());

        if(const auto& call_rv = endp.call(rpc.name(), adhoc_storage.id());
           call_rv.has_value()) {

            const response_type resp{call_rv.value()};

            LOGGER_EVAL(
                    resp.error_code(), INFO, ERROR,
                    "rpc {:>} body: {{retval: {}, adhoc_dir: {}}} [op_id: {}]",
                    rpc, resp.error_code(), resp.value(), resp.op_id());

            if(!resp.error_code()) {
                return tl::make_unexpected(resp.error_code());
            }

            return resp.value();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return tl::make_unexpected(scord::error_code::other);
}

scord::error_code
terminate_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage) {

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{adhoc_id: {}}}", rpc, adhoc_storage.id());

        if(const auto call_rv = endp.call(rpc.name(), adhoc_storage.id());
           call_rv.has_value()) {

            const network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", rpc,
                        resp.error_code(), resp.op_id());

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

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{job_id: {}, sources: {}, targets: {}, "
                    "limits: {}, mapping: {}}}",
                    rpc, job.id(), sources, targets, limits, mapping);

        if(const auto& call_rv = endp.call(rpc.name(), job.id(), sources,
                                           targets, limits, mapping);
           call_rv.has_value()) {

            const network::response_with_id resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}, tx_id: {}}} [op_id: {}]",
                        rpc, resp.error_code(), resp.value(), resp.op_id());

            if(const auto ec = resp.error_code(); !ec) {
                return tl::make_unexpected(ec);
            }

            return scord::transfer{resp.value()};
        }
    }

    LOGGER_ERROR("rpc call failed");
    return tl::make_unexpected(scord::error_code::other);
}


scord::error_code
transfer_update(const server& srv, uint64_t transfer_id, float obtained_bw) {

    network::client rpc_client{srv.protocol()};

    const auto rpc = network::rpc_info::create(RPC_NAME(), srv.address());

    if(const auto& lookup_rv = rpc_client.lookup(srv.address());
       lookup_rv.has_value()) {
        const auto& endp = lookup_rv.value();

        LOGGER_INFO("rpc {:<} body: {{transfer_id: {}}}", rpc, transfer_id);

        if(const auto& call_rv =
                   endp.call(rpc.name(), transfer_id, obtained_bw);
           call_rv.has_value()) {

            const network::generic_response resp{call_rv.value()};

            LOGGER_EVAL(resp.error_code(), INFO, ERROR,
                        "rpc {:>} body: {{retval: {}}} [op_id: {}]", rpc,
                        resp.error_code(), resp.op_id());

            return resp.error_code();
        }
    }

    LOGGER_ERROR("rpc call failed");
    return scord::error_code::other;
}

} // namespace scord::detail
