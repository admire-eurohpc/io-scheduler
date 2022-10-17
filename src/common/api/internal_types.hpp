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

namespace admire::internal {

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

} // namespace admire::internal

#endif // SCORD_INTERNAL_TYPES_HPP
