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
#include "admire_types.h"

namespace admire {

using job_id = std::int64_t;

struct server {

    server(std::string protocol, std::string address);

    explicit server(const ADM_server_t& srv);

    std::string m_protocol;
    std::string m_address;
};

struct job {

    explicit job(job_id id);

    explicit job(ADM_job_t job);

    [[nodiscard]] ADM_job_t
    to_ctype() const;

    job_id m_id;
};

struct dataset {
    explicit dataset(std::string id) : m_id(std::move(id)) {}

    std::string
    to_string() const {
        return "foo";
    }

    std::string m_id;
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

    storage(storage::type type, std::string id);

    virtual ~storage() = default;

    virtual ADM_storage_t
    to_rpc_type() const = 0;

    std::string m_id;
    type m_type;
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

    struct context {

        context(execution_mode exec_mode, access_type access_type,
                std::uint32_t nodes, std::uint32_t walltime, bool should_flush);

        explicit context(ADM_adhoc_context_t ctx);

        ADM_adhoc_context_t
        to_rpc_type() const;

        execution_mode m_exec_mode;
        access_type m_access_type;
        std::uint32_t m_nodes;
        std::uint32_t m_walltime;
        bool m_should_flush;
    };

    adhoc_storage(storage::type type, std::string id, execution_mode exec_mode,
                  access_type access_type, std::uint32_t nodes,
                  std::uint32_t walltime, bool should_flush);

    adhoc_storage(storage::type type, std::string id, ADM_adhoc_context_t ctx);


    ADM_storage_t
    to_rpc_type() const final;

    context m_ctx;
};

struct pfs_storage : public storage {

    struct context {

        explicit context(std::filesystem::path mount_point);

        explicit context(ADM_pfs_context_t ctx);

        ADM_pfs_context_t
        to_rpc_type() const;

        std::filesystem::path m_mount_point;
    };

    pfs_storage(storage::type type, std::string id,
                std::filesystem::path mount_point);

    pfs_storage(storage::type type, std::string id, ADM_pfs_context_t ctx);

    ADM_storage_t
    to_rpc_type() const final;

    context m_ctx;
};

struct job_requirements {

    job_requirements(std::vector<admire::dataset> inputs,
                     std::vector<admire::dataset> outputs);

    job_requirements(std::vector<admire::dataset> inputs,
                     std::vector<admire::dataset> outputs,
                     std::unique_ptr<storage> storage);

    explicit job_requirements(ADM_job_requirements_t reqs);

    ADM_job_requirements_t
    to_rpc_type() const;

    std::vector<admire::dataset> m_inputs;
    std::vector<admire::dataset> m_outputs;
    std::unique_ptr<admire::storage> m_storage;
};

} // namespace admire


////////////////////////////////////////////////////////////////////////////////
//  Formatting functions
////////////////////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<admire::dataset> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::dataset& d, FormatContext& ctx) const {
        return formatter<std::string_view>::format("\"" + d.m_id + "\"", ctx);
    }
};

template <>
struct fmt::formatter<admire::storage::type> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::storage::type& t, FormatContext& ctx) const {

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
struct fmt::formatter<std::unique_ptr<admire::storage>>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const std::unique_ptr<admire::storage>& s,
           FormatContext& ctx) const {

        switch(s->m_type) {
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
        const auto str = fmt::format("{{type: {}, id: {}, context: {}}}",
                                     s.m_type, std::quoted(s.m_id), s.m_ctx);
        return formatter<std::string_view>::format(str, ctx);
    }
};


template <>
struct fmt::formatter<admire::adhoc_storage::context>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::adhoc_storage::context& c, FormatContext& ctx) const {

        const auto str =
                fmt::format("{{execution_mode: {}, access_type: {}, "
                            "nodes: {}, walltime: {}, should_flush: {}}}",
                            c.m_exec_mode, c.m_access_type, c.m_nodes,
                            c.m_walltime, c.m_should_flush);

        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<admire::pfs_storage> : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::pfs_storage& s, FormatContext& ctx) const {
        const auto str = fmt::format("{{m_ctx: {}}}", s.m_ctx);
        return formatter<std::string_view>::format(str, ctx);
    }
};

template <>
struct fmt::formatter<admire::pfs_storage::context>
    : formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto
    format(const admire::pfs_storage::context& c, FormatContext& ctx) const {
        const auto str = fmt::format("{{mount_point: {}}}", c.m_mount_point);
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
                            fmt::join(r.m_inputs, ", "),
                            fmt::join(r.m_outputs, ", "),
                            (r.m_storage ? fmt::format("{}", r.m_storage)
                                         : "NULL")),
                ctx);
    }
};

#endif // SCORD_ADMIRE_TYPES_HPP
