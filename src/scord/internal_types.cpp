/******************************************************************************
 * Copyright 2021-2023, Barcelona Supercomputing Center (BSC), Spain
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

#include <logger/logger.hpp>
#include <abt_cxx/shared_mutex.hpp>
#include "internal_types.hpp"


namespace scord::internal {

job_info::job_info(scord::job job, scord::job::resources resources,
                   scord::job::requirements requirements)
    : m_job(std::move(job)), m_resources(std::move(resources)),
      m_requirements(std::move(requirements)) {}

scord::job
job_info::job() const {
    return m_job;
}

std::optional<scord::job::resources>
job_info::resources() const {
    return m_resources;
}

void
job_info::update(scord::job::resources resources) {
    m_resources = std::move(resources);
}

adhoc_storage_info::adhoc_storage_info(scord::adhoc_storage adhoc_storage)
    : m_adhoc_storage(std::move(adhoc_storage)) {}

scord::adhoc_storage
adhoc_storage_info::adhoc_storage() const {
    return m_adhoc_storage;
}

void
adhoc_storage_info::update(scord::adhoc_storage::resources new_resources) {
    m_adhoc_storage.update(std::move(new_resources));
}

scord::error_code
adhoc_storage_info::add_client_info(
        std::shared_ptr<scord::internal::job_info> job_info) {

    scord::abt::unique_lock lock(m_info_mutex);

    if(m_client_info) {
        LOGGER_ERROR("adhoc storage {} already has a client",
                     m_adhoc_storage.id());
        return error_code::adhoc_in_use;
    }

    m_client_info = std::move(job_info);

    return error_code::success;
}

void
adhoc_storage_info::remove_client_info() {
    scord::abt::unique_lock lock(m_info_mutex);
    m_client_info.reset();
}

std::shared_ptr<scord::internal::job_info>
adhoc_storage_info::client_info() const {
    scord::abt::shared_lock lock(m_info_mutex);
    return m_client_info;
}

pfs_storage_info::pfs_storage_info(scord::pfs_storage pfs_storage)
    : m_pfs_storage(std::move(pfs_storage)) {}

scord::pfs_storage
pfs_storage_info::pfs_storage() const {
    return m_pfs_storage;
}

void
pfs_storage_info::update(scord::pfs_storage::ctx pfs_context) {
    m_pfs_storage.update(std::move(pfs_context));
}

} // namespace scord::internal
