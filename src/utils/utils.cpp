/******************************************************************************
 * Copyright 2021, Barcelona Supercomputing Center (BSC), Spain
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

#include <string>
#include <algorithm>
#include <locale>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <utility>
#include <filesystem>

#include <utils.hpp>

namespace scord::utils {

uint64_t
parse_size(const std::string& str) {

    constexpr const uint64_t B_FACTOR = 1;
    constexpr const uint64_t KB_FACTOR = 1e3;
    constexpr const uint64_t KiB_FACTOR = (1 << 10);
    constexpr const uint64_t MB_FACTOR = 1e6;
    constexpr const uint64_t MiB_FACTOR = (1 << 20);
    constexpr const uint64_t GB_FACTOR = 1e9;
    constexpr const uint64_t GiB_FACTOR = (1 << 30);

    const std::pair<const std::string, const uint64_t> conversions[] = {
            {"GiB", GiB_FACTOR}, {"GB", GB_FACTOR}, {"G", GB_FACTOR},
            {"MiB", MiB_FACTOR}, {"MB", MB_FACTOR}, {"M", MB_FACTOR},
            {"KiB", KiB_FACTOR}, {"KB", KB_FACTOR}, {"K", KB_FACTOR},
            {"B", B_FACTOR},
    };

    std::string scopy(str);

    /* remove whitespaces from the string */
    scopy.erase(std::remove_if(scopy.begin(), scopy.end(),
                               [](char ch) {
                                   return std::isspace<char>(
                                           ch, std::locale::classic());
                               }),
                scopy.end());

    /* determine the units */
    std::size_t pos = std::string::npos;
    uint64_t factor = B_FACTOR;

    for(const auto& c : conversions) {
        const std::string& suffix = c.first;

        if((pos = scopy.find(suffix)) != std::string::npos) {
            /* check that the candidate is using the suffix EXACTLY
             * to prevent accepting something like "GBfoo" as a valid Gigabyte
             * unit */
            if(suffix != scopy.substr(pos)) {
                pos = std::string::npos;
                continue;
            }

            factor = c.second;
            break;
        }
    }

    /* this works as expected because if pos == std::string::npos, the standard
     * states that all characters until the end of the string should be
     * included.*/
    std::string number_str = scopy.substr(0, pos);

    /* check if it's a valid number */
    if(number_str.empty() ||
       !std::all_of(number_str.begin(), number_str.end(), ::isdigit)) {
        throw std::invalid_argument("Not a number");
    }

    double value = std::stod(number_str);

    return std::round(value * factor);
}

// lexically remove any ./ and ../ components from a provided pathname
// (adapted from boost::filesystem::path::lexically_normal())
std::filesystem::path
lexical_normalize(const std::filesystem::path& pathname, bool as_directory) {

    using std::filesystem::path;

    if(pathname.empty()) {
        return path{};
    }

    if(pathname == "/") {
        return path{"/"};
    }

    path tmp{"/"};

    for(const auto& elem : pathname) {
        if(elem == "..") {
            // move back on '../'
            tmp = tmp.remove_filename();

            if(tmp.empty()) {
                tmp = "/";
            }
        } else if(elem != "." && elem != "/") {
            // There is a weird case in some versions of boost, where the elem
            // is returned incorrectly for paths with 2 leading slashes:
            //    Example: *path('//a/b/c').begin() returns '//a' instead of '/'
            // If this happens, we extract the trailing component
            if(elem.native().size() > 2 &&
               ((elem.native())[0] == '/' && (elem.native())[1] == '/')) {
                tmp /= elem.native().substr(2);
            } else {
                tmp /= elem;
            }
        }
        // ignore './'
    }

    if(tmp != "/" && as_directory) {
        tmp /= "/";
    }

    return tmp;
}


// remove a trailing separator
// (adapted from boost::filesystem::path::remove_trailing_separator())
std::filesystem::path
remove_trailing_separator(const std::filesystem::path& pathname) {

    using std::filesystem::path;

    std::string str{pathname.generic_string()};

    auto is_directory_separator = [](path::value_type c) {
        return c == '/';
    };

    if(!str.empty() && is_directory_separator(str[str.size() - 1])) {
        str.erase(str.size() - 1);
    }

    return path{str};
}

// remove a leading separator
// (adapted from boost::filesystem::path::remove_trailing_separator())
std::filesystem::path
remove_leading_separator(const std::filesystem::path& pathname) {

    using std::filesystem::path;

    std::string str{pathname.generic_string()};

    auto is_directory_separator = [](path::value_type c) {
        return c == '/';
    };

    if(!str.empty() && is_directory_separator(str[0])) {
        str.erase(0, 1);
    }

    return path{str};
}

} // namespace scord::utils
