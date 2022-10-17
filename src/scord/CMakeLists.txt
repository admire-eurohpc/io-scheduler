################################################################################
# Copyright 2021-2022, Barcelona Supercomputing Center (BSC), Spain            #
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

# scord daemon
add_executable(scord)

target_sources(scord PRIVATE scord.cpp rpc_handlers.hpp rpc_handlers.cpp
  env.hpp job_manager.hpp adhoc_storage_manager.hpp)

target_include_directories(
  scord
  PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_BINARY_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
  scord
  PRIVATE common::config
          common::logger
          common::network::rpc_server
          common::network::rpc_types
          common::api::types
          common::utils
          common::abt_cxx
          tl::expected
          fmt::fmt
          Boost::program_options
          RedisPlusPlus::RedisPlusPlus
)

install(TARGETS scord DESTINATION ${CMAKE_INSTALL_BINDIR})
