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

#ifndef SCORD_TYPES_HPP
#define SCORD_TYPES_HPP

#include <string>
#include <cstdint>
#include <vector>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/std.h>
#include <utils/ctype_ptr.hpp>
#include <optional>
#include <cereal/access.hpp>
#include <scord/types.h>

namespace scord {

struct error_code {

    static const error_code success;
    static const error_code snafu;
    static const error_code bad_args;
    static const error_code out_of_memory;
    static const error_code entity_exists;
    static const error_code no_such_entity;
    static const error_code adhoc_in_use;
    static const error_code adhoc_type_unsupported;
    static const error_code adhoc_dir_create_failed;
    static const error_code adhoc_dir_exists;
    static const error_code subprocess_error;
    static const error_code no_resources;
    static const error_code timeout;
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
        // clang-format off
#define ADM_ERROR_CASE(x) case x: return #x
#define ADM_ERROR_DEFAULT_MSG(x) default: return x
        // clang-format on
        switch(m_value) {
            ADM_ERROR_CASE(ADM_SUCCESS);
            ADM_ERROR_CASE(ADM_ESNAFU);
            ADM_ERROR_CASE(ADM_EBADARGS);
            ADM_ERROR_CASE(ADM_ENOMEM);
            ADM_ERROR_CASE(ADM_EEXISTS);
            ADM_ERROR_CASE(ADM_ENOENT);
            ADM_ERROR_CASE(ADM_EADHOC_BUSY);
            ADM_ERROR_CASE(ADM_EADHOC_TYPE_UNSUPPORTED);
            ADM_ERROR_CASE(ADM_EADHOC_DIR_CREATE_FAILED);
            ADM_ERROR_CASE(ADM_EADHOC_DIR_EXISTS);
            ADM_ERROR_CASE(ADM_ESUBPROCESS_ERROR);
            ADM_ERROR_CASE(ADM_ETIMEOUT);
            ADM_ERROR_CASE(ADM_EOTHER);
            ADM_ERROR_DEFAULT_MSG("INVALID_ERROR_VALUE");
        }
#undef ADM_ERROR_CASE
#undef ADM_ERROR_DEFAULT_MSG
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

constexpr error_code error_code::success{ADM_SUCCESS};
constexpr error_code error_code::snafu{ADM_ESNAFU};
constexpr error_code error_code::bad_args{ADM_EBADARGS};
constexpr error_code error_code::out_of_memory{ADM_ENOMEM};
constexpr error_code error_code::entity_exists{ADM_EEXISTS};
constexpr error_code error_code::no_such_entity{ADM_ENOENT};
constexpr error_code error_code::adhoc_in_use{ADM_EADHOC_BUSY};
constexpr error_code error_code::adhoc_type_unsupported{
        ADM_EADHOC_TYPE_UNSUPPORTED};
constexpr error_code error_code::adhoc_dir_create_failed{
        ADM_EADHOC_DIR_CREATE_FAILED};
constexpr error_code error_code::adhoc_dir_exists{ADM_EADHOC_DIR_EXISTS};
constexpr error_code error_code::subprocess_error{ADM_ESUBPROCESS_ERROR};
constexpr error_code error_code::no_resources{ADM_ENO_RESOURCES};
constexpr error_code error_code::timeout{ADM_ETIMEOUT};
constexpr error_code error_code::other{ADM_EOTHER};

using job_id = std::uint64_t;
using slurm_job_id = std::uint64_t;
using transfer_id = std::uint64_t;

namespace internal {
struct job_metadata;
struct adhoc_storage_metadata;
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

    enum class type : std::underlying_type<ADM_node_type_t>::type {
        regular = ADM_NODE_REGULAR,
        administrative = ADM_NODE_ADMINISTRATIVE,
    };

    node();
    explicit node(std::string hostname,
                  node::type node_type = node::type::regular);
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

    node::type
    get_type() const;

    // The implementation for this must be deferred until
    // after the declaration of the PIMPL class
    template <class Archive>
    void
    serialize(Archive& ar);

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct dataset;
struct dataset_route;

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
        resources() = default;
        explicit resources(std::vector<scord::node> nodes);
        explicit resources(ADM_adhoc_resources_t res);
        explicit operator ADM_adhoc_resources_t() const;

        std::vector<scord::node>
        nodes() const;

        template <typename Archive>
        void
        serialize(Archive&& ar) {
            ar& m_nodes;
        }

    private:
        std::vector<scord::node> m_nodes;
    };

    struct ctx {

        ctx() = default;

        ctx(std::string controller_address, std::string data_stager_address,
            execution_mode exec_mode, access_type access_type,
            std::uint32_t walltime, bool should_flush);

        explicit ctx(ADM_adhoc_context_t ctx);
        explicit operator ADM_adhoc_context_t() const;

        std::string const&
        controller_address() const;

        std::string const&
        data_stager_address() const;

        execution_mode
        exec_mode() const;
        enum access_type
        access_type() const;
        std::uint32_t
        walltime() const;
        bool
        should_flush() const;

        template <class Archive>
        void
        serialize(Archive&& ar) {
            ar & m_controller_address;
            ar & m_data_stager_address;
            ar & m_exec_mode;
            ar & m_access_type;
            ar & m_walltime;
            ar & m_should_flush;
        }

    private:
        std::string m_controller_address;
        std::string m_data_stager_address;
        execution_mode m_exec_mode;
        enum access_type m_access_type;
        std::uint32_t m_walltime;
        bool m_should_flush;
    };

    adhoc_storage();
    explicit adhoc_storage(ADM_adhoc_storage_t storage);
    explicit operator ADM_adhoc_storage_t() const;
    adhoc_storage(enum adhoc_storage::type type, std::string name,
                  std::uint64_t id, const scord::adhoc_storage::ctx& ctx,
                  adhoc_storage::resources resources);

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
    adhoc_storage::ctx const&
    context() const;

    adhoc_storage::resources
    get_resources() const;

    void
    update(scord::adhoc_storage::ctx new_ctx);

    void
    update(scord::adhoc_storage::resources new_resources);

    // The implementation for this must be deferred until
    // after the declaration of the PIMPL class
    template <class Archive>
    void
    serialize(Archive& ar);

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

        ctx() = default;

        explicit ctx(std::filesystem::path mount_point);

        explicit ctx(ADM_pfs_context_t ctx);
        explicit operator ADM_pfs_context_t() const;

        std::filesystem::path
        mount_point() const;

        template <class Archive>
        void
        serialize(Archive&& ar) {
            ar& m_mount_point;
        }

    private:
        std::filesystem::path m_mount_point;
    };

    pfs_storage();

    pfs_storage(enum pfs_storage::type type, std::string name, std::uint64_t id,
                std::filesystem::path mount_point);

    pfs_storage(enum pfs_storage::type type, std::string name, std::uint64_t id,
                const pfs_storage::ctx& pfs_ctx);

    explicit pfs_storage(ADM_pfs_storage_t storage);
    explicit operator ADM_pfs_storage_t() const;

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
    update(scord::pfs_storage::ctx new_ctx);

    // The implementation for this must be deferred until
    // after the declaration of the PIMPL class
    template <class Archive>
    void
    serialize(Archive& ar);

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct job {

    struct resources {
        resources();
        explicit resources(std::vector<scord::node> nodes);
        explicit resources(ADM_job_resources_t res);

        std::vector<scord::node>
        nodes() const;

        template <typename Archive>
        void
        serialize(Archive&& ar) {
            ar& m_nodes;
        }

    private:
        std::vector<scord::node> m_nodes;
    };

    struct requirements {

        requirements();
        requirements(std::vector<scord::dataset_route> inputs,
                     std::vector<scord::dataset_route> outputs,
                     std::vector<scord::dataset_route> expected_outputs);
        requirements(std::vector<scord::dataset_route> inputs,
                     std::vector<scord::dataset_route> outputs,
                     std::vector<scord::dataset_route> expected_outputs,
                     scord::adhoc_storage adhoc_storage);
        explicit requirements(ADM_job_requirements_t reqs);

        std::vector<scord::dataset_route> const&
        inputs() const;
        std::vector<scord::dataset_route> const&
        outputs() const;
        std::vector<scord::dataset_route> const&
        expected_outputs() const;
        std::optional<scord::adhoc_storage>
        adhoc_storage() const;

        // The implementation for this must be deferred until
        // after the declaration of the PIMPL class
        template <class Archive>
        void
        serialize(Archive& ar) {
            ar & m_inputs;
            ar & m_outputs;
            ar & m_expected_outputs;
            ar & m_adhoc_storage;
        }

    private:
        std::vector<scord::dataset_route> m_inputs;
        std::vector<scord::dataset_route> m_outputs;
        std::vector<scord::dataset_route> m_expected_outputs;
        std::optional<scord::adhoc_storage> m_adhoc_storage;
    };

    job();
    job(job_id id, slurm_job_id slurm_id);
    explicit job(ADM_job_t job);
    explicit operator ADM_job_t() const;
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

    friend class cereal::access;
    template <class Archive>
    void
    serialize(Archive& ar);
};

/**
 * Information about a job.
 */
class job_info {

public:
    job_info() = default;
    explicit job_info(std::string adhoc_controller_address,
                      std::uint32_t procs_for_io)
        : m_adhoc_address(std::move(adhoc_controller_address)),
          m_procs_for_io(procs_for_io) {}

    constexpr std::string const&
    adhoc_controller_address() const {
        return m_adhoc_address;
    }

    /**
     * @brief Get the number of processes that should be used for I/O.
     * @return The number of processes that should be used for I/O.
     */
    constexpr std::uint32_t
    io_procs() const {
        return m_procs_for_io;
    }

private:
    friend class cereal::access;
    template <class Archive>
    void
    serialize(Archive& ar) {
        ar & m_adhoc_address;
        ar & m_procs_for_io;
    }

    std::string m_adhoc_address;
    std::uint32_t m_procs_for_io;
};

struct transfer {

    enum class mapping : std::underlying_type<ADM_transfer_mapping_t>::type {
        one_to_one = ADM_MAPPING_ONE_TO_ONE,
        one_to_n = ADM_MAPPING_ONE_TO_N,
        n_to_n = ADM_MAPPING_N_TO_N
    };

    transfer();
    explicit transfer(transfer_id id);
    explicit transfer(ADM_transfer_t transfer);
    explicit operator ADM_transfer_t() const;

    transfer(const transfer&) noexcept;
    transfer(transfer&&) noexcept;
    transfer&
    operator=(const transfer&) noexcept;
    transfer&
    operator=(transfer&&) noexcept;

    ~transfer();

    transfer_id
    id() const;

    // The implementation for this must be deferred until
    // after the declaration of the PIMPL class
    template <class Archive>
    void
    serialize(Archive& ar);

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

    entity();
    template <typename T>
    entity(scord::qos::scope s, T&& data);
    explicit entity(ADM_qos_entity_t entity);

    entity(const entity&) noexcept;
    entity(entity&&) noexcept;
    entity&
    operator=(const entity&) noexcept;
    entity&
    operator=(entity&&) noexcept;

    ~entity();

    scord::qos::scope
    scope() const;

    template <typename T>
    T
    data() const;

    // The implementation for this must be deferred until
    // after the declaration of the PIMPL class
    template <class Archive>
    void
    serialize(Archive& ar);

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct limit {

    limit();
    limit(scord::qos::subclass cls, uint64_t value);
    limit(scord::qos::subclass cls, uint64_t value,
          const scord::qos::entity& e);
    explicit limit(ADM_qos_limit_t l);

    limit(const limit&) noexcept;
    limit(limit&&) noexcept;
    limit&
    operator=(const limit&) noexcept;
    limit&
    operator=(limit&&) noexcept;

    ~limit();

    std::optional<scord::qos::entity>
    entity() const;

    scord::qos::subclass
    subclass() const;

    uint64_t
    value() const;

    // The implementation for this must be deferred until
    // after the declaration of the PIMPL class
    template <class Archive>
    void
    serialize(Archive& ar);

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

} // namespace qos


struct dataset {
    dataset();
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

    // The implementation for this must be deferred until
    // after the declaration of the PIMPL class
    template <class Archive>
    void
    serialize(Archive& ar);

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct dataset_route {
    dataset_route();
    explicit dataset_route(scord::dataset src, scord::dataset dst);
    explicit dataset_route(ADM_dataset_route_t route);
    dataset_route(const dataset_route&) noexcept;
    dataset_route(dataset_route&&) noexcept;
    dataset_route&
    operator=(const dataset_route&) noexcept;
    dataset_route&
    operator=(dataset_route&&) noexcept;
    ~dataset_route();

    scord::dataset const&
    source() const;

    scord::dataset const&
    destination() const;

    template <class Archive>
    void
    serialize(Archive& ar);

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

} // namespace scord


////////////////////////////////////////////////////////////////////////////////
//  Formatting functions
////////////////////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<scord::error_code> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::error_code& ec, FormatContext& ctx) const -> format_context::iterator {
        return formatter<std::string_view>::format(ec.name(), ctx);
    }
};

template <>
struct fmt::formatter<scord::job_info> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::job_info& ji, FormatContext& ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "{{adhoc_controller: {}, io_procs: {}}}",
                         ji.adhoc_controller_address(), ji.io_procs());
    }
};

template <>
struct fmt::formatter<scord::job> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::job& j, FormatContext& ctx) const -> format_context::iterator {
        return formatter<std::string_view>::format(
                fmt::format("{{id: {}, slurm_id: {}}}", j.id(), j.slurm_id()),
                ctx);
    }
};

template <>
struct fmt::formatter<scord::dataset> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::dataset& d, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("{{id: {:?}}}", d.id());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<std::vector<scord::dataset>>
    : fmt::formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::vector<scord::dataset>& v, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("[{}]", fmt::join(v, ", "));
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<scord::dataset_route> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::dataset_route& r, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("{{src: {}, dst: {}}}", r.source(),
                                     r.destination());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<std::vector<scord::dataset_route>>
    : fmt::formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::vector<scord::dataset_route>& v,
           FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("[{}]", fmt::join(v, ", "));
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<scord::node::type> : fmt::formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::node::type& t, FormatContext& ctx) const -> format_context::iterator {

        using scord::node;
        std::string_view name = "unknown";

        switch(t) {
            case node::type::regular:
                name = "regular";
                break;

            case node::type::administrative:
                name = "administrative";
                break;
        }

        return formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<scord::node> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::node& n, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("{{hostname: {:?}, type: {}}}",
                                    n.hostname(), n.get_type());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<std::vector<scord::node>>
    : fmt::formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::vector<scord::node>& v, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("[{}]", fmt::join(v, ", "));
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<scord::transfer::mapping> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::transfer::mapping& m, FormatContext& ctx) const -> format_context::iterator {

        using mapping = scord::transfer::mapping;

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
struct fmt::formatter<scord::transfer> : fmt::formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::transfer& tx, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("{{id: {}}}", tx.id());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<enum scord::adhoc_storage::type> {

    // Presentation format: 'f' - full, 'e' - enum
    char m_presentation = 'f';

    constexpr auto
    parse(format_parse_context& ctx) -> decltype(ctx.begin()) {

        auto it = ctx.begin(), end = ctx.end();
        if(it != end && (*it == 'f' || *it == 'e')) {
            m_presentation = *it++;
        }

        if(it != end && *it != '}') {
            ctx.on_error("invalid format");
        }

        return it;
    }

    template <typename FormatContext>
    auto
    format(const enum scord::adhoc_storage::type& t, FormatContext& ctx) const -> format_context::iterator {

        using scord::adhoc_storage;
        std::string_view name = "unknown";

        switch(t) {
            case adhoc_storage::type::gekkofs:
                name = m_presentation == 'f' ? "ADM_ADHOC_STORAGE_GEKKOFS"
                                             : "gekkofs";
                break;
            case adhoc_storage::type::dataclay:
                name = m_presentation == 'f' ? "ADM_ADHOC_STORAGE_DATACLAY"
                                             : "dataclay";
                break;
            case adhoc_storage::type::expand:
                name = m_presentation == 'f' ? "ADM_ADHOC_STORAGE_EXPAND"
                                             : "expand";
                break;
            case adhoc_storage::type::hercules:
                name = m_presentation == 'f' ? "ADM_ADHOC_STORAGE_HERCULES"
                                             : "hercules";
                break;
        }

        return fmt::format_to(ctx.out(), "{}", name);
    }
};

template <>
struct fmt::formatter<scord::adhoc_storage::execution_mode>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::adhoc_storage::execution_mode& exec_mode,
           FormatContext& ctx) const -> format_context::iterator {

        using execution_mode = scord::adhoc_storage::execution_mode;

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
struct fmt::formatter<scord::adhoc_storage::access_type>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::adhoc_storage::access_type& type,
           FormatContext& ctx) const -> format_context::iterator {

        using access_type = scord::adhoc_storage::access_type;

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

template <>
struct fmt::formatter<scord::adhoc_storage::ctx> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::adhoc_storage::ctx& c, FormatContext& ctx) const -> format_context::iterator {
        return fmt::format_to(
                ctx.out(),
                "{{controller: {:?}, data_stager: {:?}, execution_mode: {}, "
                "access_type: {}, walltime: {}, should_flush: {}}}",
                c.controller_address(),
                c.data_stager_address(), c.exec_mode(),
                c.access_type(), c.walltime(), c.should_flush());
    }
};

template <>
struct fmt::formatter<std::nullopt_t> : formatter<std::string_view> {

    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::nullopt_t& /*t*/, FormatContext& ctx) const -> format_context::iterator {
        return formatter<std::string_view>::format("none", ctx);
    }
};

template <typename T>
struct fmt::formatter<std::optional<T>> : formatter<std::string_view> {

    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::optional<T>& v, FormatContext& ctx) const -> format_context::iterator {
        return formatter<std::string_view>::format(
                v ? fmt::format("{}", v.value()) : "none", ctx);
    }
};

template <>
struct fmt::formatter<scord::adhoc_storage> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::adhoc_storage& s, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format(
                "{{type: {}, id: {}, name: {:?}, context: {}}}", s.type(), s.id(),
                s.name(), s.context());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<scord::adhoc_storage::resources>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::adhoc_storage::resources& r, FormatContext& ctx) const -> format_context::iterator {

        const auto str = fmt::format("{{nodes: {}}}", r.nodes());

        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<enum scord::pfs_storage::type>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const enum scord::pfs_storage::type& t, FormatContext& ctx) const -> format_context::iterator {

        using scord::pfs_storage;
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
struct fmt::formatter<scord::pfs_storage::ctx> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::pfs_storage::ctx& c, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("{{mount_point: {}}}", c.mount_point());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<scord::pfs_storage> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::pfs_storage& s, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("{{context: {}}}", s.context());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<scord::job::resources> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::job::resources& r, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("{{nodes: {}}}", r.nodes());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<scord::job::requirements> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::job::requirements& r, FormatContext& ctx) const -> format_context::iterator {
        return formatter<std::string_view>::format(
                fmt::format("{{inputs: {}, outputs: {}, "
                            "expected_outputs: {}, adhoc_storage: {}}}",
                            r.inputs(), r.outputs(), r.expected_outputs(),
                            r.adhoc_storage()),
                ctx);
    }
};

template <>
struct fmt::formatter<scord::qos::scope> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::qos::scope& s, FormatContext& ctx) const -> format_context::iterator {

        using scope = scord::qos::scope;

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
struct fmt::formatter<std::optional<scord::qos::entity>>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::optional<scord::qos::entity>& e,
           FormatContext& ctx) const -> format_context::iterator {

        if(!e) {
            return formatter<std::string_view>::format("none", ctx);
        }

        std::string_view data = "unknown";

        switch(e->scope()) {
            case scord::qos::scope::dataset:
                data = fmt::format("{}", e->data<scord::dataset>());
                break;
            case scord::qos::scope::node:
                data = fmt::format("{}", e->data<scord::node>());
                break;
            case scord::qos::scope::job:
                data = fmt::format("{}", e->data<scord::job>());
                break;
            case scord::qos::scope::transfer:
                data = fmt::format("{}", e->data<scord::transfer>());
                break;
        }

        const auto str =
                fmt::format("{{scope: {}, data: {}}}", e->scope(), data);
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<scord::qos::subclass> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::qos::subclass& sc, FormatContext& ctx) const -> format_context::iterator {

        using subclass = scord::qos::subclass;

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
struct fmt::formatter<scord::qos::limit> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const scord::qos::limit& l, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("{{entity: {}, subclass: {}, value: {}}}",
                                     l.entity(), l.subclass(), l.value());
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<std::vector<scord::qos::limit>>
    : fmt::formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::vector<scord::qos::limit>& l, FormatContext& ctx) const -> format_context::iterator {
        const auto str = fmt::format("[{}]", fmt::join(l, ", "));
        return formatter<std::string_view>::format(str, ctx);
    }
};

#endif // SCORD_TYPES_HPP
