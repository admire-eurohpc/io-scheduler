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

managed_ctype<ADM_node_t>
convert(const node& node);

managed_ctype<ADM_adhoc_resources_t>
convert(const adhoc_storage::resources& res);

managed_ctype<ADM_adhoc_context_t>
convert(const adhoc_storage::ctx& ctx);

managed_ctype<ADM_adhoc_storage_t>
convert(const admire::adhoc_storage& st);

managed_ctype<ADM_dataset_t>
convert(const admire::dataset& dataset);

managed_ctype<ADM_dataset_list_t>
convert(const std::vector<admire::dataset>& datasets);

std::vector<admire::dataset>
convert(ADM_dataset_t datasets[], size_t datasets_len);

std::vector<admire::dataset>
convert(ADM_dataset_list_t list);

managed_ctype<ADM_job_resources_t>
convert(const job::resources& res);

managed_ctype<ADM_job_requirements_t>
convert(const admire::job_requirements& reqs);

managed_ctype<ADM_job_t>
convert(const job& j);

job
convert(ADM_job_t j);

managed_ctype<ADM_transfer_t>
convert(const transfer& t);

transfer
convert(ADM_transfer_t j);

managed_ctype<ADM_qos_limit_list_t>
convert(const std::vector<qos::limit>& limits);

std::vector<qos::limit>
convert(ADM_qos_limit_t limits[], size_t limits_len);

std::vector<admire::qos::limit>
convert(ADM_qos_limit_list_t list);

} // namespace admire::api


////////////////////////////////////////////////////////////////////////////////
//  Specializations for conversion types
////////////////////////////////////////////////////////////////////////////////

template <>
struct admire::api::managed_ctype<ADM_node_t> {

    explicit managed_ctype(ADM_node_t node) : m_node(node) {}

    ADM_node_t
    get() const {
        return m_node.get();
    }

    ADM_node_t
    release() {
        return m_node.release();
    }

    scord::utils::ctype_ptr<ADM_node_t, ADM_node_destroy> m_node;
};

template <>
struct admire::api::managed_ctype_array<ADM_node_t> {

    managed_ctype_array() = default;

    explicit managed_ctype_array(ADM_node_t* data, size_t size)
        : m_nodes(data, size) {}

    explicit managed_ctype_array(std::vector<ADM_node_t>&& v)
        : m_nodes(v.data(), v.size()) {}

    constexpr size_t
    size() const {
        return m_nodes.size();
    }

    constexpr const ADM_node_t*
    data() const noexcept {
        return m_nodes.data();
    }

    constexpr ADM_node_t*
    data() noexcept {
        return m_nodes.data();
    }

    constexpr ADM_node_t*
    release() noexcept {
        return m_nodes.release();
    }

    scord::utils::ctype_ptr_vector<ADM_node_t, ADM_node_destroy> m_nodes;
};

template <>
struct admire::api::managed_ctype<ADM_adhoc_resources_t> {

    managed_ctype() = default;

    explicit managed_ctype(ADM_adhoc_resources_t res,
                           managed_ctype_array<ADM_node_t>&& nodes)
        : m_adhoc_resources(res), m_nodes(std::move(nodes)) {}

    ADM_adhoc_resources_t
    get() const {
        return m_adhoc_resources.get();
    }

    ADM_adhoc_resources_t
    release() {
        std::ignore = m_nodes.release();
        return m_adhoc_resources.release();
    }

    scord::utils::ctype_ptr<ADM_adhoc_resources_t, ADM_adhoc_resources_destroy>
            m_adhoc_resources;
    managed_ctype_array<ADM_node_t> m_nodes;
};

template <>
struct admire::api::managed_ctype<ADM_adhoc_context_t> {

    managed_ctype() = default;

    explicit managed_ctype(ADM_adhoc_context_t ctx,
                           managed_ctype<ADM_adhoc_resources_t>&& resources)
        : m_adhoc_context(ctx), m_adhoc_resources(std::move(resources)) {}

    ADM_adhoc_context_t
    get() const {
        return m_adhoc_context.get();
    }

    ADM_adhoc_context_t
    release() {
        std::ignore = m_adhoc_resources.release();
        return m_adhoc_context.release();
    }

    scord::utils::ctype_ptr<ADM_adhoc_context_t, ADM_adhoc_context_destroy>
            m_adhoc_context;
    managed_ctype<ADM_adhoc_resources_t> m_adhoc_resources;
};

template <>
struct admire::api::managed_ctype<ADM_adhoc_storage_t> {

    managed_ctype() = default;

    explicit managed_ctype(ADM_adhoc_storage_t st,
                           managed_ctype<ADM_adhoc_context_t>&& ctx)
        : m_storage(st), m_ctx(std::move(ctx)) {}

    ADM_adhoc_storage_t
    get() const {
        return m_storage.get();
    }

    ADM_adhoc_storage_t
    release() {
        std::ignore = m_ctx.release();
        return m_storage.release();
    }

    scord::utils::ctype_ptr<ADM_adhoc_storage_t, ADM_adhoc_storage_destroy>
            m_storage;
    managed_ctype<ADM_adhoc_context_t> m_ctx;
};

template <>
struct admire::api::managed_ctype<ADM_dataset_t> {

    explicit managed_ctype(ADM_dataset_t dataset) : m_dataset(dataset) {}

    ADM_dataset_t
    get() const {
        return m_dataset.get();
    }

    ADM_dataset_t
    release() {
        return m_dataset.release();
    }

    scord::utils::ctype_ptr<ADM_dataset_t, ADM_dataset_destroy> m_dataset;
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
struct admire::api::managed_ctype<ADM_dataset_list_t> {

    explicit managed_ctype(ADM_dataset_list_t list) : m_list(list) {}

    ADM_dataset_list_t
    get() const {
        return m_list.get();
    }

    ADM_dataset_list_t
    release() {
        return m_list.release();
    }

    scord::utils::ctype_ptr<ADM_dataset_list_t, ADM_dataset_list_destroy>
            m_list;
};

template <>
struct admire::api::managed_ctype<ADM_job_resources_t> {

    explicit managed_ctype(ADM_job_resources_t res,
                           managed_ctype_array<ADM_node_t>&& nodes)
        : m_adhoc_resources(res), m_nodes(std::move(nodes)) {}

    ADM_job_resources_t
    get() const {
        return m_adhoc_resources.get();
    }

    ADM_job_resources_t
    release() {
        return m_adhoc_resources.release();
    }

    scord::utils::ctype_ptr<ADM_job_resources_t, ADM_job_resources_destroy>
            m_adhoc_resources;
    managed_ctype_array<ADM_node_t> m_nodes;
};

template <>
struct admire::api::managed_ctype<ADM_job_requirements_t> {

    explicit managed_ctype(ADM_job_requirements_t reqs,
                           managed_ctype_array<ADM_dataset_t>&& inputs,
                           managed_ctype_array<ADM_dataset_t>&& outputs,
                           managed_ctype<ADM_adhoc_storage_t>&& adhoc_storage)
        : m_reqs(reqs), m_inputs(std::move(inputs)),
          m_outputs(std::move(outputs)),
          m_adhoc_storage(std::move(adhoc_storage)) {}

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
    managed_ctype<ADM_adhoc_storage_t> m_adhoc_storage;
};


// forward declarations
ADM_return_t
ADM_job_destroy(ADM_job_t job);

ADM_return_t
ADM_transfer_destroy(ADM_transfer_t tx);

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

template <>
struct admire::api::managed_ctype<ADM_transfer_t> {

    explicit managed_ctype(ADM_transfer_t tx) : m_transfer(tx) {}

    ADM_transfer_t
    get() const {
        return m_transfer.get();
    }

    ADM_transfer_t
    release() {
        return m_transfer.release();
    }

    scord::utils::ctype_ptr<ADM_transfer_t, ADM_transfer_destroy> m_transfer;
};

ADM_return_t
ADM_qos_limit_destroy_all(ADM_qos_limit_t l);

template <>
struct admire::api::managed_ctype_array<ADM_qos_limit_t> {

    explicit managed_ctype_array(ADM_qos_limit_t data[], size_t size)
        : m_qos_limits(data, size) {}

    explicit managed_ctype_array(std::vector<ADM_qos_limit_t>&& v)
        : m_qos_limits(v.data(), v.size()) {}

    constexpr size_t
    size() const {
        return m_qos_limits.size();
    }

    constexpr const ADM_qos_limit_t*
    data() const noexcept {
        return m_qos_limits.data();
    }

    constexpr ADM_qos_limit_t*
    data() noexcept {
        return m_qos_limits.data();
    }

    constexpr ADM_qos_limit_t*
    release() noexcept {
        return m_qos_limits.release();
    }

    scord::utils::ctype_ptr_vector<ADM_qos_limit_t, ADM_qos_limit_destroy_all>
            m_qos_limits;
};

template <>
struct admire::api::managed_ctype<ADM_qos_limit_list_t> {

    explicit managed_ctype(ADM_qos_limit_list_t list) : m_list(list) {}

    ADM_qos_limit_list_t
    get() const {
        return m_list.get();
    }

    ADM_qos_limit_list_t
    release() {
        return m_list.release();
    }

    scord::utils::ctype_ptr<ADM_qos_limit_list_t, ADM_qos_limit_list_destroy>
            m_list;
};

#endif // SCORD_CONVERT_HPP
