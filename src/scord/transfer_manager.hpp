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

struct transfer_manager {

    tl::expected<std::shared_ptr<scord::internal::transfer_info>, scord::error_code>
    create(scord::slurm_job_id slurm_id, scord::transfer_id tx_id, float qos) {

        abt::unique_lock lock(m_transfer_mutex);

        if(const auto it = m_transfer.find(id); it == m_transfer.end()) {
            const auto& [it_transfer, inserted] = m_transfer.emplace(
                    id,
                    std::make_shared<scord::internal::transfer_info>(
                            scord::transfer{id, slurm_id}, client_info, qos, ""));
   explicit transfer_info(scord::transfer transfer,
                           scord::internal::job_info client_info, float qos,
                           std::string contact_point);
            if(!inserted) {
                LOGGER_ERROR("{}: Emplace failed", __FUNCTION__);
                return tl::make_unexpected(scord::error_code::snafu);
            }

            return it_tranfer->second;
        }

        LOGGER_ERROR("{}: Job '{}' already exists", __FUNCTION__, id);
        return tl::make_unexpected(scord::error_code::entity_exists);
    }

    scord::error_code
    update(scord::job_id id, scord::job::resources job_resources) {

        abt::unique_lock lock(m_jobs_mutex);

        if(const auto it = m_jobs.find(id); it != m_jobs.end()) {
            const auto& current_job_info = it->second;
            current_job_info->update(std::move(job_resources));
            return scord::error_code::success;
        }

        LOGGER_ERROR("{}: Job '{}' does not exist", __FUNCTION__, id);
        return scord::error_code::no_such_entity;
    }

    tl::expected<std::shared_ptr<scord::internal::job_info>, scord::error_code>
    find(scord::job_id id) {

        abt::shared_lock lock(m_jobs_mutex);

        if(auto it = m_jobs.find(id); it != m_jobs.end()) {
            return it->second;
        }

        LOGGER_ERROR("Job '{}' was not registered or was already deleted", id);
        return tl::make_unexpected(scord::error_code::no_such_entity);
    }

    tl::expected<std::shared_ptr<scord::internal::job_info>, scord::error_code>
    remove(scord::job_id id) {

        abt::unique_lock lock(m_jobs_mutex);

        if(const auto it = m_jobs.find(id); it != m_jobs.end()) {
            auto nh = m_jobs.extract(it);
            return nh.mapped();
        }

        LOGGER_ERROR("Job '{}' was not registered or was already deleted", id);

        return tl::make_unexpected(scord::error_code::no_such_entity);
    }

private:
    mutable abt::shared_mutex m_cargo_mutex;
    std::unordered_map<scord::job_id,
                       std::shared_ptr<scord::internal::job_info>>
            m_jobs;
};

} // namespace scord


#endif // SCORD_JOB_MANAGER_HPP
