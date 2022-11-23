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

#include <algorithm>
#include <net/proto/rpc_types.h>
#include "convert.hpp"

// forward declarations
ADM_job_t
ADM_job_create(uint64_t id, uint64_t slurm_id);

ADM_transfer_t
ADM_transfer_create(uint64_t id);

namespace {

admire::api::managed_ctype_array<ADM_node_t>
as_ctype_array(const std::vector<admire::node>& nodes) {

    std::vector<ADM_node_t> tmp;

    std::transform(nodes.cbegin(), nodes.cend(), std::back_inserter(tmp),
                   [](const admire::node& n) {
                       return ADM_node_create(n.hostname().c_str());
                   });

    return admire::api::managed_ctype_array<ADM_node_t>{std::move(tmp)};
}

admire::api::managed_ctype_array<ADM_dataset_t>
as_ctype_array(const std::vector<admire::dataset>& datasets) {

    std::vector<ADM_dataset_t> tmp;

    std::transform(datasets.cbegin(), datasets.cend(), std::back_inserter(tmp),
                   [](const admire::dataset& d) {
                       return ADM_dataset_create(d.id().c_str());
                   });

    return admire::api::managed_ctype_array<ADM_dataset_t>{std::move(tmp)};
}

} // namespace

namespace admire::api {

managed_ctype<ADM_node_t>
convert(const node& node) {
    return managed_ctype<ADM_node_t>(ADM_node_create(node.hostname().c_str()));
}

managed_ctype<ADM_adhoc_resources_t>
convert(const adhoc_storage::resources& res) {

    auto managed_nodes = as_ctype_array(res.nodes());

    ADM_adhoc_resources_t c_res = ADM_adhoc_resources_create(
            managed_nodes.data(), managed_nodes.size());

    return managed_ctype<ADM_adhoc_resources_t>{c_res,
                                                std::move(managed_nodes)};
}

managed_ctype<ADM_adhoc_context_t>
convert(const adhoc_storage::ctx& ctx) {

    auto managed_adhoc_resources = convert(ctx.resources());

    return managed_ctype<ADM_adhoc_context_t>{
            ADM_adhoc_context_create(
                    static_cast<ADM_adhoc_mode_t>(ctx.exec_mode()),
                    static_cast<ADM_adhoc_access_t>(ctx.access_type()),
                    managed_adhoc_resources.get(), ctx.walltime(),
                    ctx.should_flush()),
            std::move(managed_adhoc_resources)};
}

managed_ctype<ADM_adhoc_storage_t>
convert(const std::optional<admire::adhoc_storage>& adhoc_storage) {

    if(!adhoc_storage) {
        return managed_ctype<ADM_adhoc_storage_t>{};
    }

    return convert(adhoc_storage.value());
}

managed_ctype<ADM_adhoc_storage_t>
convert(const admire::adhoc_storage& st) {

    auto managed_ctx = convert(st.context());
    ADM_adhoc_storage_t c_st = ADM_adhoc_storage_create(
            st.name().c_str(), static_cast<ADM_adhoc_storage_type_t>(st.type()),
            st.id(), managed_ctx.get());

    return managed_ctype<ADM_adhoc_storage_t>{c_st, std::move(managed_ctx)};
}

managed_ctype<ADM_dataset_t>
convert(const admire::dataset& dataset) {
    return managed_ctype<ADM_dataset_t>(
            ADM_dataset_create(dataset.id().c_str()));
}

managed_ctype<ADM_dataset_list_t>
convert(const std::vector<admire::dataset>& datasets) {

    std::vector<ADM_dataset_t> tmp;

    std::transform(datasets.cbegin(), datasets.cend(), std::back_inserter(tmp),
                   [](const admire::dataset& d) {
                       return ADM_dataset_create(d.id().c_str());
                   });

    auto rv = managed_ctype<ADM_dataset_list_t>{
            ADM_dataset_list_create(tmp.data(), tmp.size())};

    std::for_each(tmp.cbegin(), tmp.cend(),
                  [](ADM_dataset_t d) { ADM_dataset_destroy(d); });

    return rv;
}

std::vector<admire::dataset>
convert(ADM_dataset_t datasets[], size_t datasets_len) {

    std::vector<admire::dataset> rv;
    rv.reserve(datasets_len);

    for(size_t i = 0; i < datasets_len; ++i) {
        rv.emplace_back(datasets[i]);
    }

    return rv;
}

std::vector<admire::dataset>
convert(ADM_dataset_list_t list) {

    std::vector<admire::dataset> rv;
    rv.reserve(list->l_length);

    for(size_t i = 0; i < list->l_length; ++i) {
        rv.emplace_back(&list->l_datasets[i]);
    }

    return rv;
}

managed_ctype<ADM_job_resources_t>
convert(const job::resources& res) {

    auto managed_nodes = as_ctype_array(res.nodes());

    ADM_job_resources_t c_res = ADM_job_resources_create(managed_nodes.data(),
                                                         managed_nodes.size());

    return managed_ctype<ADM_job_resources_t>{c_res, std::move(managed_nodes)};
}

managed_ctype<ADM_job_requirements_t>
convert(const admire::job_requirements& reqs) {

    const auto& adhoc_storage = reqs.adhoc_storage();

    auto managed_storage = convert(adhoc_storage);
    auto managed_inputs = as_ctype_array(reqs.inputs());
    auto managed_outputs = as_ctype_array(reqs.outputs());

    ADM_job_requirements_t c_reqs = ADM_job_requirements_create(
            managed_inputs.data(), managed_inputs.size(),
            managed_outputs.data(), managed_outputs.size(),
            managed_storage.get());

    return managed_ctype<ADM_job_requirements_t>{
            c_reqs, std::move(managed_inputs), std::move(managed_outputs),
            std::move(managed_storage)};
}


managed_ctype<ADM_job_t>
convert(const job& j) {
    return managed_ctype<ADM_job_t>(ADM_job_create(j.id(), j.slurm_id()));
}

job
convert(ADM_job_t j) {
    return admire::job{j};
}

managed_ctype<ADM_transfer_t>
convert(const transfer& tx) {
    return managed_ctype<ADM_transfer_t>(ADM_transfer_create(tx.id()));
}

transfer
convert(ADM_transfer_t tx) {
    return transfer{tx};
}

managed_ctype<ADM_qos_limit_list_t>
convert(const std::vector<qos::limit>& limits) {

    std::vector<ADM_qos_limit_t> tmp;

    std::transform(
            limits.cbegin(), limits.cend(), std::back_inserter(tmp),
            [](const admire::qos::limit& l) {
                ADM_qos_entity_t e = nullptr;

                if(l.entity()) {

                    switch(const auto s = l.entity()->scope()) {
                        case qos::scope::dataset: {
                            e = ADM_qos_entity_create(
                                    static_cast<ADM_qos_scope_t>(s),
                                    convert(l.entity()->data<admire::dataset>())
                                            .release());
                            break;
                        }

                        case qos::scope::node: {
                            e = ADM_qos_entity_create(
                                    static_cast<ADM_qos_scope_t>(s),
                                    convert(l.entity()->data<admire::node>())
                                            .release());
                            break;
                        }

                        case qos::scope::job: {
                            e = ADM_qos_entity_create(
                                    static_cast<ADM_qos_scope_t>(s),
                                    convert(l.entity()->data<admire::job>())
                                            .release());
                            break;
                        }

                        case qos::scope::transfer: {
                            e = ADM_qos_entity_create(
                                    static_cast<ADM_qos_scope_t>(s),
                                    convert(l.entity()
                                                    ->data<admire::transfer>())
                                            .release());
                            break;
                        }
                    }
                }

                return ADM_qos_limit_create(
                        e, static_cast<ADM_qos_class_t>(l.subclass()),
                        l.value());
            });

    auto rv = managed_ctype<ADM_qos_limit_list_t>{
            ADM_qos_limit_list_create(tmp.data(), tmp.size())};

    std::for_each(tmp.cbegin(), tmp.cend(),
                  [](ADM_qos_limit_t l) { ADM_qos_limit_destroy_all(l); });

    return rv;
}

std::vector<qos::limit>
convert(ADM_qos_limit_t limits[], size_t limits_len) {

    std::vector<admire::qos::limit> rv;
    rv.reserve(limits_len);

    for(size_t i = 0; i < limits_len; ++i) {
        rv.emplace_back(limits[i]);
    }

    return rv;
}

std::vector<admire::qos::limit>
convert(ADM_qos_limit_list_t list) {

    std::vector<admire::qos::limit> rv;
    rv.reserve(list->l_length);

    for(size_t i = 0; i < list->l_length; ++i) {
        rv.emplace_back(&list->l_limits[i]);
    }

    return rv;
}


} // namespace admire::api
