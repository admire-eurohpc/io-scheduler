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

# scord_ping: ping a remote scord server
add_executable(scord_ping)

target_sources(scord_ping
  PRIVATE
  scord_ping.cpp
)

target_link_libraries(scord_ping
  PUBLIC fmt::fmt CLI11::CLI11 libscord)

install(TARGETS scord_ping
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# scord_query: query a remote scord server
add_executable(scord_query)

target_sources(scord_query
  PRIVATE
  scord_query.cpp
)

target_link_libraries(scord_query
  PUBLIC fmt::fmt CLI11::CLI11 libscord)

install(TARGETS scord_query
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_target_properties(
  scord_query PROPERTIES __INSTALLED_PATH
                         ${CMAKE_INSTALL_FULL_BINDIR}/scord_query
)


# scord_adhoc: creates an adhocfs with cargo transfers
add_executable(scord_adhoc)

target_sources(scord_adhoc
  PRIVATE
  scord_adhoc.cpp
)

target_link_libraries(scord_adhoc
  PUBLIC fmt::fmt CLI11::CLI11 libscord)

install(TARGETS scord_adhoc
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_target_properties(
  scord_adhoc PROPERTIES __INSTALLED_PATH
                         ${CMAKE_INSTALL_FULL_BINDIR}/scord_adhoc
)
