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

add_library(common INTERFACE)
target_include_directories(common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(utils)
target_include_directories(_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_library(common::utils ALIAS _utils)

add_subdirectory(logger)
target_include_directories(_logger INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_library(common::logger ALIAS _logger)

add_subdirectory(net)
target_include_directories(_rpc_server INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR})
add_library(common::network::rpc_server ALIAS _rpc_server)
target_include_directories(_rpc_client INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR})
add_library(common::network::rpc_client ALIAS _rpc_client)

add_subdirectory(abt_cxx)
target_include_directories(_abt_cxx INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR})
add_library(common::abt_cxx ALIAS _abt_cxx)
