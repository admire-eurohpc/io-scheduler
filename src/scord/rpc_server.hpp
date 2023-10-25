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

#ifndef SCORD_RPC_SERVER_HPP
#define SCORD_RPC_SERVER_HPP

#include <string>
#include <filesystem>
#include <net/server.hpp>
#include "job_manager.hpp"
#include "adhoc_storage_manager.hpp"
#include "pfs_storage_manager.hpp"

namespace scord {

class rpc_server : public network::server,
                   public network::provider<rpc_server> {

public:
    rpc_server(std::string name, std::string address, bool daemonize,
               std::filesystem::path rundir);

private:
    void
    ping(const network::request& req);

    void
    query(const network::request& req, scord::job_id job_id);

    void
    register_job(const network::request& req,
                 const scord::job::resources& job_resources,
                 const scord::job::requirements& job_requirements,
                 scord::slurm_job_id slurm_id);

    void
    update_job(const network::request& req, scord::job_id job_id,
               const scord::job::resources& new_resources);

    void
    remove_job(const network::request& req, scord::job_id job_id);

    void
    register_adhoc_storage(const network::request& req, const std::string& name,
                           enum scord::adhoc_storage::type type,
                           const scord::adhoc_storage::ctx& ctx,
                           const scord::adhoc_storage::resources& resources);
    void
    update_adhoc_storage(const network::request& req, std::uint64_t adhoc_id,
                         const scord::adhoc_storage::resources& new_resources);

    void
    remove_adhoc_storage(const network::request& req, std::uint64_t adhoc_id);

    void
    deploy_adhoc_storage(const network::request& adhoc_metadata_ptr,
                         std::uint64_t adhoc_id);

    void
    terminate_adhoc_storage(const network::request& adhoc_metadata_ptr,
                            std::uint64_t adhoc_id);

    void
    register_pfs_storage(const network::request& req, const std::string& name,
                         enum scord::pfs_storage::type type,
                         const scord::pfs_storage::ctx& ctx);

    void
    update_pfs_storage(const network::request& req, std::uint64_t pfs_id,
                       const scord::pfs_storage::ctx& new_ctx);

    void
    remove_pfs_storage(const network::request& req, std::uint64_t pfs_id);

    void
    transfer_datasets(const network::request& req, scord::job_id job_id,
                      const std::vector<scord::dataset>& sources,
                      const std::vector<scord::dataset>& targets,
                      const std::vector<scord::qos::limit>& limits,
                      enum scord::transfer::mapping mapping);

    job_manager m_job_manager;
    adhoc_storage_manager m_adhoc_manager;
    pfs_storage_manager m_pfs_manager;
};

} // namespace scord

#endif // SCORD_RPC_SERVER_HPP
