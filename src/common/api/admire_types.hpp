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

#ifndef SCORD_ADMIRE_TYPES_HPP
#define SCORD_ADMIRE_TYPES_HPP

#include <string>
#include <cstdint>
#include <vector>
#include <filesystem>
#include <fmt/format.h>
#include <utils/ctype_ptr.hpp>
#include <optional>
#include "admire_types.h"

namespace admire {

struct error_code {

    static const error_code success;
    static const error_code snafu;
    static const error_code bad_args;
    static const error_code out_of_memory;
    static const error_code entity_exists;
    static const error_code no_such_entity;
    static const error_code adhoc_in_use;
    static const error_code other;

    constexpr error_code() : m_value(ADM_SUCCESS) {}
    constexpr explicit error_code(ADM_return_t ec) : m_value(ec) {}
    constexpr explicit error_code(int32_t ec)
        : m_value(static_cast<ADM_return_t>(ec)) {}

    constexpr operator ADM_return_t() const { // NOLINT
        return m_value;
    }

    constexpr explicit operator bool() const {
        return m_value == ADM_SUCCESS;
    }

    ADM_return_t
    value() const {
        return m_value;
    }

    constexpr std::string_view
    name() const {
        switch(m_value) {
            case ADM_SUCCESS:
                return "ADM_SUCCESS";
            case ADM_ESNAFU:
                return "ADM_ESNAFU";
            case ADM_EBADARGS:
                return "ADM_EBADARGS";
            case ADM_ENOMEM:
                return "ADM_ENOMEM";
            case ADM_EEXISTS:
                return "ADM_EEXISTS";
            case ADM_ENOENT:
                return "ADM_ENOENT";
            case ADM_EADHOC_BUSY:
                return "ADM_EADHOC_BUSY";
            case ADM_EOTHER:
                return "ADM_EOTHER";
            default:
                return "INVALID_ERROR_VALUE";
        }
    }

    std::string_view
    message() const;

    template <typename Archive>
    void
    serialize(Archive&& ar) {
        ar& m_value;
    }

private:
    ADM_return_t m_value;
};

constexpr error_code error_code::success = error_code{ADM_SUCCESS};
constexpr error_code error_code::snafu = error_code{ADM_ESNAFU};
constexpr error_code error_code::bad_args = error_code{ADM_EBADARGS};
constexpr error_code error_code::out_of_memory = error_code{ADM_ENOMEM};
constexpr error_code error_code::entity_exists = error_code{ADM_EEXISTS};
constexpr error_code error_code::no_such_entity = error_code{ADM_ENOENT};
constexpr error_code error_code::adhoc_in_use = error_code{ADM_EADHOC_BUSY};
constexpr error_code error_code::other = error_code{ADM_EOTHER};

using job_id = std::uint64_t;
using slurm_job_id = std::uint64_t;
using transfer_id = std::uint64_t;

namespace internal {
struct job_info;
struct adhoc_storage_info;
} // namespace internal

struct server {

    server(std::string protocol, std::string address);
    explicit server(const ADM_server_t& srv);
    server(server&&) noexcept;
    server&
    operator=(server&&) noexcept;
    ~server();

    std::string
    protocol() const;
    std::string
    address() const;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct node {

    explicit node(std::string hostname);
    explicit node(const ADM_node_t& srv);
    node(const node&) noexcept;
    node(node&&) noexcept;
    node&
    operator=(const node&) noexcept;
    node&
    operator=(node&&) noexcept;
    ~node();

    std::string
    hostname() const;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct job_requirements;

struct job {

    struct resources {
        explicit resources(std::vector<admire::node> nodes);
        explicit resources(ADM_job_resources_t res);

        std::vector<admire::node>
        nodes() const;

    private:
        std::vector<admire::node> m_nodes;
    };

    job(job_id id, slurm_job_id slurm_id);
    explicit job(ADM_job_t job);
    job(const job&) noexcept;
    job(job&&) noexcept;
    job&
    operator=(job&&) noexcept;
    job&
    operator=(const job&) noexcept;
    ~job();

    job_id
    id() const;

    slurm_job_id
    slurm_id() const;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct transfer {

    enum class mapping : std::underlying_type<ADM_transfer_mapping_t>::type {
        one_to_one = ADM_MAPPING_ONE_TO_ONE,
        one_to_n = ADM_MAPPING_ONE_TO_N,
        n_to_n = ADM_MAPPING_N_TO_N
    };

    explicit transfer(transfer_id id);
    explicit transfer(ADM_transfer_t transfer);

    transfer(const transfer&) noexcept;
    transfer(transfer&&) noexcept;
    transfer&
    operator=(const transfer&) noexcept;
    transfer&
    operator=(transfer&&) noexcept;

    ~transfer();

    transfer_id
    id() const;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

namespace qos {

enum class subclass : std::underlying_type<ADM_qos_class_t>::type {
    bandwidth = ADM_QOS_CLASS_BANDWIDTH,
    iops = ADM_QOS_CLASS_IOPS,
};

enum class scope : std::underlying_type<ADM_qos_scope_t>::type {
    dataset = ADM_QOS_SCOPE_DATASET,
    node = ADM_QOS_SCOPE_NODE,
    job = ADM_QOS_SCOPE_JOB,
    transfer = ADM_QOS_SCOPE_TRANSFER
};

struct entity {

    template <typename T>
    entity(admire::qos::scope s, T&& data);
    explicit entity(ADM_qos_entity_t entity);

    entity(const entity&) noexcept;
    entity(entity&&) noexcept;
    entity&
    operator=(const entity&) noexcept;
    entity&
    operator=(entity&&) noexcept;

    ~entity();

    admire::qos::scope
    scope() const;

    template <typename T>
    T
    data() const;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct limit {

    limit(admire::qos::subclass cls, uint64_t value);
    limit(admire::qos::subclass cls, uint64_t value,
          const admire::qos::entity& e);
    explicit limit(ADM_qos_limit_t l);

    limit(const limit&) noexcept;
    limit(limit&&) noexcept;
    limit&
    operator=(const limit&) noexcept;
    limit&
    operator=(limit&&) noexcept;

    ~limit();

    std::optional<admire::qos::entity>
    entity() const;

    admire::qos::subclass
    subclass() const;

    uint64_t
    value() const;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

} // namespace qos


struct dataset {
    explicit dataset(std::string id);
    explicit dataset(ADM_dataset_t dataset);
    dataset(const dataset&) noexcept;
    dataset(dataset&&) noexcept;
    dataset&
    operator=(const dataset&) noexcept;
    dataset&
    operator=(dataset&&) noexcept;
    ~dataset();

    std::string
    id() const;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct adhoc_storage {

    enum class type : std::underlying_type<ADM_adhoc_storage_type_t>::type {
        gekkofs = ADM_ADHOC_STORAGE_GEKKOFS,
        dataclay = ADM_ADHOC_STORAGE_DATACLAY,
        expand = ADM_ADHOC_STORAGE_EXPAND,
        hercules = ADM_ADHOC_STORAGE_HERCULES,
    };

    enum class execution_mode : std::underlying_type<ADM_adhoc_mode_t>::type {
        in_job_shared = ADM_ADHOC_MODE_IN_JOB_SHARED,
        in_job_dedicated = ADM_ADHOC_MODE_IN_JOB_DEDICATED,
        separate_new = ADM_ADHOC_MODE_SEPARATE_NEW,
        separate_existing = ADM_ADHOC_MODE_SEPARATE_EXISTING
    };

    enum class access_type : std::underlying_type<ADM_adhoc_mode_t>::type {
        read_only = ADM_ADHOC_ACCESS_RDONLY,
        write_only = ADM_ADHOC_ACCESS_WRONLY,
        read_write = ADM_ADHOC_ACCESS_RDWR,
    };

    struct resources {
        explicit resources(std::vector<admire::node> nodes);
        explicit resources(ADM_adhoc_resources_t res);

        std::vector<admire::node>
        nodes() const;

    private:
        std::vector<admire::node> m_nodes;
    };

    struct ctx {

        ctx(execution_mode exec_mode, access_type access_type,
            adhoc_storage::resources resources, std::uint32_t walltime,
            bool should_flush);

        explicit ctx(ADM_adhoc_context_t ctx);

        execution_mode
        exec_mode() const;
        enum access_type
        access_type() const;
        adhoc_storage::resources
        resources() const;
        std::uint32_t
        walltime() const;
        bool
        should_flush() const;

    private:
        execution_mode m_exec_mode;
        enum access_type m_access_type;
        adhoc_storage::resources m_resources;
        std::uint32_t m_walltime;
        bool m_should_flush;
    };

    adhoc_storage(enum adhoc_storage::type type, std::string name,
                  std::uint64_t id, execution_mode exec_mode,
                  access_type access_type, adhoc_storage::resources res,
                  std::uint32_t walltime, bool should_flush);
    explicit adhoc_storage(ADM_adhoc_storage_t storage);
    adhoc_storage(enum adhoc_storage::type type, std::string name,
                  std::uint64_t id, const admire::adhoc_storage::ctx& ctx);

    adhoc_storage(const adhoc_storage& other) noexcept;
    adhoc_storage(adhoc_storage&&) noexcept;
    adhoc_storage&
    operator=(const adhoc_storage&) noexcept;
    adhoc_storage&
    operator=(adhoc_storage&&) noexcept;
    ~adhoc_storage();

    std::string
    name() const;
    type
    type() const;
    std::uint64_t
    id() const;
    adhoc_storage::ctx
    context() const;

    void
    update(admire::adhoc_storage::ctx new_ctx);

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct pfs_storage {

    enum class type : std::underlying_type<ADM_pfs_storage_type_t>::type {
        lustre = ADM_PFS_STORAGE_LUSTRE,
        gpfs = ADM_PFS_STORAGE_GPFS
    };

    struct ctx {

        explicit ctx(std::filesystem::path mount_point);

        explicit ctx(ADM_pfs_context_t ctx);

        std::filesystem::path
        mount_point() const;

    private:
        std::filesystem::path m_mount_point;
    };

    pfs_storage(enum pfs_storage::type type, std::string name, std::uint64_t id,
                std::filesystem::path mount_point);

    pfs_storage(enum pfs_storage::type type, std::string name, std::uint64_t id,
                const pfs_storage::ctx& pfs_ctx);

    explicit pfs_storage(ADM_pfs_storage_t storage);

    pfs_storage(const pfs_storage& other) noexcept;
    pfs_storage(pfs_storage&&) noexcept;
    pfs_storage&
    operator=(const pfs_storage& other) noexcept;
    pfs_storage&
    operator=(pfs_storage&&) noexcept;
    ~pfs_storage();

    std::string
    name() const;
    type
    type() const;
    std::uint64_t
    id() const;
    pfs_storage::ctx
    context() const;

    void
    update(admire::pfs_storage::ctx new_ctx);

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct job_requirements {

    job_requirements(std::vector<admire::dataset> inputs,
                     std::vector<admire::dataset> outputs);

    job_requirements(std::vector<admire::dataset> inputs,
                     std::vector<admire::dataset> outputs,
                     admire::adhoc_storage adhoc_storage);

    explicit job_requirements(ADM_job_requirements_t reqs);

    job_requirements(const job_requirements&) noexcept;
    job_requirements(job_requirements&&) noexcept;
    job_requirements&
    operator=(const job_requirements&) noexcept;
    job_requirements&
    operator=(job_requirements&&) noexcept;

    ~job_requirements();

    std::vector<admire::dataset>
    inputs() const;
    std::vector<admire::dataset>
    outputs() const;
    std::optional<admire::adhoc_storage>
    adhoc_storage() const;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

} // namespace admire


////////////////////////////////////////////////////////////////////////////////
//  Formatting functions
////////////////////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<admire::error_code> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::error_code& ec, FormatContext& ctx) const {
        return formatter<std::string_view>::format(ec.name(), ctx);
    }
};

template <>
struct fmt::formatter<admire::job> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::job& j, FormatContext& ctx) const {
        return formatter<std::string_view>::format(
                fmt::format("{{id: {}, slurm_id: {}}}", j.id(), j.slurm_id()),
                ctx);
    }
};

template <>
struct fmt::formatter<admire::dataset> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::dataset& d, FormatContext& ctx) const {
        const auto str = fmt::format("{{id: {}}}", std::quoted(d.id()));
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<admire::node> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::node& n, FormatContext& ctx) const {
        const auto str =
                fmt::format("{{hostname: {}}}", std::quoted(n.hostname()));
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<enum admire::adhoc_storage::type>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const enum admire::adhoc_storage::type& t,
           FormatContext& ctx) const {

        using admire::adhoc_storage;
        std::string_view name = "unknown";

        switch(t) {
            case adhoc_storage::type::gekkofs:
                name = "ADM_ADHOC_STORAGE_GEKKOFS";
                break;
            case adhoc_storage::type::dataclay:
                name = "ADM_ADHOC_STORAGE_DATACLAY";
                break;
            case adhoc_storage::type::expand:
                name = "ADM_ADHOC_STORAGE_EXPAND";
                break;
            case adhoc_storage::type::hercules:
                name = "ADM_ADHOC_STORAGE_HERCULES";
                break;
        }

        return formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<admire::adhoc_storage::execution_mode>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::adhoc_storage::execution_mode& exec_mode,
           FormatContext& ctx) const {

        using execution_mode = admire::adhoc_storage::execution_mode;

        std::string_view name = "unknown";

        switch(exec_mode) {
            case execution_mode::in_job_shared:
                name = "IN_JOB_SHARED";
                break;
            case execution_mode::in_job_dedicated:
                name = "IN_JOB_DEDICATED";
                break;
            case execution_mode::separate_new:
                name = "SEPARATE_NEW";
                break;
            case execution_mode::separate_existing:
                name = "SEPARATE_EXISTING";
                break;
        }

        return formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<admire::adhoc_storage::access_type>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::adhoc_storage::access_type& type,
           FormatContext& ctx) const {

        using access_type = admire::adhoc_storage::access_type;

        std::string_view name = "unknown";

        switch(type) {
            case access_type::read_only:
                name = "RDONLY";
                break;
            case access_type::write_only:
                name = "WRONLY";
                break;
            case access_type::read_write:
                name = "RDWR";
                break;
        }

        return formatter<std::string_view>::format(name, ctx);
    }
};

template <typename T>
struct fmt::formatter<std::optional<T>> : formatter<std::string_view> {

    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::optional<T>& v, FormatContext& ctx) const {
        return formatter<std::string_view>::format(
                v ? fmt::format("{}", v.value()) : "none", ctx);
    }
};

template <>
struct fmt::formatter<admire::adhoc_storage> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::adhoc_storage& s, FormatContext& ctx) const {
        const auto str = fmt::format(
                "{{type: {}, id: {}, name: {}, context: {}}}", s.type(), s.id(),
                std::quoted(s.name()), s.context());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<admire::adhoc_storage::resources>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::adhoc_storage::resources& r,
           FormatContext& ctx) const {

        const auto str = fmt::format("{{nodes: {}}}", r.nodes());

        return formatter<std::string_view>::format(str, ctx);
    }
};


template <>
struct fmt::formatter<admire::adhoc_storage::ctx>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::adhoc_storage::ctx& c, FormatContext& ctx) const {

        const auto str =
                fmt::format("{{execution_mode: {}, access_type: {}, "
                            "resources: {}, walltime: {}, should_flush: {}}}",
                            c.exec_mode(), c.access_type(), c.resources(),
                            c.walltime(), c.should_flush());

        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<enum admire::pfs_storage::type>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const enum admire::pfs_storage::type& t, FormatContext& ctx) const {

        using admire::pfs_storage;
        std::string_view name = "unknown";

        switch(t) {
            case pfs_storage::type::lustre:
                name = "ADM_PFS_STORAGE_LUSTRE";
                break;
            case pfs_storage::type::gpfs:
                name = "ADM_PFS_STORAGE_GPFS";
                break;
        }

        return formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<admire::pfs_storage> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::pfs_storage& s, FormatContext& ctx) const {
        const auto str = fmt::format("{{context: {}}}", s.context());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<admire::pfs_storage::ctx> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::pfs_storage::ctx& c, FormatContext& ctx) const {
        const auto str = fmt::format("{{mount_point: {}}}", c.mount_point());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<admire::job::resources> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::job::resources& r, FormatContext& ctx) const {
        const auto str = fmt::format("{{nodes: {}}}", r.nodes());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<admire::job_requirements> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::job_requirements& r, FormatContext& ctx) const {
        return formatter<std::string_view>::format(
                fmt::format("{{inputs: {}, outputs: {}, adhoc_storage: {}}}",
                            r.inputs(), r.outputs(), r.adhoc_storage()),
                ctx);
    }
};

template <>
struct fmt::formatter<admire::qos::scope> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::qos::scope& s, FormatContext& ctx) const {

        using scope = admire::qos::scope;

        std::string_view name = "unknown";

        switch(s) {
            case scope::dataset:
                name = "ADM_QOS_SCOPE_DATASET";
                break;
            case scope::node:
                name = "ADM_QOS_SCOPE_NODE";
                break;
            case scope::job:
                name = "ADM_QOS_SCOPE_JOB";
                break;
            case scope::transfer:
                name = "ADM_QOS_SCOPE_TRANSFER";
                break;
        }

        return formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<std::optional<admire::qos::entity>>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::optional<admire::qos::entity>& e,
           FormatContext& ctx) const {

        if(!e) {
            return formatter<std::string_view>::format("none", ctx);
        }

        std::string_view data = "unknown";

        switch(e->scope()) {
            case admire::qos::scope::dataset:
                data = fmt::format("{}", e->data<admire::dataset>());
                break;
            case admire::qos::scope::node:
                data = fmt::format("{}", e->data<admire::node>());
                break;
            case admire::qos::scope::job:
                data = fmt::format("{}", e->data<admire::job>());
                break;
            case admire::qos::scope::transfer:
                data = fmt::format("{}", e->data<admire::transfer>());
                break;
        }

        const auto str =
                fmt::format("{{scope: {}, data: {}}}", e->scope(), data);
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<admire::qos::subclass> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::qos::subclass& sc, FormatContext& ctx) const {

        using subclass = admire::qos::subclass;

        std::string_view name = "unknown";

        switch(sc) {
            case subclass::bandwidth:
                name = "ADM_QOS_CLASS_BANDWIDTH";
                break;
            case subclass::iops:
                name = "ADM_QOS_CLASS_IOPS";
                break;
        }

        return formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<admire::qos::limit> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::qos::limit& l, FormatContext& ctx) const {
        const auto str = fmt::format("{{entity: {}, subclass: {}, value: {}}}",
                                     l.entity(), l.subclass(), l.value());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<admire::transfer::mapping> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::transfer::mapping& m, FormatContext& ctx) const {

        using mapping = admire::transfer::mapping;

        std::string_view name = "unknown";

        switch(m) {
            case mapping::one_to_one:
                name = "ADM_MAPPING_ONE_TO_ONE";
                break;
            case mapping::one_to_n:
                name = "ADM_MAPPING_ONE_TO_N";
                break;
            case mapping::n_to_n:
                name = "ADM_MAPPING_N_TO_N";
                break;
        }

        return formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<admire::transfer> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::transfer& tx, FormatContext& ctx) const {
        const auto str = fmt::format("{{id: {}}}", tx.id());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<std::vector<admire::node>> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::vector<admire::node>& v, FormatContext& ctx) const {
        const auto str = fmt::format("[{}]", fmt::join(v, ", "));
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<std::vector<admire::dataset>>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::vector<admire::dataset>& v, FormatContext& ctx) const {
        const auto str = fmt::format("[{}]", fmt::join(v, ", "));
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<std::vector<admire::qos::limit>>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::vector<admire::qos::limit>& l, FormatContext& ctx) const {
        const auto str = fmt::format("[{}]", fmt::join(l, ", "));
        return formatter<std::string_view>::format(str, ctx);
    }
};

#endif // SCORD_ADMIRE_TYPES_HPP
