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

#ifndef SCORD_NETWORK_ENGINE_HPP
#define SCORD_NETWORK_ENGINE_HPP

#include <memory>
#include <exception>
#include <margo.h>
#include <logger.hpp>
#include <utility>
#include <detail/address.hpp>
#include "rpcs.hpp"


namespace scord::network {

namespace detail {

#define REGISTER_RPC(__mid, __m_rpc_names, __func_name, __in_t, __out_t,       \
                     __handler, requires_response)                             \
    {                                                                          \
        hg_id_t id = margo_provider_register_name(                             \
                __mid, __func_name, BOOST_PP_CAT(hg_proc_, __in_t),            \
                BOOST_PP_CAT(hg_proc_, __out_t), _handler_for_##__handler,     \
                MARGO_DEFAULT_PROVIDER_ID, ABT_POOL_NULL);                     \
        __m_rpc_names.emplace(__func_name, id);                                \
        if(!requires_response) {                                               \
            ::margo_registered_disable_response(__mid, id, HG_TRUE);           \
        }                                                                      \
    }


struct margo_context {

    explicit margo_context(::margo_instance_id mid) : m_mid(mid) {}

    void
    register_rpc(const std::string& name, bool requires_response) {
        auto id = MARGO_REGISTER(m_mid, name.c_str(), void, void, ping);
        m_rpc_names.emplace(name, id);

        if(!requires_response) {
            ::margo_registered_disable_response(m_mid, id, HG_TRUE);
        }
    }

    margo_instance_id m_mid;
    std::unordered_map<std::string, hg_id_t> m_rpc_names;
};

} // namespace detail

// forward declarations
struct endpoint;

struct engine {

    enum class execution_mode : bool {
        server = MARGO_SERVER_MODE,
        client = MARGO_CLIENT_MODE
    };

    explicit engine(std::string_view address,
                    execution_mode = execution_mode::client) {
        struct margo_init_info info = MARGO_INIT_INFO_INITIALIZER;

        m_context = std::make_shared<detail::margo_context>(
                margo_init_ext(address.data(), MARGO_SERVER_MODE, &info));

        if(m_context->m_mid == MARGO_INSTANCE_NULL) {
            throw std::runtime_error("Margo initialization failed");
        }
    }

    ~engine() {
        if(m_context) {
            ::margo_finalize(m_context->m_mid);
        }
    }

    void
    register_rpcs() {

        // register RPCs manually for now
        REGISTER_RPC(m_context->m_mid, m_context->m_rpc_names, "ping", void,
                     void, ping, false);

        REGISTER_RPC(m_context->m_mid, m_context->m_rpc_names, "ADM_input",

                     ADM_input_in_t, ADM_input_out_t, ADM_input, true);

        REGISTER_RPC(m_context->m_mid, m_context->m_rpc_names, "ADM_output",
                     ADM_output_in_t, ADM_output_out_t, ADM_output, true);

        REGISTER_RPC(m_context->m_mid, m_context->m_rpc_names, "ADM_inout",
                     ADM_inout_in_t, ADM_inout_out_t, ADM_inout, true);

        REGISTER_RPC(m_context->m_mid, m_context->m_rpc_names,
                     "ADM_adhoc_context", ADM_adhoc_context_in_t,
                     ADM_adhoc_context_out_t, ADM_adhoc_context, true);

        REGISTER_RPC(m_context->m_mid, m_context->m_rpc_names,
                     "ADM_adhoc_context_id", ADM_adhoc_context_id_in_t,
                     ADM_adhoc_context_id_out_t, ADM_adhoc_context_id, true);

        REGISTER_RPC(m_context->m_mid, m_context->m_rpc_names,
                     "ADM_adhoc_nodes", ADM_adhoc_nodes_in_t,
                     ADM_adhoc_nodes_out_t, ADM_adhoc_nodes, true);
    }

    void
    listen() const {

        /* NOTE: there isn't anything else for the server to do at this point
         * except wait for itself to be shut down.  The
         * margo_wait_for_finalize() call here yields to let Margo drive
         * progress until that happens.
         */
        ::margo_wait_for_finalize(m_context->m_mid);
    }

    void
    stop() {
        ::margo_finalize(m_context->m_mid);

        // It is not safe to access m_margo_context->m_mid after the
        // margo_finalize() call. Make sure that no other threads can do a
        // double margo_finalize() (e.g when calling ~engine()) by resetting
        // m_margo_context.
        m_context.reset();
    }

    endpoint
    lookup(const std::string& address) const;

    std::shared_ptr<detail::margo_context> m_context;
};

struct endpoint {
private:
    // Endpoints should only be created by calling engine::lookup()
    friend class engine;

    endpoint(std::shared_ptr<detail::margo_context> context,
             std::shared_ptr<detail::address> address)
        : m_margo_context(std::move(context)), m_address(std::move(address)) {}

public:
    endpoint(const endpoint& /*other*/) = default;
    endpoint&
    operator=(const endpoint& /*other*/) = default;
    endpoint(endpoint&& /*rhs*/) = default;
    endpoint&
    operator=(endpoint&& /*rhs*/) = default;

    template <typename... Args>
    void
    call(const std::string& id, Args&&... args) {

        const auto it = m_margo_context->m_rpc_names.find(id);

        if(it == m_margo_context->m_rpc_names.end()) {
            throw std::runtime_error(
                    fmt::format("Unknown remote procedure: {}", id));
        }

        hg_handle_t handle;
        auto ret = ::margo_create(m_margo_context->m_mid,
                                  m_address->mercury_address(), it->second,
                                  &handle);
        if(ret != HG_SUCCESS) {
            throw std::runtime_error(
                    fmt::format("Error during endpoint::call(): {}",
                                ::HG_Error_to_string(ret)));
        }

        ret = ::margo_forward(handle, nullptr);

        if(ret != HG_SUCCESS) {
            throw std::runtime_error(
                    fmt::format("Error during endpoint::call(): {}",
                                ::HG_Error_to_string(ret)));
        }

        ret = ::margo_destroy(handle);

        if(ret != HG_SUCCESS) {
            throw std::runtime_error(
                    fmt::format("Error during endpoint::call(): {}",
                                ::HG_Error_to_string(ret)));
        }
    }

    /**
     * Deprecated call, used to support Margo directly
     *
     **/
    template <typename T1 = void*, typename T2 = void*>
    [[deprecated("It should be eventually replaced by a generic call")]] void
    call(const std::string& id, T1 input = nullptr, T2 output = nullptr) {

        const auto it = m_margo_context->m_rpc_names.find(id);

        if(it == m_margo_context->m_rpc_names.end()) {
            throw std::runtime_error(
                    fmt::format("Unknown remote procedure: {}", id));
        }

        hg_handle_t handle;
        auto ret = ::margo_create(m_margo_context->m_mid,
                                  m_address->mercury_address(), it->second,
                                  &handle);
        if(ret != HG_SUCCESS) {
            throw std::runtime_error(
                    fmt::format("Error during endpoint::call(): {}",
                                ::HG_Error_to_string(ret)));
        }

        ret = ::margo_forward(handle, input);

        if(ret != HG_SUCCESS) {
            throw std::runtime_error(
                    fmt::format("Error during endpoint::call(): {}",
                                ::HG_Error_to_string(ret)));
        }

        if(output != nullptr) {
            ret = ::margo_get_output(handle, output);
        }


        ret = ::margo_destroy(handle);

        if(ret != HG_SUCCESS) {
            throw std::runtime_error(
                    fmt::format("Error during endpoint::call(): {}",
                                ::HG_Error_to_string(ret)));
        }
    }

private:
    std::shared_ptr<detail::margo_context> m_margo_context;
    std::shared_ptr<detail::address> m_address;
};

// now that we have the complete definition of engine and endpoint, we can
// finally define engine::lookup completely
inline endpoint
engine::lookup(const std::string& address) const {

    hg_addr_t svr_addr;
    auto ret =
            ::margo_addr_lookup(m_context->m_mid, address.c_str(), &svr_addr);
    if(ret != HG_SUCCESS) {
        throw std::runtime_error(
                fmt::format("Error during engine::lookup(): {}",
                            ::HG_Error_to_string(ret)));
    }

    return {m_context, std::make_shared<detail::address>(
                               ::margo_get_class(m_context->m_mid), svr_addr)};
}


struct rpc_client : engine {
    explicit rpc_client(const std::string& protocol)
        : engine(protocol, execution_mode::client) {}
};

struct rpc_acceptor : engine {

    static std::string
    format_address(const std::string& protocol, const std::string& address,
                   int port) {
        return fmt::format("{}://{}:{}", protocol, address, port);
    }

    rpc_acceptor(const std::string& protocol, const std::string& bind_address,
                 int port)
        : engine(format_address(protocol, bind_address, port)) {}
};


} // namespace scord::network

#endif // SCORD_NETWORK_ENGINE_HPP
