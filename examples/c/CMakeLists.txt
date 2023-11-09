################################################################################
# Copyright 2021, Barcelona Supercomputing Center (BSC), Spain                 #
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

list(APPEND c_examples_with_controller
  # job
  ADM_register_job ADM_update_job ADM_remove_job
  # adhoc storage
  ADM_register_adhoc_storage ADM_update_adhoc_storage ADM_remove_adhoc_storage
  ADM_deploy_adhoc_storage ADM_terminate_adhoc_storage
  # transfers
  ADM_transfer_datasets ADM_get_transfer_priority ADM_set_transfer_priority
  ADM_cancel_transfer ADM_get_pending_transfers
  # transfers (user)
  ADM_transfer_datasets_user
  # qos
  ADM_set_qos_constraints ADM_get_qos_constraints
  # data operations
  ADM_define_data_operation ADM_connect_data_operation
  ADM_finalize_data_operation ADM_link_transfer_to_data_operation
  # ADM_in_situ_ops ADM_in_transit_ops
  # misc
  ADM_get_statistics ADM_set_dataset_information ADM_set_io_resources
  )

list(APPEND c_examples_without_controller
  # ping
  ADM_ping
  # pfs storage
  ADM_register_pfs_storage ADM_update_pfs_storage ADM_remove_pfs_storage
  )

add_library(c_examples_common STATIC)
target_sources(c_examples_common PUBLIC common.h PRIVATE common.c)
target_link_libraries(c_examples_common libscord_c_types)

foreach(example IN LISTS c_examples_with_controller c_examples_without_controller)
  add_executable(${example}_c)
  target_sources(${example}_c PRIVATE ${example}.c)

  if (example MATCHES "^.*_user$")
    target_link_libraries(${example}_c PUBLIC libscord-user)
  else ()
    target_link_libraries(${example}_c PUBLIC libscord)
  endif ()

  target_link_libraries(${example}_c PUBLIC c_examples_common)

  set_target_properties(${example}_c PROPERTIES OUTPUT_NAME ${example})
endforeach()

if(SCORD_BUILD_TESTS)
  foreach(example IN LISTS c_examples_with_controller)

    # prepare environment for the RPC test itself and its validation test
    set(TEST_NAME "${example}_c_test")
    set(TEST_DIRECTORY "${SCORD_TESTS_DIRECTORY}/${TEST_NAME}")
    file(MAKE_DIRECTORY ${TEST_DIRECTORY})

    set(TEST_ENV)
    list(APPEND TEST_ENV LIBSCORD_LOG=1)
    list(APPEND TEST_ENV LIBSCORD_LOG_OUTPUT=${TEST_DIRECTORY}/libscord.log)

    add_test(run_${TEST_NAME} ${example}
      ${SCORD_ADDRESS_STRING}
      ${SCORD_CTL_ADDRESS_STRING}
      ${DATA_STAGER_ADDRESS_STRING})
    set_tests_properties(run_${TEST_NAME}
      PROPERTIES FIXTURES_REQUIRED "scord_daemon;scord_ctl;cargo"
      ENVIRONMENT "${TEST_ENV}")

    add_test(validate_${TEST_NAME}
      ${CMAKE_SOURCE_DIR}/ci/check_rpcs.py
      ${example}
      ${TEST_DIRECTORY}/libscord.log
      ${SCORD_TESTS_DIRECTORY}/scord_daemon/scord_daemon.log
      ${SCORD_ADDRESS_STRING}
      ${SCORD_TESTS_DIRECTORY}/scord_daemon/scord_ctl.log
      ${SCORD_CTL_ADDRESS_STRING}
      )
    set_tests_properties(validate_${TEST_NAME}
      PROPERTIES DEPENDS stop_scord_daemon
      )
  endforeach()

  # Disable until we have more than stubs for the user library
  set_tests_properties(run_ADM_transfer_datasets_user_c_test
    PROPERTIES DISABLED TRUE)
  set_tests_properties(validate_ADM_transfer_datasets_user_c_test
    PROPERTIES DISABLED TRUE)

  foreach(example IN LISTS c_examples_without_controller)

    # prepare environment for the RPC test itself and its validation test
    set(TEST_NAME "${example}_c_test")
    set(TEST_DIRECTORY "${SCORD_TESTS_DIRECTORY}/${TEST_NAME}")
    file(MAKE_DIRECTORY ${TEST_DIRECTORY})

    set(TEST_ENV)
    list(APPEND TEST_ENV LIBSCORD_LOG=1)
    list(APPEND TEST_ENV LIBSCORD_LOG_OUTPUT=${TEST_DIRECTORY}/libscord.log)

    add_test(run_${TEST_NAME} ${example} ${SCORD_ADDRESS_STRING})
    set_tests_properties(run_${TEST_NAME}
      PROPERTIES FIXTURES_REQUIRED scord_daemon
      ENVIRONMENT "${TEST_ENV}")

    add_test(validate_${TEST_NAME}
      ${CMAKE_SOURCE_DIR}/ci/check_rpcs.py
      ${example}
      ${TEST_DIRECTORY}/libscord.log
      ${SCORD_TESTS_DIRECTORY}/scord_daemon/scord_daemon.log
      ${SCORD_ADDRESS_STRING}
      )
    set_tests_properties(validate_${TEST_NAME}
      PROPERTIES DEPENDS stop_scord_daemon
      )
  endforeach()
endif()
