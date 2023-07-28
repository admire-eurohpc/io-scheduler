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

#include <logger/logger.hpp>
#include <net/serialization.hpp>
#include <utility>
#include <variant>
#include <optional>
#include "scord/types.hpp"
#include "scord/types.h"
#include "types_private.h"

/******************************************************************************/
/* C++ Type definitions and related functions                                 */
/******************************************************************************/

extern "C" {
const char*
ADM_strerror(ADM_return_t errnum);
};

namespace scord {

std::string_view
error_code::message() const {
    return ::ADM_strerror(m_value);
}

class server::impl {

public:
    impl(std::string protocol, std::string address)
        : m_protocol(std::move(protocol)), m_address(std::move(address)) {}

    std::string
    protocol() const {
        return m_protocol;
    }

    std::string
    address() const {
        return m_address;
    }

private:
    std::string m_protocol;
    std::string m_address;
};

server::server(std::string protocol, std::string address)
    : m_pimpl(std::make_unique<server::impl>(std::move(protocol),
                                             std::move(address))) {}

server::server(const ADM_server_t& srv)
    : server::server(srv->s_protocol, srv->s_address) {}

server::server(server&&) noexcept = default;

server&
server::operator=(server&&) noexcept = default;

server::~server() = default;

std::string
server::protocol() const {
    return m_pimpl->protocol();
}

std::string
server::address() const {
    return m_pimpl->address();
}

class node::impl {

public:
    impl() = default;
    explicit impl(std::string hostname, node::type node_type)
        : m_hostname(std::move(hostname)), m_type(node_type) {}

    std::string
    hostname() const {
        return m_hostname;
    }

    node::type
    get_type() const {
        return m_type;
    }

    template <class Archive>
    void
    load(Archive& ar) {
        ar(SCORD_SERIALIZATION_NVP(m_hostname));
        ar(SCORD_SERIALIZATION_NVP(m_type));
    }

    template <class Archive>
    void
    save(Archive& ar) const {
        ar(SCORD_SERIALIZATION_NVP(m_hostname));
        ar(SCORD_SERIALIZATION_NVP(m_type));
    }

private:
    std::string m_hostname;
    node::type m_type;
};

node::node() = default;

node::node(std::string hostname, node::type type)
    : m_pimpl(std::make_unique<node::impl>(std::move(hostname), type)) {}

node::node(const ADM_node_t& node)
    : node::node(node->n_hostname, static_cast<node::type>(node->n_type)) {}

node::node(const node& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

node::node(node&&) noexcept = default;

node&
node::operator=(const node& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

node&
node::operator=(node&&) noexcept = default;

node::~node() = default;

std::string
node::hostname() const {
    return m_pimpl->hostname();
}

node::type
node::get_type() const {
    return m_pimpl->get_type();
}

// since the PIMPL class is fully defined at this point, we can now
// define the serialization function
template <class Archive>
inline void
node::serialize(Archive& ar) {
    ar(SCORD_SERIALIZATION_NVP(m_pimpl));
}

//  we must also explicitly instantiate our template functions for
//  serialization in the desired archives
template void
node::impl::save<network::serialization::output_archive>(
        network::serialization::output_archive&) const;

template void
node::impl::load<network::serialization::input_archive>(
        network::serialization::input_archive&);

template void
node::serialize<network::serialization::output_archive>(
        network::serialization::output_archive&);

template void
node::serialize<network::serialization::input_archive>(
        network::serialization::input_archive&);

class job::impl {

public:
    impl() {}
    impl(job_id id, slurm_job_id slurm_job_id)
        : m_id(id), m_slurm_job_id(slurm_job_id) {}
    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;

    job_id
    id() const {
        return m_id;
    }

    slurm_job_id
    slurm_id() const {
        return m_slurm_job_id;
    }

private:
    friend class cereal::access;

    template <class Archive>
    void
    load(Archive& ar) {
        ar(CEREAL_NVP(m_id));
    }

    template <class Archive>
    void
    save(Archive& ar) const {
        ar(CEREAL_NVP(m_id));
    }

    job_id m_id;
    slurm_job_id m_slurm_job_id;
};

job::requirements::requirements() = default;

job::requirements::requirements(std::vector<scord::dataset> inputs,
                                std::vector<scord::dataset> outputs)
    : m_inputs(std::move(inputs)), m_outputs(std::move(outputs)) {}

job::requirements::requirements(std::vector<scord::dataset> inputs,
                                std::vector<scord::dataset> outputs,
                                scord::adhoc_storage adhoc_storage)
    : m_inputs(std::move(inputs)), m_outputs(std::move(outputs)),
      m_adhoc_storage(std::move(adhoc_storage)) {}

job::requirements::requirements(ADM_job_requirements_t reqs) {

    m_inputs.reserve(reqs->r_inputs->l_length);

    for(size_t i = 0; i < reqs->r_inputs->l_length; ++i) {
        m_inputs.emplace_back(reqs->r_inputs->l_datasets[i].d_id);
    }

    m_outputs.reserve(reqs->r_outputs->l_length);

    for(size_t i = 0; i < reqs->r_outputs->l_length; ++i) {
        m_outputs.emplace_back(reqs->r_outputs->l_datasets[i].d_id);
    }

    if(reqs->r_adhoc_storage) {
        m_adhoc_storage = scord::adhoc_storage(reqs->r_adhoc_storage);
    }
}

std::vector<scord::dataset>
job::requirements::inputs() const {
    return m_inputs;
}

std::vector<scord::dataset>
job::requirements::outputs() const {
    return m_outputs;
}

std::optional<scord::adhoc_storage>
job::requirements::adhoc_storage() const {
    return m_adhoc_storage;
}


job::resources::resources() = default;

job::resources::resources(std::vector<scord::node> nodes)
    : m_nodes(std::move(nodes)) {}

job::resources::resources(ADM_job_resources_t res) {
    assert(res->r_nodes);
    m_nodes.reserve(res->r_nodes->l_length);

    for(size_t i = 0; i < res->r_nodes->l_length; ++i) {
        m_nodes.emplace_back(res->r_nodes->l_nodes[i].n_hostname,
                             static_cast<scord::node::type>(
                                     res->r_nodes->l_nodes[i].n_type));
    }
}

std::vector<scord::node>
job::resources::nodes() const {
    return m_nodes;
}

job::job() = default;

job::job(job_id id, slurm_job_id slurm_job_id)
    : m_pimpl(std::make_unique<job::impl>(id, slurm_job_id)) {}

job::job(ADM_job_t job) : job::job(job->j_id, job->j_slurm_id) {}

job::operator ADM_job_t() const {
    return ADM_job_create(m_pimpl->id(), m_pimpl->slurm_id());
}

job::job(job&&) noexcept = default;

job&
job::operator=(job&&) noexcept = default;

job::job(const job& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

job&
job::operator=(const job& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

job::~job() = default;

job_id
job::id() const {
    return m_pimpl->id();
}

job_id
job::slurm_id() const {
    return m_pimpl->slurm_id();
}

template <class Archive>
inline void
job::serialize(Archive& ar) {
    ar(CEREAL_NVP(m_pimpl));
}

template void
job::serialize<thallium::proc_input_archive<>>(thallium::proc_input_archive<>&);
template void
job::serialize<thallium::proc_output_archive<>>(
        thallium::proc_output_archive<>&);


class transfer::impl {

public:
    impl() = default;
    explicit impl(transfer_id id) : m_id(id) {}

    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;

    transfer_id
    id() const {
        return m_id;
    }

    template <class Archive>
    void
    load(Archive& ar) {
        ar(SCORD_SERIALIZATION_NVP(m_id));
    }

    template <class Archive>
    void
    save(Archive& ar) const {
        ar(SCORD_SERIALIZATION_NVP(m_id));
    }

private:
    transfer_id m_id;
};

transfer::transfer() = default;

transfer::transfer(transfer_id id)
    : m_pimpl(std::make_unique<transfer::impl>(id)) {}

transfer::transfer(ADM_transfer_t transfer)
    : transfer::transfer(transfer->t_id) {}

transfer::operator ADM_transfer_t() const {
    return ADM_transfer_create(m_pimpl->id());
}

transfer::transfer(transfer&&) noexcept = default;

transfer&
transfer::operator=(transfer&&) noexcept = default;

transfer::transfer(const transfer& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

transfer&
transfer::operator=(const transfer& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

transfer::~transfer() = default;

transfer_id
transfer::id() const {
    return m_pimpl->id();
}

// since the PIMPL class is fully defined at this point, we can now
// define the serialization function
template <class Archive>
inline void
transfer::serialize(Archive& ar) {
    ar(SCORD_SERIALIZATION_NVP(m_pimpl));
}

//  we must also explicitly instantiate our template functions for
//  serialization in the desired archives
template void
transfer::impl::save<network::serialization::output_archive>(
        network::serialization::output_archive&) const;

template void
transfer::impl::load<network::serialization::input_archive>(
        network::serialization::input_archive&);

template void
transfer::serialize<network::serialization::output_archive>(
        network::serialization::output_archive&);

template void
transfer::serialize<network::serialization::input_archive>(
        network::serialization::input_archive&);

class dataset::impl {
public:
    impl() = default;
    explicit impl(std::string id) : m_id(std::move(id)) {}
    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;
    ~impl() = default;

    std::string
    id() const {
        return m_id;
    }

    template <class Archive>
    void
    load(Archive& ar) {
        ar(SCORD_SERIALIZATION_NVP(m_id));
    }

    template <class Archive>
    void
    save(Archive& ar) const {
        ar(SCORD_SERIALIZATION_NVP(m_id));
    }

private:
    std::string m_id;
};

dataset::dataset() = default;

dataset::dataset(std::string id)
    : m_pimpl(std::make_unique<dataset::impl>(std::move(id))) {}

dataset::dataset(ADM_dataset_t dataset) : dataset::dataset(dataset->d_id) {}

dataset::dataset(const dataset& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

dataset::dataset(dataset&&) noexcept = default;

dataset&
dataset::operator=(const dataset& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

dataset&
dataset::operator=(dataset&&) noexcept = default;

dataset::~dataset() = default;

std::string
dataset::id() const {
    return m_pimpl->id();
}

// since the PIMPL class is fully defined at this point, we can now
// define the serialization function
template <class Archive>
inline void
dataset::serialize(Archive& ar) {
    ar(SCORD_SERIALIZATION_NVP(m_pimpl));
}

//  we must also explicitly instantiate our template functions for
//  serialization in the desired archives
template void
dataset::impl::save<network::serialization::output_archive>(
        network::serialization::output_archive&) const;

template void
dataset::impl::load<network::serialization::input_archive>(
        network::serialization::input_archive&);

template void
dataset::serialize<network::serialization::output_archive>(
        network::serialization::output_archive&);

template void
dataset::serialize<network::serialization::input_archive>(
        network::serialization::input_archive&);

adhoc_storage::resources::resources(std::vector<scord::node> nodes)
    : m_nodes(std::move(nodes)) {}

adhoc_storage::resources::resources(ADM_adhoc_resources_t res) {
    assert(res->r_nodes);
    m_nodes.reserve(res->r_nodes->l_length);

    for(size_t i = 0; i < res->r_nodes->l_length; ++i) {
        m_nodes.emplace_back(res->r_nodes->l_nodes[i].n_hostname);
    }
}

adhoc_storage::resources::operator ADM_adhoc_resources_t() const {

    std::vector<ADM_node_t> tmp;
    std::transform(m_nodes.cbegin(), m_nodes.cend(), std::back_inserter(tmp),
                   [](const scord::node& n) {
                       return ADM_node_create(
                               n.hostname().c_str(),
                               static_cast<ADM_node_type_t>(n.get_type()));
                   });

    // N.B. This works because AMD_adhoc_resources_create() internally copies
    // the data from the array passed to it. If that ever changes we will
    // have a problem here...
    return ADM_adhoc_resources_create(tmp.data(), tmp.size());
}

std::vector<scord::node>
adhoc_storage::resources::nodes() const {
    return m_nodes;
}

adhoc_storage::ctx::ctx(std::string controller_address,
                        adhoc_storage::execution_mode exec_mode,
                        adhoc_storage::access_type access_type,
                        std::uint32_t walltime, bool should_flush)
    : m_controller_address(std::move(controller_address)),
      m_exec_mode(exec_mode), m_access_type(access_type), m_walltime(walltime),
      m_should_flush(should_flush) {}

adhoc_storage::ctx::ctx(ADM_adhoc_context_t ctx)
    : adhoc_storage::ctx(ctx->c_ctl_address,
                         static_cast<execution_mode>(ctx->c_mode),
                         static_cast<enum access_type>(ctx->c_access),
                         ctx->c_walltime, ctx->c_should_bg_flush) {}

adhoc_storage::ctx::operator ADM_adhoc_context_t() const {
    return ADM_adhoc_context_create(
            m_controller_address.c_str(),
            static_cast<ADM_adhoc_mode_t>(m_exec_mode),
            static_cast<ADM_adhoc_access_t>(m_access_type), m_walltime,
            m_should_flush);
}

std::string
adhoc_storage::ctx::controller_address() const {
    return m_controller_address;
}

adhoc_storage::execution_mode
adhoc_storage::ctx::exec_mode() const {
    return m_exec_mode;
}

adhoc_storage::access_type
adhoc_storage::ctx::access_type() const {
    return m_access_type;
}

std::uint32_t
adhoc_storage::ctx::walltime() const {
    return m_walltime;
}

bool
adhoc_storage::ctx::should_flush() const {
    return m_should_flush;
}

class adhoc_storage::impl {

public:
    impl() = default;
    explicit impl(enum adhoc_storage::type type, std::string name,
                  std::uint64_t id, adhoc_storage::ctx ctx,
                  struct adhoc_storage::resources resources)
        : m_type(type), m_name(std::move(name)), m_id(id),
          m_ctx(std::move(ctx)), m_resources(std::move(resources)) {}
    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;
    ~impl() = default;

    std::string
    name() const {
        return m_name;
    }

    enum type
    type() const {
        return m_type;
    }

    std::uint64_t
    id() const {
        return m_id;
    }

    adhoc_storage::ctx
    context() const {
        return m_ctx;
    }

    struct adhoc_storage::resources
    resources() const {
        return m_resources;
    };

    void
    update(adhoc_storage::ctx new_ctx) {
        m_ctx = std::move(new_ctx);
    }

    void
    update(scord::adhoc_storage::resources new_resources) {
        m_resources = std::move(new_resources);
    }

    template <class Archive>
    void
    load(Archive& ar) {
        ar(SCORD_SERIALIZATION_NVP(m_type));
        ar(SCORD_SERIALIZATION_NVP(m_name));
        ar(SCORD_SERIALIZATION_NVP(m_id));
        ar(SCORD_SERIALIZATION_NVP(m_ctx));
        ar(SCORD_SERIALIZATION_NVP(m_resources));
    }

    template <class Archive>
    void
    save(Archive& ar) const {
        ar(SCORD_SERIALIZATION_NVP(m_type));
        ar(SCORD_SERIALIZATION_NVP(m_name));
        ar(SCORD_SERIALIZATION_NVP(m_id));
        ar(SCORD_SERIALIZATION_NVP(m_ctx));
        ar(SCORD_SERIALIZATION_NVP(m_resources));
    }


private:
    enum type m_type;
    std::string m_name;
    std::uint64_t m_id;
    adhoc_storage::ctx m_ctx;
    struct adhoc_storage::resources m_resources;
};

adhoc_storage::adhoc_storage() = default;

adhoc_storage::adhoc_storage(ADM_adhoc_storage_t st)
    : m_pimpl(std::make_unique<impl>(
              static_cast<enum adhoc_storage::type>(st->s_type), st->s_name,
              st->s_id, adhoc_storage::ctx{st->s_adhoc_ctx},
              adhoc_storage::resources{st->s_resources})) {}

adhoc_storage::operator ADM_adhoc_storage_t() const {
    return ADM_adhoc_storage_create(
            m_pimpl->name().c_str(),
            static_cast<ADM_adhoc_storage_type_t>(m_pimpl->type()),
            m_pimpl->id(), static_cast<ADM_adhoc_context_t>(m_pimpl->context()),
            static_cast<ADM_adhoc_resources_t>(m_pimpl->resources()));
}

adhoc_storage::adhoc_storage(enum adhoc_storage::type type, std::string name,
                             std::uint64_t id, const adhoc_storage::ctx& ctx,
                             adhoc_storage::resources resources)
    : m_pimpl(std::make_unique<impl>(type, std::move(name), id, ctx,
                                     std::move(resources))) {}

adhoc_storage::adhoc_storage(const adhoc_storage& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

adhoc_storage::adhoc_storage(adhoc_storage&&) noexcept = default;

adhoc_storage&
adhoc_storage::operator=(const adhoc_storage& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

adhoc_storage&
adhoc_storage::operator=(adhoc_storage&&) noexcept = default;

std::string
adhoc_storage::name() const {
    return m_pimpl->name();
}

enum adhoc_storage::type
adhoc_storage::type() const {
    return m_pimpl->type();
}

std::uint64_t
adhoc_storage::id() const {
    return m_pimpl->id();
}

adhoc_storage::ctx
adhoc_storage::context() const {
    return m_pimpl->context();
}

adhoc_storage::resources
adhoc_storage::get_resources() const {
    return m_pimpl->resources();
}

void
adhoc_storage::update(scord::adhoc_storage::ctx new_ctx) {
    return m_pimpl->update(std::move(new_ctx));
}

void
adhoc_storage::update(scord::adhoc_storage::resources new_resources) {
    return m_pimpl->update(std::move(new_resources));
}

// since the PIMPL class is fully defined at this point, we can now
// define the serialization function
template <class Archive>
inline void
adhoc_storage::serialize(Archive& ar) {
    ar(SCORD_SERIALIZATION_NVP(m_pimpl));
}

//  we must also explicitly instantiate our template functions for
//  serialization in the desired archives
template void
adhoc_storage::impl::save<network::serialization::output_archive>(
        network::serialization::output_archive&) const;

template void
adhoc_storage::impl::load<network::serialization::input_archive>(
        network::serialization::input_archive&);

template void
adhoc_storage::serialize<network::serialization::output_archive>(
        network::serialization::output_archive&);

template void
adhoc_storage::serialize<network::serialization::input_archive>(
        network::serialization::input_archive&);

adhoc_storage::~adhoc_storage() = default;

pfs_storage::ctx::ctx(std::filesystem::path mount_point)
    : m_mount_point(std::move(mount_point)) {}

pfs_storage::ctx::ctx(ADM_pfs_context_t ctx) : pfs_storage::ctx(ctx->c_mount) {}

pfs_storage::ctx::operator ADM_pfs_context_t() const {
    return ADM_pfs_context_create(m_mount_point.c_str());
}

std::filesystem::path
pfs_storage::ctx::mount_point() const {
    return m_mount_point;
}

class pfs_storage::impl {

public:
    impl() = default;
    explicit impl(enum pfs_storage::type type, std::string name,
                  std::uint64_t id, pfs_storage::ctx ctx)
        : m_type(type), m_name(std::move(name)), m_id(id),
          m_ctx(std::move(ctx)) {}
    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;
    ~impl() = default;

    enum type
    type() const {
        return m_type;
    };

    std::string
    name() const {
        return m_name;
    }

    std::uint64_t
    id() const {
        return m_id;
    };

    pfs_storage::ctx
    context() const {
        return m_ctx;
    }

    void
    update(pfs_storage::ctx new_ctx) {
        m_ctx = std::move(new_ctx);
    }

    template <class Archive>
    void
    load(Archive& ar) {
        ar(SCORD_SERIALIZATION_NVP(m_type));
        ar(SCORD_SERIALIZATION_NVP(m_name));
        ar(SCORD_SERIALIZATION_NVP(m_id));
        ar(SCORD_SERIALIZATION_NVP(m_ctx));
    }

    template <class Archive>
    void
    save(Archive& ar) const {
        ar(SCORD_SERIALIZATION_NVP(m_type));
        ar(SCORD_SERIALIZATION_NVP(m_name));
        ar(SCORD_SERIALIZATION_NVP(m_id));
        ar(SCORD_SERIALIZATION_NVP(m_ctx));
    }

private:
    enum type m_type;
    std::string m_name;
    std::uint64_t m_id;
    pfs_storage::ctx m_ctx;
};

pfs_storage::pfs_storage() = default;

pfs_storage::pfs_storage(enum pfs_storage::type type, std::string name,
                         std::uint64_t id, std::filesystem::path mount_point)
    : m_pimpl(std::make_unique<impl>(
              type, std::move(name), id,
              pfs_storage::ctx{std::move(mount_point)})) {}

pfs_storage::pfs_storage(enum pfs_storage::type type, std::string name,
                         std::uint64_t id, const pfs_storage::ctx& pfs_ctx)
    : m_pimpl(std::make_unique<impl>(type, std::move(name), id, pfs_ctx)) {}

pfs_storage::pfs_storage(ADM_pfs_storage_t st)
    : m_pimpl(std::make_unique<impl>(
              static_cast<enum pfs_storage::type>(st->s_type), st->s_name,
              st->s_id, pfs_storage::ctx{st->s_pfs_ctx})) {}

pfs_storage::operator ADM_pfs_storage_t() const {
    return ADM_pfs_storage_create(
            m_pimpl->name().c_str(),
            static_cast<ADM_pfs_storage_type_t>(m_pimpl->type()), m_pimpl->id(),
            static_cast<ADM_pfs_context_t>(m_pimpl->context()));
}

pfs_storage::pfs_storage(const pfs_storage& other) noexcept
    : m_pimpl(std::make_unique<impl>(*other.m_pimpl)) {}

pfs_storage::pfs_storage(pfs_storage&&) noexcept = default;

pfs_storage&
pfs_storage::operator=(const pfs_storage& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

pfs_storage&
pfs_storage::operator=(pfs_storage&&) noexcept = default;

pfs_storage::~pfs_storage() = default;

std::string
pfs_storage::name() const {
    return m_pimpl->name();
}

enum pfs_storage::type
pfs_storage::type() const {
    return m_pimpl->type();
}

std::uint64_t
pfs_storage::id() const {
    return m_pimpl->id();
}

pfs_storage::ctx
pfs_storage::context() const {
    return m_pimpl->context();
}

void
pfs_storage::update(scord::pfs_storage::ctx new_ctx) {
    return m_pimpl->update(std::move(new_ctx));
}

// since the PIMPL class is fully defined at this point, we can now
// define the serialization function
template <class Archive>
inline void
pfs_storage::serialize(Archive& ar) {
    ar(SCORD_SERIALIZATION_NVP(m_pimpl));
}

//  we must also explicitly instantiate our template functions for
//  serialization in the desired archives
template void
pfs_storage::impl::save<network::serialization::output_archive>(
        network::serialization::output_archive&) const;

template void
pfs_storage::impl::load<network::serialization::input_archive>(
        network::serialization::input_archive&);

template void
pfs_storage::serialize<network::serialization::output_archive>(
        network::serialization::output_archive&);

template void
pfs_storage::serialize<network::serialization::input_archive>(
        network::serialization::input_archive&);

namespace qos {

class entity::impl {
public:
    impl() = default;

    template <typename T>
    impl(const scord::qos::scope& s, T&& data) : m_scope(s), m_data(data) {}

    explicit impl(ADM_qos_entity_t entity)
        : m_scope(static_cast<qos::scope>(entity->e_scope)),
          m_data(init_helper(entity)) {}

    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;

    scord::qos::scope
    scope() const {
        return m_scope;
    }

    template <typename T>
    T
    data() const {
        return std::get<T>(m_data);
    }

    template <class Archive>
    void
    load(Archive& ar) {
        ar(SCORD_SERIALIZATION_NVP(m_scope));
        ar(SCORD_SERIALIZATION_NVP(m_data));
    }

    template <class Archive>
    void
    save(Archive& ar) const {
        ar(SCORD_SERIALIZATION_NVP(m_scope));
        ar(SCORD_SERIALIZATION_NVP(m_data));
    }

private:
    static std::variant<dataset, node, job, transfer>
    init_helper(ADM_qos_entity_t entity) {
        switch(entity->e_scope) {
            case ADM_QOS_SCOPE_DATASET:
                return scord::dataset(entity->e_dataset);
            case ADM_QOS_SCOPE_NODE:
                return scord::node(entity->e_node);
            case ADM_QOS_SCOPE_JOB:
                return scord::job(entity->e_job);
            case ADM_QOS_SCOPE_TRANSFER:
                return scord::transfer(entity->e_transfer);
            default:
                throw std::runtime_error(fmt::format(
                        "Unexpected scope value: {}", entity->e_scope));
        }
    }


private:
    scord::qos::scope m_scope;
    std::variant<dataset, node, job, transfer> m_data;
};

entity::entity() = default;

template <typename T>
entity::entity(scord::qos::scope s, T&& data)
    : m_pimpl(std::make_unique<entity::impl>(s, std::forward<T>(data))) {}

entity::entity(ADM_qos_entity_t entity)
    : m_pimpl(std::make_unique<entity::impl>(entity)) {}

entity::entity(const entity& other) noexcept
    : m_pimpl(std::make_unique<entity::impl>(*other.m_pimpl)) {}

entity::entity(entity&&) noexcept = default;

entity&
entity::operator=(const entity& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

entity&
entity::operator=(entity&&) noexcept = default;

entity::~entity() = default;

scord::qos::scope
entity::scope() const {
    return m_pimpl->scope();
}

template <>
scord::node
entity::data<scord::node>() const {
    return m_pimpl->data<scord::node>();
}

template <>
scord::job
entity::data<scord::job>() const {
    return m_pimpl->data<scord::job>();
}

template <>
scord::dataset
entity::data<scord::dataset>() const {
    return m_pimpl->data<scord::dataset>();
}

template <>
scord::transfer
entity::data<scord::transfer>() const {
    return m_pimpl->data<scord::transfer>();
}

// since the PIMPL class is fully defined at this point, we can now
// define the serialization function
template <class Archive>
inline void
entity::serialize(Archive& ar) {
    ar(SCORD_SERIALIZATION_NVP(m_pimpl));
}

//  we must also explicitly instantiate our template functions for
//  serialization in the desired archives
template void
entity::impl::save<network::serialization::output_archive>(
        network::serialization::output_archive&) const;

template void
entity::impl::load<network::serialization::input_archive>(
        network::serialization::input_archive&);

template void
entity::serialize<network::serialization::output_archive>(
        network::serialization::output_archive&);

template void
entity::serialize<network::serialization::input_archive>(
        network::serialization::input_archive&);

class limit::impl {

public:
    impl() = default;
    impl(scord::qos::subclass cls, uint64_t value, scord::qos::entity e)
        : m_subclass(cls), m_value(value), m_entity(std::move(e)) {}

    impl(scord::qos::subclass cls, uint64_t value)
        : m_subclass(cls), m_value(value) {}

    explicit impl(ADM_qos_limit_t l)
        : m_subclass(static_cast<qos::subclass>(l->l_class)),
          m_value(l->l_value),
          m_entity(l->l_entity ? std::optional(scord::qos::entity(l->l_entity))
                               : std::nullopt) {}

    impl(const impl& rhs) = default;
    impl(impl&& rhs) = default;
    impl&
    operator=(const impl& other) noexcept = default;
    impl&
    operator=(impl&&) noexcept = default;

    std::optional<scord::qos::entity>
    entity() const {
        return m_entity;
    }

    scord::qos::subclass
    subclass() const {
        return m_subclass;
    }

    uint64_t
    value() const {
        return m_value;
    }

    template <class Archive>
    void
    load(Archive& ar) {
        ar(SCORD_SERIALIZATION_NVP(m_subclass));
        ar(SCORD_SERIALIZATION_NVP(m_value));
        ar(SCORD_SERIALIZATION_NVP(m_entity));
    }

    template <class Archive>
    void
    save(Archive& ar) const {
        ar(SCORD_SERIALIZATION_NVP(m_subclass));
        ar(SCORD_SERIALIZATION_NVP(m_value));
        ar(SCORD_SERIALIZATION_NVP(m_entity));
    }

private:
    scord::qos::subclass m_subclass;
    uint64_t m_value;
    std::optional<scord::qos::entity> m_entity;
};

limit::limit() = default;

limit::limit(scord::qos::subclass cls, uint64_t value)
    : m_pimpl(std::make_unique<limit::impl>(cls, value)) {}

limit::limit(scord::qos::subclass cls, uint64_t value,
             const scord::qos::entity& e)
    : m_pimpl(std::make_unique<limit::impl>(cls, value, e)) {}

limit::limit(ADM_qos_limit_t l) : m_pimpl(std::make_unique<limit::impl>(l)) {}

limit::limit(const limit& other) noexcept
    : m_pimpl(std::make_unique<limit::impl>(*other.m_pimpl)) {}

limit::limit(limit&&) noexcept = default;

limit&
limit::operator=(const limit& other) noexcept {
    this->m_pimpl = std::make_unique<impl>(*other.m_pimpl);
    return *this;
}

limit&
limit::operator=(limit&&) noexcept = default;

limit::~limit() = default;

std::optional<scord::qos::entity>
limit::entity() const {
    return m_pimpl->entity();
}

scord::qos::subclass
limit::subclass() const {
    return m_pimpl->subclass();
}

uint64_t
limit::value() const {
    return m_pimpl->value();
}

// since the PIMPL class is fully defined at this point, we can now
// define the serialization function
template <class Archive>
inline void
limit::serialize(Archive& ar) {
    ar(SCORD_SERIALIZATION_NVP(m_pimpl));
}

//  we must also explicitly instantiate our template functions for
//  serialization in the desired archives
template void
limit::impl::save<network::serialization::output_archive>(
        network::serialization::output_archive&) const;

template void
limit::impl::load<network::serialization::input_archive>(
        network::serialization::input_archive&);

template void
limit::serialize<network::serialization::output_archive>(
        network::serialization::output_archive&);

template void
limit::serialize<network::serialization::input_archive>(
        network::serialization::input_archive&);

} // namespace qos

} // namespace scord
