/******************************************************************************
 * Copyright 2021-2023, Barcelona Supercomputing Center (BSC), Spain
 *
 * This software was partially supported by the EuroHPC-funded project ADMIRE
 *   (Project ID: 956748, https://www.admire-eurohpc.eu).
 *
 * This file is part of the scord API.
 *
 * The scord API is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The scord API is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with the scord API.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *****************************************************************************/

#include <string>
#include <scord/types.h>
#include <vector>


namespace {

std::vector<std::string>
split(const std::string& text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end;

    while((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }

    tokens.push_back(text.substr(start));
    return tokens;
}

} // namespace

extern "C" ADM_return_t
scord_utils_parse_dataset_routes(const char* routes,
                                 ADM_dataset_route_t** parsed_routes,
                                 size_t* parsed_routes_count) {

    std::vector<ADM_dataset_route_t> tmp;

    if(routes == nullptr || parsed_routes == nullptr ||
       parsed_routes_count == nullptr) {
        return ADM_EBADARGS;
    }

    const std::string route_str(routes);

    if(route_str.empty()) {
        return ADM_EBADARGS;
    }

    for(auto&& rs : split(route_str, ';')) {

        const auto parts = split(rs, '=');

        if(parts.size() != 2) {
            return ADM_EBADARGS;
        }

        ADM_dataset_route_t dr =
                ADM_dataset_route_create(ADM_dataset_create(parts[0].c_str()),
                                         ADM_dataset_create(parts[1].c_str()));

        if(dr == nullptr) {
            return ADM_ENOMEM;
        }

        tmp.push_back(dr);
    }

    *parsed_routes = static_cast<ADM_dataset_route_t*>(
            malloc(tmp.size() * sizeof(ADM_dataset_route_t)));

    if(*parsed_routes == nullptr) {
        return ADM_ENOMEM;
    }

    *parsed_routes_count = tmp.size();

    for(std::size_t i = 0; i < tmp.size(); i++) {
        (*parsed_routes)[i] = tmp[i];
    }

    return ADM_SUCCESS;
}
