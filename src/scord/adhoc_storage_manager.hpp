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


#ifndef SCORD_ADHOC_STORAGE_MANAGER_HPP
#define SCORD_ADHOC_STORAGE_MANAGER_HPP

#include <admire_types.hpp>
#include <internal_types.hpp>
#include <utils/utils.hpp>
#include <unordered_map>
#include <abt_cxx/shared_mutex.hpp>
#include <tl/expected.hpp>
#include <atomic>
#include <logger/logger.hpp>

namespace scord {

struct adhoc_storage_manager : scord::utils::singleton<adhoc_storage_manager> {

    tl::expected<std::shared_ptr<admire::adhoc_storage>, admire::error_code>
    create(enum admire::adhoc_storage::type type, const std::string& name,
           const admire::adhoc_storage::ctx& ctx) {

        static std::atomic_uint64_t current_id;
        std::uint64_t id = current_id++;

        abt::unique_lock lock(m_adhoc_storages_mutex);

        if(const auto it = m_adhoc_storages.find(id);
           it == m_adhoc_storages.end()) {
            const auto& [it_adhoc, inserted] = m_adhoc_storages.emplace(
                    id, std::make_shared<admire::adhoc_storage>(
                                type, name, current_id++, ctx));

            if(!inserted) {
                LOGGER_ERROR("{}: Emplace failed", __FUNCTION__);
                return tl::make_unexpected(ADM_ESNAFU);
            }

            return it_adhoc->second;
        }

        LOGGER_ERROR("{}: Adhoc storage '{}' already exists", __FUNCTION__, id);
        return tl::make_unexpected(ADM_EEXISTS);
    }

    admire::error_code
    update(std::uint64_t id, admire::adhoc_storage::ctx ctx) {

        abt::unique_lock lock(m_adhoc_storages_mutex);

        if(const auto it = m_adhoc_storages.find(id);
           it != m_adhoc_storages.end()) {

            const auto& current_adhoc_ptr = it->second;

            const auto new_adhoc = admire::adhoc_storage{
                    current_adhoc_ptr->type(), current_adhoc_ptr->name(),
                    current_adhoc_ptr->id(), std::move(ctx)};
            *it->second = new_adhoc;
            return ADM_SUCCESS;
        }

        LOGGER_ERROR("{}: Adhoc storage '{}' does not exist", __FUNCTION__, id);
        return ADM_ENOENT;
    }

    tl::expected<std::shared_ptr<admire::adhoc_storage>, admire::error_code>
    find(std::uint64_t id) {

        abt::shared_lock lock(m_adhoc_storages_mutex);

        if(auto it = m_adhoc_storages.find(id); it != m_adhoc_storages.end()) {
            return it->second;
        }

        LOGGER_ERROR("Adhoc storage '{}' was not registered or was already "
                     "deleted",
                     id);
        return tl::make_unexpected(ADM_ENOENT);
    }

    admire::error_code
    remove(std::uint64_t id) {

        abt::unique_lock lock(m_adhoc_storages_mutex);

        if(m_adhoc_storages.count(id) != 0) {
            m_adhoc_storages.erase(id);
            return ADM_SUCCESS;
        }

        LOGGER_ERROR("Adhoc storage '{}' was not registered or was already "
                     "deleted",
                     id);

        return ADM_ENOENT;
    }

private:
    friend class scord::utils::singleton<adhoc_storage_manager>;
    adhoc_storage_manager() = default;

    mutable abt::shared_mutex m_adhoc_storages_mutex;
    std::unordered_map<std::uint64_t, std::shared_ptr<admire::adhoc_storage>>
            m_adhoc_storages;
};

} // namespace scord

#endif // SCORD_ADHOC_STORAGE_MANAGER_HPP
