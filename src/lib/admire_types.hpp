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

#endif // SCORD_ADMIRE_TYPES_HPP
