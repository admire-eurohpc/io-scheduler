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

# scord-ctl daemon
add_executable(scord-ctl)

target_sources(
  scord-ctl PRIVATE scord_ctl.cpp rpc_server.cpp rpc_server.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/defaults.hpp config_file.hpp config_file.cpp
  command.hpp command.cpp
)

configure_file(defaults.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/defaults.hpp @ONLY)

target_include_directories(
  scord-ctl
  PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_BINARY_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
  scord-ctl PRIVATE common::logger common::network::rpc_server
                    libscord_cxx_types fmt::fmt CLI11::CLI11 ryml::ryml
)

set_target_properties(
  scord-ctl PROPERTIES __INSTALLED_PATH
                       ${CMAKE_INSTALL_FULL_BINDIR}/scord-ctl
)

install(TARGETS scord-ctl DESTINATION ${CMAKE_INSTALL_BINDIR})
