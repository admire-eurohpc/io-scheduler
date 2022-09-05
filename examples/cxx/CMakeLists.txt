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

list(APPEND examples_cxx
  ADM_ping
  ADM_register_job ADM_update_job ADM_remove_job
  ADM_register_adhoc_storage ADM_update_adhoc_storage
  ADM_remove_adhoc_storage ADM_deploy_adhoc_storage
  ADM_register_pfs_storage ADM_update_pfs_storage
  ADM_remove_pfs_storage

  # ADM_in_situ_ops ADM_in_transit_ops
  ADM_transfer_dataset
  ADM_set_dataset_information ADM_set_io_resources ADM_get_transfer_priority
  ADM_set_transfer_priority ADM_cancel_transfer ADM_get_pending_transfers
  ADM_set_qos_constraints ADM_get_qos_constraints ADM_define_data_operation ADM_connect_data_operation
  ADM_finalize_data_operation ADM_link_transfer_to_data_operation ADM_get_statistics)

foreach(example IN LISTS examples_cxx)
  add_executable(${example}_cxx)
  target_sources(${example}_cxx PRIVATE ${example}.cpp)
  target_link_libraries(${example}_cxx
    PUBLIC common::network::engine fmt::fmt adm_iosched)
  set_target_properties(${example}_cxx PROPERTIES OUTPUT_NAME ${example})
endforeach()

if(SCORD_BUILD_TESTS)
  add_test(ADM_ping_test ADM_ping ofi+tcp://127.0.0.1:52000)

  add_test(ADM_cancel_transfer_cxx_test ADM_cancel_transfer ofi+tcp://127.0.0.1:52000)
  set_tests_properties(ADM_cancel_transfer_cxx_test PROPERTIES LABELS "ADM_cancel_transfer;cxx")

  add_test(ADM_connect_data_operation_cxx_test ADM_connect_data_operation ofi+tcp://127.0.0.1:52000)

  add_test(ADM_define_data_operation_cxx_test ADM_define_data_operation ofi+tcp://127.0.0.1:52000)

  add_test(ADM_deploy_adhoc_storage_cxx_test ADM_deploy_adhoc_storage ofi+tcp://127.0.0.1:52000)

  add_test(ADM_finalize_data_operation_cxx_test ADM_finalize_data_operation ofi+tcp://127.0.0.1:52000)

  add_test(ADM_get_pending_transfers_cxx_test ADM_get_pending_transfers ofi+tcp://127.0.0.1:52000)

  add_test(ADM_get_qos_constraints_cxx_test ADM_get_qos_constraints ofi+tcp://127.0.0.1:52000)

  add_test(ADM_get_transfer_priority_cxx_test ADM_get_transfer_priority ofi+tcp://127.0.0.1:52000)

  add_test(ADM_link_transfer_to_data_operation_cxx_test ADM_link_transfer_to_data_operation ofi+tcp://127.0.0.1:52000)

  add_test(ADM_register_job_cxx_test ADM_register_job ofi+tcp://127.0.0.1:52000)

  # TODO: add_test(ADM_register_pfs_storage_test ADM_register_pfs_storage ofi+tcp://127.0.0.1:52000)
  add_test(ADM_remove_adhoc_storage_cxx_test ADM_remove_adhoc_storage ofi+tcp://127.0.0.1:52000)

  add_test(ADM_remove_job_cxx_test ADM_remove_job ofi+tcp://127.0.0.1:52000)

  # TODO: add_test(ADM_remove_pfs_storage_test ADM_remove_pfs_storage ofi+tcp://127.0.0.1:52000 42)
  add_test(ADM_set_dataset_information_cxx_test ADM_set_dataset_information ofi+tcp://127.0.0.1:52000)

  add_test(ADM_set_io_resources_cxx_test ADM_set_io_resources ofi+tcp://127.0.0.1:52000)

  add_test(ADM_set_qos_constraints_cxx_test ADM_set_qos_constraints ofi+tcp://127.0.0.1:52000)

  add_test(ADM_set_transfer_priority_cxx_test ADM_set_transfer_priority ofi+tcp://127.0.0.1:52000)

  add_test(ADM_transfer_dataset_cxx_test ADM_transfer_dataset ofi+tcp://127.0.0.1:52000)

  add_test(ADM_update_adhoc_storage_cxx_test ADM_update_adhoc_storage ofi+tcp://127.0.0.1:52000)

  add_test(ADM_update_job_cxx_test ADM_update_job ofi+tcp://127.0.0.1:52000)

  # TODO: add_test(ADM_update_pfs_storage_cxx_test ADM_update_pfs_storage ofi+tcp://127.0.0.1:52000 42)
endif()
