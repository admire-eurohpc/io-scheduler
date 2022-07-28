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

    [[nodiscard]] ADM_job_t
    to_ctype() const;

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

} // namespace admire

#endif // SCORD_ADMIRE_TYPES_HPP
