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

#ifndef SCORD_JOB_MANAGER_HPP
#define SCORD_JOB_MANAGER_HPP

#include <scord/types.hpp>
#include <atomic>
#include <utility>
#include <unordered_map>
#include <tl/expected.hpp>
#include <logger/logger.hpp>
#include <abt_cxx/shared_mutex.hpp>
#include "internal_types.hpp"

namespace scord {

struct job_manager {

    tl::expected<std::shared_ptr<scord::internal::job_metadata>,
                 scord::error_code>
    create(scord::slurm_job_id slurm_id, scord::job::resources job_resources,
           scord::job::requirements job_requirements,
           std::shared_ptr<scord::internal::adhoc_storage_metadata>
                   adhoc_metadata_ptr) {

        static std::atomic_uint64_t current_id;
        scord::job_id id = current_id++;

        abt::unique_lock lock(m_jobs_mutex);

        if(const auto it = m_jobs.find(id); it == m_jobs.end()) {
            const auto& [it_job, inserted] = m_jobs.emplace(
                    id,
                    std::make_shared<scord::internal::job_metadata>(
                            scord::job{id, slurm_id}, std::move(job_resources),
                            std::move(job_requirements),
                            std::move(adhoc_metadata_ptr)));

            m_slurm_to_scord.emplace(slurm_id, id);

            if(!inserted) {
                LOGGER_ERROR("{}: Emplace failed", __FUNCTION__);
                return tl::make_unexpected(scord::error_code::snafu);
            }

            return it_job->second;
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

    tl::expected<std::shared_ptr<scord::internal::job_metadata>,
                 scord::error_code>
    find(scord::job_id id) {

        abt::shared_lock lock(m_jobs_mutex);

        if(auto it = m_jobs.find(id); it != m_jobs.end()) {
            return it->second;
        }

        LOGGER_ERROR("Job '{}' was not registered or was already deleted", id);
        return tl::make_unexpected(scord::error_code::no_such_entity);
    }

    tl::expected<std::shared_ptr<scord::internal::job_metadata>,
                 scord::error_code>
    find_by_slurm_id(scord::slurm_job_id slurm_id) {

        abt::shared_lock lock(m_jobs_mutex);

        if(auto it = m_slurm_to_scord.find(slurm_id);
           it != m_slurm_to_scord.end()) {
            return find(it->second);
        }

        LOGGER_ERROR("Slurm job '{}' was not registered or was already deleted",
                     slurm_id);
        return tl::make_unexpected(scord::error_code::no_such_entity);
    }

    tl::expected<std::shared_ptr<scord::internal::job_metadata>,
                 scord::error_code>
    remove(scord::job_id id) {

        abt::unique_lock lock(m_jobs_mutex);

        if(const auto it = m_jobs.find(id); it != m_jobs.end()) {
            auto nh = m_jobs.extract(it);
            m_slurm_to_scord.erase(nh.mapped()->job().slurm_id());
            return nh.mapped();
        }

        LOGGER_ERROR("Job '{}' was not registered or was already deleted", id);

        return tl::make_unexpected(scord::error_code::no_such_entity);
    }

private:
    mutable abt::shared_mutex m_jobs_mutex;
    std::unordered_map<scord::job_id,
                       std::shared_ptr<scord::internal::job_metadata>>
            m_jobs;
    std::unordered_map<scord::slurm_job_id, scord::job_id> m_slurm_to_scord;
};

} // namespace scord


#endif // SCORD_JOB_MANAGER_HPP
