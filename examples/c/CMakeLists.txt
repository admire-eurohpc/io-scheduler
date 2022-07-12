# ###############################################################################
# Copyright 2021-2022, Barcelona Supercomputing Center (BSC), Spain            #
# #
# This software was partially supported by the EuroHPC-funded project ADMIRE   #
# (Project ID: 956748, https://www.admire-eurohpc.eu).                       #
# #
# This file is part of scord.                                                  #
# #
# scord is free software: you can redistribute it and/or modify                #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation, either version 3 of the License, or            #
# (at your option) any later version.                                          #
# #
# scord is distributed in the hope that it will be useful,                     #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
# #
# You should have received a copy of the GNU General Public License            #
# along with scord.  If not, see <https://www.gnu.org/licenses/>.              #
# #
# SPDX-License-Identifier: GPL-3.0-or-later                                    #
# ###############################################################################

list(APPEND examples_c ADM_register_job ADM_cancel_transfer ADM_connect_data_operation ADM_define_data_operation
  ADM_deploy_adhoc_storage ADM_finalize_data_operation ADM_get_pending_transfers ADM_get_qos_constraints
  ADM_get_statistics ADM_get_transfer_priority)

foreach(example IN LISTS examples_c)
  add_executable(${example}_c)
  target_sources(${example}_c PRIVATE ${example}.c)
  target_link_libraries(${example}_c PUBLIC adm_iosched)
  set_target_properties(${example}_c PROPERTIES OUTPUT_NAME ${example})
endforeach()

enable_testing()

add_test(ADM_register_job_test ADM_register_job ofi+tcp://127.0.0.1:52000 42)

add_test(ADM_cancel_transfer_test ADM_cancel_transfer ofi+tcp://127.0.0.1:52000 42)

add_test(ADM_connect_data_operation_test ADM_connect_data_operation ofi+tcp://127.0.0.1:52000 42 /tmp true arguments 10)

add_test(ADM_define_data_operation_test ADM_define_data_operation ofi+tcp://127.0.0.1:52000 /tmp 42 arguments)

add_test(ADM_deploy_adhoc_storage_test ADM_deploy_adhoc_storage ofi+tcp://127.0.0.1:52000 42)

add_test(ADM_finalize_data_operation_test ADM_finalize_data_operation ofi+tcp://127.0.0.1:52000 42)

add_test(ADM_get_pending_transfers_test ADM_get_pending_transfers ofi+tcp://127.0.0.1:52000)

add_test(ADM_get_qos_constraints_test ADM_get_qos_constraints ofi+tcp://127.0.0.1:52000 dataset 42)

add_test(ADM_get_statistics_test ADM_get_statistics ofi+tcp://127.0.0.1:52000 42 1)

add_test(ADM_get_transfer_priority_test ADM_get_transfer_priority ofi+tcp://127.0.0.1:52000 8)