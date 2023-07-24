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

#ifndef SCORD_TRANSFER_MANAGER_HPP
#define SCORD_TRANSFER_MANAGER_HPP

#include <scord/types.hpp>
#include <atomic>
#include <utility>
#include <unordered_map>
#include <tl/expected.hpp>
#include <logger/logger.hpp>
#include <abt_cxx/shared_mutex.hpp>
#include "internal_types.hpp"

namespace scord {

template <typename TransferHandle>
struct transfer_manager {

    tl::expected<
            std::shared_ptr<scord::internal::transfer_metadata<TransferHandle>>,
            scord::error_code>
    create(TransferHandle tx, std::vector<scord::qos::limit> limits) {

        static std::atomic_uint64_t current_id;
        scord::transfer_id id = current_id++;

        abt::unique_lock lock(m_transfer_mutex);

        if(const auto it = m_transfer.find(tx_id); it == m_transfer.end()) {
            const auto& [it_transfer, inserted] = m_transfer.emplace(
                    id, std::make_shared<
                                internal::transfer_metadata<TransferHandle>>(
                                id, std::move(tx), std::move(limits)));

            if(!inserted) {
                LOGGER_ERROR("{}: Emplace failed", __FUNCTION__);
                return tl::make_unexpected(scord::error_code::snafu);
            }

            return it_transfer->second;
        }

        LOGGER_ERROR("{}: Transfer '{}' already exists", __FUNCTION__, id);
        return tl::make_unexpected(scord::error_code::entity_exists);
    }

    scord::error_code
    update(scord::transfer_id id, float obtained_bw) {

        abt::unique_lock lock(m_transfer_mutex);

        if(const auto it = m_transfer.find(id); it != m_transfer.end()) {
            const auto& current_transfer_info = it->second;
            current_transfer_info->update(obtained_bw);
            return scord::error_code::success;
        }

        LOGGER_ERROR("{}: Transfer '{}' does not exist", __FUNCTION__, id);
        return scord::error_code::no_such_entity;
    }

    tl::expected<
            std::shared_ptr<scord::internal::transfer_metadata<TransferHandle>>,
            scord::error_code>
    find(scord::transfer_id id) {

        abt::shared_lock lock(m_transfer_mutex);

        if(auto it = m_transfer.find(id); it != m_transfer.end()) {
            return it->second;
        }

        LOGGER_ERROR("Transfer '{}' was not registered or was already deleted",
                     id);
        return tl::make_unexpected(scord::error_code::no_such_entity);
    }

    tl::expected<
            std::shared_ptr<scord::internal::transfer_metadata<TransferHandle>>,
            scord::error_code>
    remove(scord::transfer_id id) {

        abt::unique_lock lock(m_transfer_mutex);

        if(m_transfer.count(id) != 0) {
            m_transfer.erase(id);
            return scord::error_code::success;
        }


        LOGGER_ERROR("Transfer '{}' was not registered or was already deleted",
                     id);

        return scord::error_code::no_such_entity;
    }

    std::unordered_map<scord::transfer_id,
                       std::shared_ptr<scord::internal::transfer_info>>
    transfer() {
        return m_transfer;
    }

    void
    lock() {
        m_transfer_mutex.lock();
    }

    void
    unlock() {
        m_transfer_mutex.unlock();
    }

private:
    mutable abt::shared_mutex m_transfer_mutex;
    std::unordered_map<
            scord::transfer_id,
            std::shared_ptr<scord::internal::transfer_metadata<TransferHandle>>>
            m_transfer;
};

} // namespace scord

#endif // SCORD_TRANSFER_MANAGER_HPP
