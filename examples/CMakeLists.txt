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

  configure_file(scord.conf.in scord.conf)
  configure_file(scord-ctl.conf.in scord-ctl.conf)

  set(SCORD_ADDRESS_STRING
    ${SCORD_TRANSPORT_PROTOCOL}://${SCORD_BIND_ADDRESS}:${SCORD_BIND_PORT})

  set(DATA_STAGER_ADDRESS_STRING
    ${SCORD_TRANSPORT_PROTOCOL}://${SCORD_BIND_ADDRESS}:${CARGO_BIND_PORT})

  add_test(start_scord_daemon
    ${CMAKE_SOURCE_DIR}/scripts/runner.sh start scord.pid
           ${CMAKE_BINARY_DIR}/src/scord/scord -f -c ${CMAKE_CURRENT_BINARY_DIR}/scord.conf
  )

  set_tests_properties(start_scord_daemon
    PROPERTIES FIXTURES_SETUP scord_daemon)

  add_test(stop_scord_daemon
    ${CMAKE_SOURCE_DIR}/scripts/runner.sh stop TERM scord.pid
    )

  set_tests_properties(stop_scord_daemon PROPERTIES FIXTURES_CLEANUP
    scord_daemon)

  set(SCORD_CTL_TRANSPORT_PROTOCOL ${SCORD_TRANSPORT_PROTOCOL})
  set(SCORD_CTL_BIND_ADDRESS ${SCORD_BIND_ADDRESS})
  set(SCORD_CTL_ADDRESS_STRING
    ${SCORD_CTL_TRANSPORT_PROTOCOL}://${SCORD_CTL_BIND_ADDRESS}:${SCORD_CTL_BIND_PORT})

  add_test(start_scord_ctl
    ${CMAKE_SOURCE_DIR}/scripts/runner.sh start scord-ctl.pid
        ${CMAKE_BINARY_DIR}/src/scord-ctl/scord-ctl
            -c ${CMAKE_CURRENT_BINARY_DIR}/scord-ctl.conf
            -l ${SCORD_CTL_ADDRESS_STRING}
            -o ${TEST_DIRECTORY}/scord_ctl.log
  )

  set_tests_properties(start_scord_ctl
    PROPERTIES FIXTURES_SETUP scord_ctl)

  add_test(stop_scord_ctl
    ${CMAKE_SOURCE_DIR}/scripts/runner.sh stop TERM scord-ctl.pid
    )

  set_tests_properties(stop_scord_ctl PROPERTIES FIXTURES_CLEANUP scord_ctl)


  add_test(start_cargo
    ${CMAKE_SOURCE_DIR}/scripts/runner.sh start cargo.pid
           mpirun --allow-run-as-root -n 2 ${CARGO_BIN_INSTALL_DIR}/cargo -l ${DATA_STAGER_ADDRESS_STRING} -o ${TEST_DIRECTORY}/cargo.log
  )
  set_tests_properties(start_cargo
    PROPERTIES FIXTURES_SETUP cargo)

  add_test(stop_cargo
    ${CMAKE_SOURCE_DIR}/scripts/runner.sh stop TERM cargo.pid)

  set_tests_properties(stop_cargo
    PROPERTIES FIXTURES_CLEANUP cargo)

endif()

add_subdirectory(c)
add_subdirectory(cxx)
