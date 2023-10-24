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


#ifndef SCORD_INTERNAL_TYPES_HPP
#define SCORD_INTERNAL_TYPES_HPP

#include <optional>
#include <logger/logger.hpp>
#include <scord/types.hpp>

namespace scord::internal {

struct job_metadata {

    job_metadata(scord::job job, scord::job::resources resources,
                 scord::job::requirements requirements);

    scord::job
    job() const;

    std::optional<scord::job::resources>
    resources() const;

    std::uint32_t
    io_procs() const;

    std::optional<scord::job::requirements>
    requirements() const {
        return m_requirements;
    }

    void
    update(scord::job::resources resources);

    scord::job m_job;
    std::optional<scord::job::resources> m_resources;
    std::optional<scord::job::requirements> m_requirements;
};

struct adhoc_storage_metadata {

    adhoc_storage_metadata(std::string uuid,
                           scord::adhoc_storage adhoc_storage);

    scord::adhoc_storage const&
    adhoc_storage() const;

    std::string const&
    uuid() const;

    void
    update(scord::adhoc_storage::resources new_resources);

    scord::error_code
    add_client_info(std::shared_ptr<scord::internal::job_metadata> job_info);

    void
    remove_client_info();

    std::shared_ptr<scord::internal::job_metadata>
    client_info() const;

    std::string m_uuid;
    scord::adhoc_storage m_adhoc_storage;
    std::shared_ptr<scord::internal::job_metadata> m_client_info;
    mutable scord::abt::shared_mutex m_info_mutex;
};

struct pfs_storage_metadata {

    explicit pfs_storage_metadata(scord::pfs_storage pfs_storage);

    scord::pfs_storage
    pfs_storage() const;

    void
    update(scord::pfs_storage::ctx pfs_context);

    scord::pfs_storage m_pfs_storage;
    std::shared_ptr<scord::internal::job_metadata> m_client_info;
};

} // namespace scord::internal

#endif // SCORD_INTERNAL_TYPES_HPP
