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

#include <scord/types.hpp>
#include <utility>
#include <unordered_map>
#include <abt_cxx/shared_mutex.hpp>
#include <tl/expected.hpp>
#include <atomic>
#include <random>
#include <logger/logger.hpp>
#include "internal_types.hpp"

namespace {

[[nodiscard]] std::string
generate_adhoc_uuid(enum scord::adhoc_storage::type adhoc_type) {

    using namespace std::literals;

    /**
     * @brief Generate a random string of the given length.
     *
     * @param length The length of the string to generate.
     *
     * @return A random string of the given length.
     */
    const auto generate_random_string = [](int length = 32) -> std::string {
        constexpr auto dice = []() {
            constexpr auto chars =
                    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"sv;
            thread_local std::mt19937 generator{std::random_device{}()};
            std::uniform_int_distribution<> distribution(0, chars.length() - 1);
            return chars.at(distribution(generator));
        };

        std::string result;
        result.reserve(length);
        std::ranges::generate_n(std::back_inserter(result), length, dice);
        return result;
    };

    return fmt::format("{:e}-{}", adhoc_type, generate_random_string());
}

} // namespace

namespace scord {

struct adhoc_storage_manager {

    tl::expected<std::shared_ptr<scord::internal::adhoc_storage_metadata>,
                 scord::error_code>
    create(enum scord::adhoc_storage::type type, const std::string& name,
           const scord::adhoc_storage::ctx& ctx,
           const scord::adhoc_storage::resources& resources) {

        static std::atomic_uint64_t current_id;
        std::uint64_t id = current_id++;

        abt::unique_lock lock(m_adhoc_storages_mutex);

        if(const auto it = m_adhoc_storages.find(id);
           it == m_adhoc_storages.end()) {
            const auto& [it_adhoc, inserted] = m_adhoc_storages.emplace(
                    id,
                    std::make_shared<scord::internal::adhoc_storage_metadata>(
                            ::generate_adhoc_uuid(type),
                            scord::adhoc_storage{type, name, id, ctx,
                                                 resources}));

            if(!inserted) {
                LOGGER_ERROR("{}: Emplace failed", __FUNCTION__);
                return tl::make_unexpected(scord::error_code::snafu);
            }

            return it_adhoc->second;
        }

        LOGGER_ERROR("{}: Adhoc storage '{}' already exists", __FUNCTION__, id);
        return tl::make_unexpected(scord::error_code::entity_exists);
    }

    scord::error_code
    update(std::uint64_t id, scord::adhoc_storage::resources new_resources) {

        abt::unique_lock lock(m_adhoc_storages_mutex);

        if(const auto it = m_adhoc_storages.find(id);
           it != m_adhoc_storages.end()) {
            const auto adhoc_metadata_ptr = it->second;
            adhoc_metadata_ptr->update(std::move(new_resources));
            return scord::error_code::success;
        }

        LOGGER_ERROR("{}: Adhoc storage '{}' does not exist", __FUNCTION__, id);
        return scord::error_code::no_such_entity;
    }

    tl::expected<std::shared_ptr<scord::internal::adhoc_storage_metadata>,
                 scord::error_code>
    find(std::uint64_t id) {

        abt::shared_lock lock(m_adhoc_storages_mutex);

        if(auto it = m_adhoc_storages.find(id); it != m_adhoc_storages.end()) {
            return it->second;
        }

        LOGGER_ERROR("Adhoc storage '{}' was not registered or was already "
                     "deleted",
                     id);
        return tl::make_unexpected(scord::error_code::no_such_entity);
    }

    scord::error_code
    remove(std::uint64_t id) {

        abt::unique_lock lock(m_adhoc_storages_mutex);

        if(m_adhoc_storages.count(id) != 0) {
            m_adhoc_storages.erase(id);
            return scord::error_code::success;
        }

        LOGGER_ERROR("Adhoc storage '{}' was not registered or was already "
                     "deleted",
                     id);

        return scord::error_code::no_such_entity;
    }

    scord::error_code
    add_client_info(
            std::uint64_t adhoc_id,
            std::shared_ptr<scord::internal::job_metadata> job_metadata_ptr) {

        if(auto am_result = find(adhoc_id); am_result.has_value()) {
            const auto adhoc_metadata_ptr = am_result.value();
            return adhoc_metadata_ptr->add_client_info(
                    std::move(job_metadata_ptr));
        }

        return scord::error_code::no_such_entity;
    }

    scord::error_code
    remove_client_info(std::uint64_t adhoc_id) {
        if(auto am_result = find(adhoc_id); am_result.has_value()) {
            const auto adhoc_metadata_ptr = *am_result;
            adhoc_metadata_ptr->remove_client_info();
            return scord::error_code::success;
        }

        return scord::error_code::no_such_entity;
    }


private:
    mutable abt::shared_mutex m_adhoc_storages_mutex;
    std::unordered_map<std::uint64_t,
                       std::shared_ptr<scord::internal::adhoc_storage_metadata>>
            m_adhoc_storages;
};

} // namespace scord

#endif // SCORD_ADHOC_STORAGE_MANAGER_HPP
