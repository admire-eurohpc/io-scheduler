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

#ifndef LIBSCORD_ENV_HPP
#define LIBSCORD_ENV_HPP

#define LIBSCORD_ENV_PREFIX "LIBSCORD_"
#define ADD_PREFIX(str)     LIBSCORD_ENV_PREFIX str

namespace scord::env {

static constexpr auto LOG = ADD_PREFIX("LOG");
static constexpr auto LOG_OUTPUT = ADD_PREFIX("LOG_OUTPUT");

} // namespace scord::env


#endif // LIBSCORD_ENV_HPP
