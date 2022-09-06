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

add_library(adm_iosched SHARED)

target_sources(adm_iosched
  PUBLIC admire.h admire.hpp
  PRIVATE admire.cpp c_wrapper.cpp detail/impl.hpp detail/impl.cpp errors.c)

set_target_properties(adm_iosched PROPERTIES PUBLIC_HEADER "admire.h;admire.hpp")

target_include_directories(adm_iosched PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(adm_iosched PRIVATE common::network::engine
  common::network::rpc_types PUBLIC
  tl::expected common::api::types)

install(
  TARGETS adm_iosched
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)