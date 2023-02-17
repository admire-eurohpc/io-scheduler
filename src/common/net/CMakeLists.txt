################################################################################
# Copyright 2021, Barcelona Supercomputing Center (BSC), Spain                 #
#                                                                              #
# This software was partially supported by the EuroHPC-funded project ADMIRE   #
#   (Project ID: 956748, https://www.admire-eurohpc.eu).                       #
#                                                                              #
# This file is part of scord.                                                  #
#                                                                              #
# scord is free software: you can redistribute it and/or modify                #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation, either version 3 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# scord is distributed in the hope that it will be useful,                     #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with scord.  If not, see <https://www.gnu.org/licenses/>.              #
#                                                                              #
# SPDX-License-Identifier: GPL-3.0-or-later                                    #
################################################################################

add_library(_rpc_client STATIC)
target_sources(
  _rpc_client
  INTERFACE endpoint.hpp client.hpp request.hpp serialization.hpp
  PRIVATE endpoint.cpp client.cpp
)

target_link_libraries(_rpc_client PUBLIC common::config common::logger thallium)
set_property(TARGET _rpc_client PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(_rpc_server STATIC)
target_sources(
  _rpc_server
  INTERFACE server.hpp request.hpp serialization.hpp
  PRIVATE server.cpp
)

target_link_libraries(_rpc_server PUBLIC common::config common::logger thallium)
