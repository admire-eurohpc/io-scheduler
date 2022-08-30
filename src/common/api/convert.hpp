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

#ifndef SCORD_CONVERT_HPP
#define SCORD_CONVERT_HPP

#include "admire_types.h"
#include "admire_types.hpp"

namespace admire::api {

template <typename T>
struct managed_rpc_type;

template <typename T>
struct unmanaged_rpc_type;

} // namespace admire::api


////////////////////////////////////////////////////////////////////////////////
//  Specializations for conversion types
////////////////////////////////////////////////////////////////////////////////

template <>
struct admire::api::managed_rpc_type<admire::adhoc_storage::ctx> {

    template <typename T, auto Deleter>
    using managed_ptr = scord::utils::ctype_ptr<T, Deleter>;

    explicit managed_rpc_type(const admire::adhoc_storage::ctx& ctx)
        : m_adhoc_context(ADM_adhoc_context_create(
                  static_cast<ADM_adhoc_mode_t>(ctx.exec_mode()),
                  static_cast<ADM_adhoc_access_t>(ctx.access_type()),
                  ctx.nodes(), ctx.walltime(), ctx.should_flush())) {}

    ADM_adhoc_context_t
    get() const {
        return m_adhoc_context.get();
    }

    managed_ptr<ADM_adhoc_context_t, ADM_adhoc_context_destroy> m_adhoc_context;
};


template <>
struct admire::api::managed_rpc_type<admire::adhoc_storage> {

    template <typename T, auto Deleter>
    using rpc_storage_ptr = scord::utils::ctype_ptr<T, Deleter>;

    template <typename T, auto Deleter>
    using managed_ptr = scord::utils::ctype_ptr<T, Deleter>;

    explicit managed_rpc_type(const admire::adhoc_storage& st)
        : m_adhoc_context(*std::static_pointer_cast<admire::adhoc_storage::ctx>(
                  st.context())),
          m_storage(ADM_storage_create(
                  st.id().c_str(), static_cast<ADM_storage_type_t>(st.type()),
                  m_adhoc_context.get())) {}

    ADM_storage_t
    get() const {
        return m_storage.get();
    }

    managed_rpc_type<admire::adhoc_storage::ctx> m_adhoc_context;
    rpc_storage_ptr<ADM_storage_t, ADM_storage_destroy> m_storage;
};

template <>
struct admire::api::managed_rpc_type<std::vector<admire::dataset>> {

    template <typename T, auto Deleter>
    using managed_ptr_vector = scord::utils::ctype_ptr_vector<T, Deleter>;

    explicit managed_rpc_type(const std::vector<admire::dataset>& datasets) {
        m_datasets.reserve(datasets.size());

        for(const auto& d : datasets) {
            m_datasets.emplace_back(ADM_dataset_create(d.id().c_str()));
        }
    }

    const ADM_dataset_t*
    data() const {
        return m_datasets.data();
    }

    ADM_dataset_t*
    data() {
        return m_datasets.data();
    }

    std::size_t
    size() const {
        return m_datasets.size();
    }

    managed_ptr_vector<ADM_dataset_t, ADM_dataset_destroy> m_datasets;
};

template <>
struct admire::api::managed_rpc_type<admire::job_requirements> {

    template <typename T, auto Deleter>
    using rpc_requirements_ptr = scord::utils::ctype_ptr<T, Deleter>;

    explicit managed_rpc_type(const admire::job_requirements& reqs)
        : m_inputs(reqs.inputs()), m_outputs(reqs.outputs()),
          m_storage(*std::dynamic_pointer_cast<admire::adhoc_storage>(
                  reqs.storage())),
          m_reqs(ADM_job_requirements_create(m_inputs.data(), m_inputs.size(),
                                             m_outputs.data(), m_outputs.size(),
                                             m_storage.get())) {}

    ADM_job_requirements_t
    get() const {
        return m_reqs.get();
    }

    managed_rpc_type<std::vector<admire::dataset>> m_inputs;
    managed_rpc_type<std::vector<admire::dataset>> m_outputs;
    managed_rpc_type<admire::adhoc_storage> m_storage;
    rpc_requirements_ptr<ADM_job_requirements_t, ADM_job_requirements_destroy>
            m_reqs;
};

// forward declarations
ADM_job_t
ADM_job_create(uint64_t id);
ADM_return_t
ADM_job_destroy(ADM_job_t job);

template <>
struct admire::api::managed_rpc_type<admire::job> {

    template <typename T, auto Deleter>
    using rpc_job_ptr = scord::utils::ctype_ptr<T, Deleter>;

    explicit managed_rpc_type(const admire::job& j)
        : m_job(ADM_job_create(j.id())) {}

    ADM_job_t
    get() const {
        return m_job.get();
    }

    rpc_job_ptr<ADM_job_t, ADM_job_destroy> m_job;
};

template <>
struct admire::api::managed_rpc_type<ADM_job_t> {

    template <typename T, auto Deleter>
    using rpc_job_ptr = scord::utils::ctype_ptr<T, Deleter>;

    explicit managed_rpc_type(ADM_job_t job) : m_job(job) {}

    admire::job
    get() const {
        return admire::job(m_job.get());
    }

    rpc_job_ptr<ADM_job_t, ADM_job_destroy> m_job;
};

template <>
struct admire::api::unmanaged_rpc_type<admire::job> {

    explicit unmanaged_rpc_type(const admire::job& j)
        : m_job(ADM_job_create(j.id())) {}

    ADM_job_t
    get() const {
        return m_job;
    }

    ADM_job_t m_job;
};

#endif // SCORD_CONVERT_HPP
