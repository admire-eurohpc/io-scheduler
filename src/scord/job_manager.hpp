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

#include <admire_types.hpp>
#include <atomic>
#include <utility>
#include <utils/utils.hpp>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <tl/expected.hpp>
#include <logger/logger.hpp>
#include <abt_cxx/shared_mutex.hpp>

namespace scord {

struct job_info {
    explicit job_info(admire::job job) : m_job(std::move(job)) {}
    job_info(admire::job job, admire::job::resources resources,
             admire::job_requirements requirements)
        : m_job(std::move(job)), m_resources(std::move(resources)),
          m_requirements(std::move(requirements)) {}

    admire::job
    job() const {
        return m_job;
    }

    std::optional<admire::job::resources>
    resources() const {
        return m_resources;
    }

    std::optional<admire::job_requirements>
    requirements() const {
        return m_requirements;
    }

    admire::job m_job;
    std::optional<admire::job::resources> m_resources;
    std::optional<admire::job_requirements> m_requirements;
};

struct job_manager : scord::utils::singleton<job_manager> {


    tl::expected<job_info, admire::error_code>
    create(admire::job::resources job_resources,
           admire::job_requirements job_requirements) {

        abt::unique_lock lock(m_jobs_mutex);

        static std::atomic_uint64_t current_id;
        admire::job_id id = current_id++;

        if(const auto it = m_jobs.find(id); it == m_jobs.end()) {
            const auto& [it_job, inserted] = m_jobs.emplace(
                    id, job_info{admire::job{id}, std::move(job_resources),
                                 std::move(job_requirements)});

            if(!inserted) {
                LOGGER_ERROR("{}: Emplace failed", __FUNCTION__);
                return tl::make_unexpected(ADM_ESNAFU);
            }

            return it_job->second;
        }

        LOGGER_ERROR("{}: Job '{}' already exists", __FUNCTION__, id);
        return tl::make_unexpected(ADM_EEXISTS);
    }

    admire::error_code
    update(admire::job_id id, admire::job::resources job_resources,
           admire::job_requirements job_requirements) {

        abt::unique_lock lock(m_jobs_mutex);

        if(const auto it = m_jobs.find(id); it != m_jobs.end()) {
            it->second = job_info{admire::job{id}, std::move(job_resources),
                                  std::move(job_requirements)};
            return ADM_SUCCESS;
        }

        LOGGER_ERROR("{}: Job '{}' does not exist", __FUNCTION__, id);
        return ADM_ENOENT;
    }

    tl::expected<job_info, admire::error_code>
    find(admire::job_id id) {

        abt::shared_lock lock(m_jobs_mutex);

        if(auto it = m_jobs.find(id); it != m_jobs.end()) {
            return it->second;
        }

        LOGGER_ERROR("Job '{}' was not registered or was already deleted", id);
        return tl::make_unexpected(ADM_ENOENT);
    }

    admire::error_code
    remove(admire::job_id id) {

        abt::unique_lock lock(m_jobs_mutex);

        if(m_jobs.count(id) != 0) {
            m_jobs.erase(id);
            return ADM_SUCCESS;
        }

        LOGGER_ERROR("Job '{}' was not registered or was already deleted", id);

        return ADM_ENOENT;
    }

private:
    friend class scord::utils::singleton<job_manager>;
    job_manager() = default;

    mutable std::shared_mutex m_jobs_mutex;
    std::unordered_map<admire::job_id, job_info> m_jobs;
};

} // namespace scord


#endif // SCORD_JOB_MANAGER_HPP
