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


#ifndef SCORD_ADMIRE_IMPL_HPP
#define SCORD_ADMIRE_IMPL_HPP

#include <admire.hpp>
#include <tl/expected.hpp>

namespace admire::detail {

admire::error_code
ping(const server& srv);

tl::expected<admire::job, admire::error_code>
register_job(const server& srv, const job::resources& job_resources,
             const job_requirements& reqs, admire::slurm_job_id slurm_id);

admire::error_code
update_job(const server& srv, const job& job,
           const job::resources& job_resources);

admire::error_code
remove_job(const server& srv, const job& job);

tl::expected<transfer, error_code>
transfer_datasets(const server& srv, const job& job,
                  const std::vector<dataset>& sources,
                  const std::vector<dataset>& targets,
                  const std::vector<qos::limit>& limits,
                  transfer::mapping mapping);

tl::expected<admire::adhoc_storage, admire::error_code>
register_adhoc_storage(const server& srv, const std::string& name,
                       enum adhoc_storage::type type,
                       const adhoc_storage::ctx& ctx);

admire::error_code
update_adhoc_storage(const server& srv,
                     const adhoc_storage::ctx& adhoc_storage_ctx,
                     const adhoc_storage& adhoc_storage);

admire::error_code
remove_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage);

} // namespace admire::detail

#endif // SCORD_ADMIRE_IMPL_HPP
