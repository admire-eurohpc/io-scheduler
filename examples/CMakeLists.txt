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

if(SCORD_BUILD_TESTS)
  set(SCORD_TESTS_DIRECTORY "${CMAKE_BINARY_DIR}/Testing")
  file(MAKE_DIRECTORY ${SCORD_TESTS_DIRECTORY})

  # prepare the environment for the scord_daemon fixture
  set(TEST_DIRECTORY "${SCORD_TESTS_DIRECTORY}/scord_daemon")
  file(MAKE_DIRECTORY ${TEST_DIRECTORY})

  set(TEST_ENV)
  list(APPEND TEST_ENV SCORD_LOG=1)
  list(APPEND TEST_ENV SCORD_LOG_OUTPUT=${TEST_DIRECTORY}/scord_daemon.log)

  add_test(start_scord_daemon
    ${CMAKE_SOURCE_DIR}/scripts/runner.sh start scord.pid
           ${CMAKE_BINARY_DIR}/src/scord/scord -f
  )

  set_tests_properties(start_scord_daemon
    PROPERTIES FIXTURES_SETUP scord_daemon
    ENVIRONMENT "${TEST_ENV}")

  add_test(stop_scord_daemon
    ${CMAKE_SOURCE_DIR}/scripts/runner.sh stop TERM scord.pid
    )

  set_tests_properties(stop_scord_daemon PROPERTIES FIXTURES_CLEANUP
    scord_daemon)
endif()

add_subdirectory(c)
add_subdirectory(cxx)
