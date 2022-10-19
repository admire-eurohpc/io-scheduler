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

add_library(_api_types STATIC)

target_sources(_api_types PUBLIC admire_types.h admire_types.hpp PRIVATE
  types.cpp convert.hpp convert.cpp internal_types.hpp errors.c)

target_include_directories(_api_types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(_api_types PRIVATE
  Margo::Margo common::logger PUBLIC fmt::fmt common::abt_cxx)

set_property(TARGET _api_types PROPERTY POSITION_INDEPENDENT_CODE ON)

install(
  FILES admire_types.h admire_types.hpp
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
