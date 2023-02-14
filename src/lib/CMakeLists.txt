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

add_library(libscord SHARED)

target_sources(libscord
  PUBLIC admire.h admire.hpp
  PRIVATE admire.cpp c_wrapper.cpp detail/impl.hpp detail/impl.cpp env.hpp)

set_target_properties(libscord PROPERTIES PUBLIC_HEADER "admire.h;admire.hpp")

target_include_directories(libscord PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(libscord PRIVATE
  common::network::rpc_client
  common::network::rpc_types PUBLIC
  tl::expected common::api::types)

set_target_properties(libscord PROPERTIES OUTPUT_NAME "scord")

install(
  TARGETS libscord
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
