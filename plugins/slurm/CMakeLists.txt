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

get_target_property(SCORDCTL_PROGRAM scord-ctl __INSTALLED_PATH)
get_target_property(SCORD_QUERY_PROGRAM scord_query __INSTALLED_PATH)

configure_file(defaults.h.in defaults.h @ONLY)

target_sources(
  slurm-plugin PRIVATE slurmadmcli.c ${CMAKE_CURRENT_BINARY_DIR}/defaults.h
                       utils.c utils.h
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

find_program(MPIEXEC_EXECUTABLE
  NAMES mpiexec mpiexec.hydra mpiexec.mpd mpirun
  PATH_SUFFIXES bin sbin
  DOC "MPI launcher executable")

set(SCORD_SERVICE_ADDRESS
  ${SCORD_TRANSPORT_PROTOCOL}://${SCORD_BIND_ADDRESS}:${SCORD_BIND_PORT})

configure_file(scord_common.sh.in scord_common.sh @ONLY)
configure_file(scord_prolog.sh.in scord_prolog.sh @ONLY)
configure_file(scord_epilog.sh.in scord_epilog.sh @ONLY)

get_filename_component(INSTALL_DESTINATION ${CMAKE_CURRENT_SOURCE_DIR} NAME)

install(
  PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/scord_common.sh
    ${CMAKE_CURRENT_BINARY_DIR}/scord_prolog.sh
    ${CMAKE_CURRENT_BINARY_DIR}/scord_epilog.sh
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/${INSTALL_DESTINATION}
)
