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

################################################################################
# Create a target for the C type definitions so that we can
# include them where needed
################################################################################
add_library(libscord_c_types STATIC)

target_sources(libscord_c_types PRIVATE scord/types.h types.c errors.c)

set(public_headers, "")
list(APPEND public_headers "scord/types.h")

set_target_properties(
  libscord_c_types PROPERTIES PUBLIC_HEADER "${public_headers}"
)

target_include_directories(
  libscord_c_types PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                          $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_link_libraries(
  libscord_c_types
  PRIVATE Margo::Margo common::logger
  PUBLIC fmt::fmt common::abt_cxx
)

set_property(TARGET libscord_c_types PROPERTY POSITION_INDEPENDENT_CODE ON)

################################################################################
# Create a target for the C++ type definitions so that we can
# include them where needed
################################################################################
add_library(libscord_cxx_types STATIC)

target_sources(libscord_cxx_types PRIVATE scord/types.hpp types.cpp)

set(public_headers, "")
list(APPEND public_headers "scord/types.hpp")

set_target_properties(
  libscord_cxx_types PROPERTIES PUBLIC_HEADER "${public_headers}"
)

target_include_directories(
  libscord_cxx_types PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                            $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_link_libraries(
  libscord_cxx_types
  PRIVATE libscord_c_types Margo::Margo common::logger
  PUBLIC fmt::fmt common::abt_cxx
)

set_property(TARGET libscord_cxx_types PROPERTY POSITION_INDEPENDENT_CODE ON)

################################################################################
# Create a target for the actual library that will be used by admin clients
################################################################################
add_library(libscord SHARED)

target_sources(
  libscord
  PUBLIC scord/scord.h scord/scord.hpp scord/types.hpp
  PRIVATE libscord.cpp c_wrapper.cpp utils.cpp detail/impl.hpp detail/impl.cpp
  env.hpp
)

set(public_headers, "")
list(APPEND public_headers "scord/scord.h" "scord/scord.hpp" "scord/types.hpp")

set_target_properties(libscord PROPERTIES PUBLIC_HEADER "${public_headers}")

target_include_directories(
  libscord PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_link_libraries(
  libscord
  PRIVATE common::network::rpc_client
  PUBLIC tl::expected libscord_c_types libscord_cxx_types
)

set_target_properties(libscord PROPERTIES OUTPUT_NAME "scord")

################################################################################
# Create a target for the actual library that will be used by users
################################################################################
add_library(libscord-user SHARED)

target_sources(
  libscord-user
  PUBLIC scord/scord-user.h
  PRIVATE libscord-user.c
)

set(public_headers, "")
list(APPEND public_headers "scord/scord-user.h")

set_target_properties(libscord-user PROPERTIES PUBLIC_HEADER
  "${public_headers}")

target_include_directories(
  libscord-user PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_link_libraries(
  libscord-user
  PRIVATE common::network::rpc_client
  PUBLIC libscord_c_types
)

set_target_properties(libscord-user PROPERTIES OUTPUT_NAME "scord-user")

################################################################################
# Install the libraries
################################################################################
install(
  TARGETS libscord libscord-user libscord_c_types libscord_cxx_types
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
