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
#include <utility>
#include <utils/utils.hpp>
#include <unordered_map>
#include <abt_cxx/shared_mutex.hpp>
#include <tl/expected.hpp>
#include <atomic>
#include <logger/logger.hpp>

namespace scord {

struct adhoc_storage_manager : scord::utils::singleton<adhoc_storage_manager> {

    tl::expected<std::shared_ptr<admire::internal::adhoc_storage_info>,
                 admire::error_code>
    create(enum admire::adhoc_storage::type type, const std::string& name,
           const admire::adhoc_storage::ctx& ctx) {

        static std::atomic_uint64_t current_id;
        std::uint64_t id = current_id++;

        abt::unique_lock lock(m_adhoc_storages_mutex);

        if(const auto it = m_adhoc_storages.find(id);
           it == m_adhoc_storages.end()) {
            const auto& [it_adhoc, inserted] = m_adhoc_storages.emplace(
                    id, std::make_shared<admire::internal::adhoc_storage_info>(
                                admire::adhoc_storage{type, name, id, ctx}));

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
    update(std::uint64_t id, admire::adhoc_storage::ctx new_ctx) {

        abt::unique_lock lock(m_adhoc_storages_mutex);

        if(const auto it = m_adhoc_storages.find(id);
           it != m_adhoc_storages.end()) {
            const auto current_adhoc_info = it->second;
            current_adhoc_info->update(std::move(new_ctx));
            return ADM_SUCCESS;
        }

        LOGGER_ERROR("{}: Adhoc storage '{}' does not exist", __FUNCTION__, id);
        return ADM_ENOENT;
    }

    tl::expected<std::shared_ptr<admire::internal::adhoc_storage_info>,
                 admire::error_code>
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

    admire::error_code
    add_client_info(std::uint64_t adhoc_id,
                    std::shared_ptr<admire::internal::job_info> job_info) {

        if(auto am_result = find(adhoc_id); am_result.has_value()) {
            const auto adhoc_storage_info = am_result.value();
            return adhoc_storage_info->add_client_info(std::move(job_info));
        }

        return ADM_ENOENT;
    }

    admire::error_code
    remove_client_info(std::uint64_t adhoc_id) {
        if(auto am_result = find(adhoc_id); am_result.has_value()) {
            const auto adhoc_storage_info = *am_result;
            adhoc_storage_info->remove_client_info();
            return ADM_SUCCESS;
        }

        return ADM_ENOENT;
    }


private:
    friend class scord::utils::singleton<adhoc_storage_manager>;
    adhoc_storage_manager() = default;

    mutable abt::shared_mutex m_adhoc_storages_mutex;
    std::unordered_map<std::uint64_t,
                       std::shared_ptr<admire::internal::adhoc_storage_info>>
            m_adhoc_storages;
};

} // namespace scord

#endif // SCORD_ADHOC_STORAGE_MANAGER_HPP