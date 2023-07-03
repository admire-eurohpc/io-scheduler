################################################################################
# Copyright 2021-2023, Barcelona Supercomputing Center (BSC), Spain            #
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

message(STATUS "[${PROJECT_NAME}] Searching for Slurm")
find_package(Slurm REQUIRED)

add_library(slurm-plugin SHARED)

get_target_property(SCORD_CTL_BIN scord-ctl SCORD_CTL_BINARY)

configure_file(defaults.h.in defaults.h @ONLY)

target_sources(
  slurm-plugin PRIVATE slurmadmcli.c ${CMAKE_CURRENT_BINARY_DIR}/defaults.h
)

target_include_directories(
  slurm-plugin
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

target_link_libraries(slurm-plugin PUBLIC Slurm::Slurm libscord)

set_target_properties(
  slurm-plugin
  PROPERTIES LINKER_LANGUAGE C
             OUTPUT_NAME slurmadmcli
             PREFIX "lib"
             SUFFIX ".so"
)

install(
  TARGETS slurm-plugin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
