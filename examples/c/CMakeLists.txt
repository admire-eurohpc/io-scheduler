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

list(APPEND examples_c
  # ping
  ADM_ping
  # job
  ADM_register_job ADM_update_job ADM_remove_job
  # adhoc storage
  ADM_register_adhoc_storage ADM_update_adhoc_storage ADM_remove_adhoc_storage
  ADM_deploy_adhoc_storage
  # pfs storage
  ADM_register_pfs_storage ADM_update_pfs_storage ADM_remove_pfs_storage
  # transfers
  ADM_transfer_datasets ADM_get_transfer_priority ADM_set_transfer_priority
  ADM_cancel_transfer ADM_get_pending_transfers
  # qos
  ADM_set_qos_constraints ADM_get_qos_constraints
  # data operations
  ADM_define_data_operation ADM_connect_data_operation
  ADM_finalize_data_operation ADM_link_transfer_to_data_operation
  # ADM_in_situ_ops ADM_in_transit_ops
  # misc
  ADM_get_statistics ADM_set_dataset_information ADM_set_io_resources
  )

add_library(c_examples_common STATIC)
target_sources(c_examples_common PUBLIC common.h PRIVATE common.c)
target_link_libraries(c_examples_common common::api::types)

foreach(example IN LISTS examples_c)
  add_executable(${example}_c)
  target_sources(${example}_c PRIVATE ${example}.c)
  target_link_libraries(${example}_c PUBLIC adm_iosched c_examples_common)
  set_target_properties(${example}_c PROPERTIES OUTPUT_NAME ${example})
endforeach()

if(SCORD_BUILD_TESTS)
  foreach(example IN LISTS examples_c)
    add_test(${example}_c_test ${example} ${SCORD_TRANSPORT_PROTOCOL}://${SCORD_BIND_ADDRESS}:${SCORD_BIND_PORT})
    set_tests_properties(${example}_c_test
      PROPERTIES FIXTURES_REQUIRED scord_daemon)
  endforeach()
endif()
