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

#ifndef SCORD_PFS_STORAGE_MANAGER_HPP
#define SCORD_PFS_STORAGE_MANAGER_HPP

#include <scord/types.hpp>
#include <scord/internal_types.hpp>
#include <utility>
#include <unordered_map>
#include <abt_cxx/shared_mutex.hpp>
#include <tl/expected.hpp>
#include <atomic>
#include <logger/logger.hpp>

namespace scord {

struct pfs_storage_manager {

    tl::expected<std::shared_ptr<scord::internal::pfs_storage_metadata>,
                 scord::error_code>
    create(enum scord::pfs_storage::type type, const std::string& name,
           const scord::pfs_storage::ctx& ctx) {

        static std::atomic_uint64_t current_id;
        std::uint64_t id = current_id++;

        abt::unique_lock lock(m_pfs_storages_mutex);

        if(const auto it = m_pfs_storages.find(id);
           it == m_pfs_storages.end()) {
            const auto& [it_pfs, inserted] = m_pfs_storages.emplace(
                    id, std::make_shared<scord::internal::pfs_storage_metadata>(
                                scord::pfs_storage{type, name, id, ctx}));

            if(!inserted) {
                LOGGER_ERROR("{}: Emplace failed", __FUNCTION__);
                return tl::make_unexpected(scord::error_code::snafu);
            }

            return it_pfs->second;
        }

        LOGGER_ERROR("{}: PFS storage '{}' already exists", __FUNCTION__, id);
        return tl::make_unexpected(scord::error_code::entity_exists);
    }

    scord::error_code
    update(std::uint64_t id, scord::pfs_storage::ctx new_ctx) {

        abt::unique_lock lock(m_pfs_storages_mutex);

        if(const auto it = m_pfs_storages.find(id);
           it != m_pfs_storages.end()) {
            const auto pfs_metadata_ptr = it->second;
            pfs_metadata_ptr->update(std::move(new_ctx));
            return scord::error_code::success;
        }

        LOGGER_ERROR("{}: Adhoc storage '{}' does not exist", __FUNCTION__, id);
        return scord::error_code::no_such_entity;
    }

    tl::expected<std::shared_ptr<scord::internal::pfs_storage_metadata>,
                 scord::error_code>
    find(std::uint64_t id) {

        abt::shared_lock lock(m_pfs_storages_mutex);

        if(auto it = m_pfs_storages.find(id); it != m_pfs_storages.end()) {
            return it->second;
        }

        LOGGER_ERROR("PFS storage '{}' was not registered or was already "
                     "deleted",
                     id);
        return tl::make_unexpected(scord::error_code::no_such_entity);
    }

    scord::error_code
    remove(std::uint64_t id) {

        abt::unique_lock lock(m_pfs_storages_mutex);

        if(m_pfs_storages.count(id) != 0) {
            m_pfs_storages.erase(id);
            return scord::error_code::success;
        }

        LOGGER_ERROR("PFS storage '{}' was not registered or was already "
                     "deleted",
                     id);

        return scord::error_code::no_such_entity;
    }

private:
    mutable abt::shared_mutex m_pfs_storages_mutex;
    std::unordered_map<std::uint64_t,
                       std::shared_ptr<scord::internal::pfs_storage_metadata>>
            m_pfs_storages;
};

} // namespace scord

#endif // SCORD_PFS_STORAGE_MANAGER_HPP
