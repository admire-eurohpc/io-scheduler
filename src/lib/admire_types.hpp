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
#include <utils/c_ptr.hpp>
#include "admire_types.h"

namespace admire {

using error_code = ADM_return_t;

using job_id = std::uint64_t;

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

struct job {

    explicit job(job_id id);
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

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

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

struct storage {

    enum class type : std::underlying_type<ADM_storage_type_t>::type {
        gekkofs = ADM_STORAGE_GEKKOFS,
        dataclay = ADM_STORAGE_DATACLAY,
        expand = ADM_STORAGE_EXPAND,
        hercules = ADM_STORAGE_HERCULES,
        lustre = ADM_STORAGE_LUSTRE,
        gpfs = ADM_STORAGE_GPFS
    };

    struct ctx {
        virtual ~ctx() = default;
    };

    storage(storage::type type, std::string id);

    virtual ~storage() = default;

    std::string
    id() const;
    type
    type() const;

    virtual std::shared_ptr<ctx>
    context() const = 0;

protected:
    std::string m_id;
    enum type m_type;
};

struct adhoc_storage : public storage {

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

    struct ctx : storage::ctx {

        ctx(execution_mode exec_mode, access_type access_type,
            std::uint32_t nodes, std::uint32_t walltime, bool should_flush);

        explicit ctx(ADM_adhoc_context_t ctx);

        execution_mode
        exec_mode() const;
        enum access_type
        access_type() const;
        std::uint32_t
        nodes() const;
        std::uint32_t
        walltime() const;
        bool
        should_flush() const;

    private:
        execution_mode m_exec_mode;
        enum access_type m_access_type;
        std::uint32_t m_nodes;
        std::uint32_t m_walltime;
        bool m_should_flush;
    };

    adhoc_storage(enum storage::type type, std::string id,
                  execution_mode exec_mode, access_type access_type,
                  std::uint32_t nodes, std::uint32_t walltime,
                  bool should_flush);
    adhoc_storage(enum storage::type type, std::string id,
                  ADM_adhoc_context_t ctx);
    adhoc_storage(adhoc_storage&&) noexcept = default;
    adhoc_storage&
    operator=(adhoc_storage&&) noexcept = default;
    ~adhoc_storage() override;

    std::shared_ptr<storage::ctx>
    context() const final;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct pfs_storage : public storage {

    struct ctx : storage::ctx {

        explicit ctx(std::filesystem::path mount_point);

        explicit ctx(ADM_pfs_context_t ctx);

        std::filesystem::path
        mount_point() const;

    private:
        std::filesystem::path m_mount_point;
    };

    pfs_storage(enum storage::type type, std::string id,
                std::filesystem::path mount_point);
    pfs_storage(enum storage::type type, std::string id, ADM_pfs_context_t ctx);
    pfs_storage(pfs_storage&&) noexcept = default;
    pfs_storage&
    operator=(pfs_storage&&) noexcept = default;
    ~pfs_storage() override;

    std::shared_ptr<storage::ctx>
    context() const final;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

struct job_requirements {

    job_requirements(std::vector<admire::dataset> inputs,
                     std::vector<admire::dataset> outputs);

    job_requirements(std::vector<admire::dataset> inputs,
                     std::vector<admire::dataset> outputs,
                     std::unique_ptr<storage> storage);

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
    std::shared_ptr<admire::storage>
    storage() const;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

template <typename T>
struct managed_rpc_type;

template <typename T>
struct unmanaged_rpc_type;

} // namespace admire


////////////////////////////////////////////////////////////////////////////////
//  Specializations for conversion types
////////////////////////////////////////////////////////////////////////////////

template <>
struct admire::managed_rpc_type<admire::adhoc_storage::ctx> {

    template <typename T, auto Deleter>
    using managed_ptr = scord::utils::c_ptr<T, Deleter>;

    explicit managed_rpc_type(const admire::adhoc_storage::ctx& ctx)
        : m_adhoc_context(ADM_adhoc_context_create(
                  static_cast<ADM_adhoc_mode_t>(ctx.exec_mode()),
                  static_cast<ADM_adhoc_access_t>(ctx.access_type()),
                  ctx.nodes(), ctx.walltime(), ctx.should_flush())) {}

    ADM_adhoc_context_t
    get() const {
        return m_adhoc_context.get();
    }

    managed_ptr<ADM_adhoc_context_t, ADM_adhoc_context_destroy> m_adhoc_context;
};


template <>
struct admire::managed_rpc_type<admire::adhoc_storage> {

    template <typename T, auto Deleter>
    using rpc_storage_ptr = scord::utils::c_ptr<T, Deleter>;

    template <typename T, auto Deleter>
    using managed_ptr = scord::utils::c_ptr<T, Deleter>;

    explicit managed_rpc_type(const admire::adhoc_storage& st)
        : m_adhoc_context(*std::static_pointer_cast<admire::adhoc_storage::ctx>(
                  st.context())),
          m_storage(ADM_storage_create(
                  st.id().c_str(), static_cast<ADM_storage_type_t>(st.type()),
                  m_adhoc_context.get())) {}

    ADM_storage_t
    get() const {
        return m_storage.get();
    }

    managed_rpc_type<admire::adhoc_storage::ctx> m_adhoc_context;
    rpc_storage_ptr<ADM_storage_t, ADM_storage_destroy> m_storage;
};

template <>
struct admire::managed_rpc_type<std::vector<admire::dataset>> {

    template <typename T, auto Deleter>
    using managed_ptr_vector = scord::utils::c_ptr_vector<T, Deleter>;

    explicit managed_rpc_type(const std::vector<admire::dataset>& datasets) {
        m_datasets.reserve(datasets.size());

        for(const auto& d : datasets) {
            m_datasets.emplace_back(ADM_dataset_create(d.id().c_str()));
        }
    }

    const ADM_dataset_t*
    data() const {
        return m_datasets.data();
    }

    ADM_dataset_t*
    data() {
        return m_datasets.data();
    }

    std::size_t
    size() const {
        return m_datasets.size();
    }

    managed_ptr_vector<ADM_dataset_t, ADM_dataset_destroy> m_datasets;
};

template <>
struct admire::managed_rpc_type<admire::job_requirements> {

    template <typename T, auto Deleter>
    using rpc_requirements_ptr = scord::utils::c_ptr<T, Deleter>;

    explicit managed_rpc_type(const admire::job_requirements& reqs)
        : m_inputs(reqs.inputs()), m_outputs(reqs.outputs()),
          m_storage(*std::dynamic_pointer_cast<admire::adhoc_storage>(
                  reqs.storage())),
          m_reqs(ADM_job_requirements_create(m_inputs.data(), m_inputs.size(),
                                             m_outputs.data(), m_outputs.size(),
                                             m_storage.get())) {}

    ADM_job_requirements_t
    get() const {
        return m_reqs.get();
    }

    managed_rpc_type<std::vector<admire::dataset>> m_inputs;
    managed_rpc_type<std::vector<admire::dataset>> m_outputs;
    managed_rpc_type<admire::adhoc_storage> m_storage;
    rpc_requirements_ptr<ADM_job_requirements_t, ADM_job_requirements_destroy>
            m_reqs;
};

// forward declarations
ADM_job_t
ADM_job_create(uint64_t id);
ADM_return_t
ADM_job_destroy(ADM_job_t job);

template <>
struct admire::managed_rpc_type<admire::job> {

    template <typename T, auto Deleter>
    using rpc_job_ptr = scord::utils::c_ptr<T, Deleter>;

    explicit managed_rpc_type(const admire::job& j)
        : m_job(ADM_job_create(j.id())) {}

    ADM_job_t
    get() const {
        return m_job.get();
    }

    rpc_job_ptr<ADM_job_t, ADM_job_destroy> m_job;
};

template <>
struct admire::managed_rpc_type<ADM_job_t> {

    template <typename T, auto Deleter>
    using rpc_job_ptr = scord::utils::c_ptr<T, Deleter>;

    explicit managed_rpc_type(ADM_job_t job) : m_job(job) {}

    admire::job
    get() const {
        return admire::job(m_job.get());
    }

    rpc_job_ptr<ADM_job_t, ADM_job_destroy> m_job;
};

template <>
struct admire::unmanaged_rpc_type<admire::job> {

    explicit unmanaged_rpc_type(const admire::job& j)
        : m_job(ADM_job_create(j.id())) {}

    ADM_job_t
    get() const {
        return m_job;
    }

    ADM_job_t m_job;
};


////////////////////////////////////////////////////////////////////////////////
//  Formatting functions
////////////////////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<admire::error_code> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::error_code& ec, FormatContext& ctx) const {
        std::string_view name = "unknown";

        switch(ec) {
            case ADM_SUCCESS:
                name = "ADM_SUCCESS";
                break;
            case ADM_ESNAFU:
                name = "ADM_ESNAFU";
                break;
            case ADM_EBADARGS:
                name = "ADM_EBADARGS";
                break;
            case ADM_ENOMEM:
                name = "ADM_ENOMEM";
                break;
            case ADM_EOTHER:
                name = "ADM_EOTHER";
                break;
            default:
                break;
        }

        return formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<admire::job> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::job& j, FormatContext& ctx) const {
        return formatter<std::string_view>::format(
                fmt::format("id: {}", j.id()), ctx);
    }
};

template <>
struct fmt::formatter<admire::dataset> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::dataset& d, FormatContext& ctx) const {
        return formatter<std::string_view>::format("\"" + d.id() + "\"", ctx);
    }
};

template <>
struct fmt::formatter<enum admire::storage::type>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const enum admire::storage::type& t, FormatContext& ctx) const {

        using admire::storage;
        std::string_view name = "unknown";

        switch(t) {
            case storage::type::gekkofs:
                name = "ADM_STORAGE_GEKKOFS";
                break;
            case storage::type::dataclay:
                name = "ADM_STORAGE_DATACLAY";
                break;
            case storage::type::expand:
                name = "ADM_STORAGE_EXPAND";
                break;
            case storage::type::hercules:
                name = "ADM_STORAGE_HERCULES";
                break;
            case storage::type::lustre:
                name = "ADM_STORAGE_LUSTRE";
                break;
            case storage::type::gpfs:
                name = "ADM_STORAGE_GPFS";
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

template <>
struct fmt::formatter<std::shared_ptr<admire::storage>>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::shared_ptr<admire::storage>& s,
           FormatContext& ctx) const {

        if(!s) {
            return formatter<std::string_view>::format("NULL", ctx);
        }

        switch(s->type()) {
            case admire::storage::type::gekkofs:
            case admire::storage::type::dataclay:
            case admire::storage::type::expand:
            case admire::storage::type::hercules:
                return formatter<std::string_view>::format(
                        fmt::format("{}",
                                    *(dynamic_cast<admire::adhoc_storage*>(
                                            s.get()))),
                        ctx);
            case admire::storage::type::lustre:
            case admire::storage::type::gpfs:
                return formatter<std::string_view>::format(
                        fmt::format("{}", *(dynamic_cast<admire::pfs_storage*>(
                                                  s.get()))),
                        ctx);
            default:
                return formatter<std::string_view>::format("unknown", ctx);
        }
    }
};

template <>
struct fmt::formatter<admire::adhoc_storage> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::adhoc_storage& s, FormatContext& ctx) const {

        const auto pctx = std::dynamic_pointer_cast<admire::adhoc_storage::ctx>(
                s.context());

        const auto str =
                fmt::format("{{type: {}, id: {}, context: {}}}", s.type(),
                            std::quoted(s.id()),
                            (pctx ? fmt::format("{}", *pctx) : "NULL"));
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
                            "nodes: {}, walltime: {}, should_flush: {}}}",
                            c.exec_mode(), c.access_type(), c.nodes(),
                            c.walltime(), c.should_flush());

        return formatter<std::string_view>::format(str, ctx);
    }
};


template <>
struct fmt::formatter<admire::pfs_storage> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::pfs_storage& s, FormatContext& ctx) const {

        const auto pctx = std::dynamic_pointer_cast<admire::pfs_storage::ctx>(
                s.context());
        const auto str = fmt::format(
                "{{context: {}}}", (pctx ? fmt::format("{}", *pctx) : "NULL"));
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
struct fmt::formatter<admire::job_requirements> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::job_requirements& r, FormatContext& ctx) const {
        return formatter<std::string_view>::format(
                fmt::format("inputs: [{}], outputs: [{}], storage: {}",
                            fmt::join(r.inputs(), ", "),
                            fmt::join(r.outputs(), ", "), r.storage()),
                ctx);
    }
};

#endif // SCORD_ADMIRE_TYPES_HPP
