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

// convenience types for managing the types from the C API in a RAII fashion
template <typename T>
struct managed_ctype;

template <typename T>
struct managed_ctype_array;

// conversion functions between C API and CXX API types

managed_ctype<ADM_adhoc_context_t>
convert(const adhoc_storage::ctx& ctx);

managed_ctype<ADM_storage_t>
convert(const admire::adhoc_storage& st);

managed_ctype_array<ADM_dataset_t>
convert(const std::vector<admire::dataset>& datasets);

managed_ctype<ADM_job_requirements_t>
convert(const admire::job_requirements& reqs);

managed_ctype<ADM_job_t>
convert(const job& j);

job
convert(ADM_job_t j);


} // namespace admire::api


////////////////////////////////////////////////////////////////////////////////
//  Specializations for conversion types
////////////////////////////////////////////////////////////////////////////////

template <>
struct admire::api::managed_ctype<ADM_adhoc_context_t> {

    explicit managed_ctype(ADM_adhoc_context_t ctx) : m_adhoc_context(ctx) {}

    ADM_adhoc_context_t
    get() const {
        return m_adhoc_context.get();
    }

    ADM_adhoc_context_t
    release() {
        return m_adhoc_context.release();
    }

    scord::utils::ctype_ptr<ADM_adhoc_context_t, ADM_adhoc_context_destroy>
            m_adhoc_context;
};

template <>
struct admire::api::managed_ctype<ADM_storage_t> {

    explicit managed_ctype(ADM_storage_t st,
                           managed_ctype<ADM_adhoc_context_t>&& ctx)
        : m_storage(st), m_ctx(std::move(ctx)) {}

    ADM_storage_t
    get() const {
        return m_storage.get();
    }

    ADM_storage_t
    release() {
        return m_storage.release();
    }

    scord::utils::ctype_ptr<ADM_storage_t, ADM_storage_destroy> m_storage;
    managed_ctype<ADM_adhoc_context_t> m_ctx;
};

template <>
struct admire::api::managed_ctype_array<ADM_dataset_t> {

    explicit managed_ctype_array(ADM_dataset_t* data, size_t size)
        : m_datasets(data, size) {}

    explicit managed_ctype_array(std::vector<ADM_dataset_t>&& v)
        : m_datasets(v.data(), v.size()) {}

    constexpr size_t
    size() const {
        return m_datasets.size();
    }

    constexpr const ADM_dataset_t*
    data() const noexcept {
        return m_datasets.data();
    }

    constexpr ADM_dataset_t*
    data() noexcept {
        return m_datasets.data();
    }

    constexpr ADM_dataset_t*
    release() noexcept {
        return m_datasets.release();
    }

    scord::utils::ctype_ptr_vector<ADM_dataset_t, ADM_dataset_destroy>
            m_datasets;
};

template <>
struct admire::api::managed_ctype<ADM_job_requirements_t> {

    explicit managed_ctype(ADM_job_requirements_t reqs,
                           managed_ctype_array<ADM_dataset_t>&& inputs,
                           managed_ctype_array<ADM_dataset_t>&& outputs,
                           managed_ctype<ADM_storage_t>&& storage)
        : m_reqs(reqs), m_inputs(std::move(inputs)),
          m_outputs(std::move(outputs)), m_storage(std::move(storage)) {}

    ADM_job_requirements_t
    get() const {
        return m_reqs.get();
    }

    ADM_job_requirements_t
    release() {
        return m_reqs.release();
    }


    scord::utils::ctype_ptr<ADM_job_requirements_t,
                            ADM_job_requirements_destroy>
            m_reqs;
    managed_ctype_array<ADM_dataset_t> m_inputs;
    managed_ctype_array<ADM_dataset_t> m_outputs;
    managed_ctype<ADM_storage_t> m_storage;
};


// forward declarations
ADM_return_t
ADM_job_destroy(ADM_job_t job);

template <>
struct admire::api::managed_ctype<ADM_job_t> {

    explicit managed_ctype(ADM_job_t job) : m_job(job) {}

    ADM_job_t
    get() const {
        return m_job.get();
    }

    ADM_job_t
    release() {
        return m_job.release();
    }

    scord::utils::ctype_ptr<ADM_job_t, ADM_job_destroy> m_job;
};

#endif // SCORD_CONVERT_HPP
