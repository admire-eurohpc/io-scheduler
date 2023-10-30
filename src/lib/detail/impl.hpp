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

#include <scord/scord.hpp>
#include <tl/expected.hpp>

namespace scord::detail {

scord::error_code
ping(const server& srv);

tl::expected<scord::job_info, scord::error_code>
query(const server& srv, slurm_job_id job_id);

tl::expected<scord::job, scord::error_code>
register_job(const server& srv, const job::resources& job_resources,
             const job::requirements& job_requirements,
             scord::slurm_job_id slurm_id);

scord::error_code
update_job(const server& srv, const job& job,
           const job::resources& new_resources);

scord::error_code
remove_job(const server& srv, const job& job);

tl::expected<scord::adhoc_storage, scord::error_code>
register_adhoc_storage(const server& srv, const std::string& name,
                       enum adhoc_storage::type type,
                       const adhoc_storage::ctx& ctx,
                       const adhoc_storage::resources& resources);

scord::error_code
update_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage,
                     const adhoc_storage::resources& new_resources);

scord::error_code
remove_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage);

tl::expected<std::filesystem::path, scord::error_code>
deploy_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage);

scord::error_code
terminate_adhoc_storage(const server& srv, const adhoc_storage& adhoc_storage);

tl::expected<scord::pfs_storage, scord::error_code>
register_pfs_storage(const server& srv, const std::string& name,
                     enum pfs_storage::type type, const pfs_storage::ctx& ctx);

scord::error_code
update_pfs_storage(const server& srv, const pfs_storage& pfs_storage,
                   const scord::pfs_storage::ctx& new_ctx);

scord::error_code
remove_pfs_storage(const server& srv, const pfs_storage& pfs_storage);

tl::expected<transfer, error_code>
transfer_datasets(const server& srv, const job& job,
                  const std::vector<dataset>& sources,
                  const std::vector<dataset>& targets,
                  const std::vector<qos::limit>& limits,
                  transfer::mapping mapping);

} // namespace scord::detail

#endif // SCORD_ADMIRE_IMPL_HPP
