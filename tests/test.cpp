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

#include <catch2/catch_test_macros.hpp>
#include <scord/scord.h>
#include <string>

SCENARIO("Error messages can be printed", "[lib][ADM_strerror]") {

    GIVEN("An error number") {
        WHEN("The error number is ADM_SUCCESS") {
            REQUIRE(std::string{ADM_strerror(ADM_SUCCESS)} == "Success");
        }

        WHEN("The error number is ADM_ESNAFU") {
            REQUIRE(std::string{ADM_strerror(ADM_ESNAFU)} == "Internal error");
        }

        WHEN("The error number is ADM_EBADARGS") {
            REQUIRE(std::string{ADM_strerror(ADM_EBADARGS)} == "Bad arguments");
        }

        WHEN("The error number is ADM_ENOMEM") {
            REQUIRE(std::string{ADM_strerror(ADM_ENOMEM)} ==
                    "Cannot allocate memory");
        }

        WHEN("The error number is ADM_ETIMEOUT") {
            REQUIRE(std::string{ADM_strerror(ADM_ETIMEOUT)} ==
                    "Timeout");
        }
        WHEN("The error number is ADM_EOTHER") {
            REQUIRE(std::string{ADM_strerror(ADM_EOTHER)} ==
                    "Undetermined error");
        }

        WHEN("The error number is larger than ADM_EOTHER and "
             "lower than ADM_ERR_MAX") {

            for(int i = ADM_EOTHER; i < ADM_ERR_MAX; ++i) {
                const auto e = static_cast<ADM_return_t>(i);
                REQUIRE(std::string{ADM_strerror(e)} == "Undetermined error");
            }
        }

        WHEN("The error number is larger than ADM_ERR_MAX") {
            for(int i = ADM_ERR_MAX; i < ADM_ERR_MAX * 2; ++i) {
                const auto e = static_cast<ADM_return_t>(i);
                REQUIRE(std::string{ADM_strerror(e)} == "Unknown error");
            }
        }
    }
}
