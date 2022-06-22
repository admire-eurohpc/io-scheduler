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

add_library(network_engine STATIC)
target_sources(
  network_engine
  INTERFACE engine.hpp
  PRIVATE detail/address.hpp
)

target_include_directories(network_engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  network_engine PUBLIC rpcs logger transport_library Mercury::Mercury
                        Argobots::Argobots Margo::Margo
)
set_property(TARGET network_engine PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(rpc_server STATIC)
target_sources(
  rpc_server
  INTERFACE server.hpp
  PRIVATE server.cpp
)

target_include_directories(
  rpc_server
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(rpc_server PUBLIC config network_engine)
