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
#include "convert.hpp"

// forward declarations
ADM_job_t
ADM_job_create(uint64_t id);

ADM_transfer_t
ADM_transfer_create(uint64_t id);

namespace admire::api {

managed_ctype<ADM_adhoc_context_t>
convert(const adhoc_storage::ctx& ctx) {
    return managed_ctype<ADM_adhoc_context_t>{ADM_adhoc_context_create(
            static_cast<ADM_adhoc_mode_t>(ctx.exec_mode()),
            static_cast<ADM_adhoc_access_t>(ctx.access_type()), ctx.nodes(),
            ctx.walltime(), ctx.should_flush())};
}

managed_ctype<ADM_storage_t>
convert(const admire::adhoc_storage& st) {

    auto managed_ctx =
            convert(*std::static_pointer_cast<admire::adhoc_storage::ctx>(
                    st.context()));

    ADM_storage_t c_st = ADM_storage_create(
            st.id().c_str(), static_cast<ADM_storage_type_t>(st.type()),
            managed_ctx.get());

    return managed_ctype<ADM_storage_t>{c_st, std::move(managed_ctx)};
}

managed_ctype_array<ADM_dataset_t>
convert(const std::vector<admire::dataset>& datasets) {

    std::vector<ADM_dataset_t> tmp;

    std::transform(datasets.cbegin(), datasets.cend(), std::back_inserter(tmp),
                   [](const admire::dataset& d) {
                       return ADM_dataset_create(d.id().c_str());
                   });

    return managed_ctype_array<ADM_dataset_t>{std::move(tmp)};
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

managed_ctype<ADM_job_requirements_t>
convert(const admire::job_requirements& reqs) {

    const auto& adhoc_storage =
            *std::dynamic_pointer_cast<admire::adhoc_storage>(reqs.storage());

    auto managed_storage = convert(adhoc_storage);
    auto managed_inputs = convert(reqs.inputs());
    auto managed_outputs = convert(reqs.outputs());

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
    return managed_ctype<ADM_job_t>(ADM_job_create(j.id()));
}

job
convert(ADM_job_t j) {
    return admire::job{j};
}

managed_ctype<ADM_transfer_t>
convert(const transfer& tx) {
    return managed_ctype<ADM_transfer_t>(ADM_transfer_create(tx.id()));
}

} // namespace admire::api
