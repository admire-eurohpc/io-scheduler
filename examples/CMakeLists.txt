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

list(APPEND examples
            ping ADM_input ADM_output ADM_inout ADM_adhoc_context ADM_adhoc_context_id
            ADM_adhoc_nodes ADM_adhoc_walltime ADM_adhoc_access ADM_adhoc_distribution
            ADM_adhoc_background_flush ADM_in_situ_ops ADM_in_transit_ops ADM_transfer_dataset
            ADM_set_dataset_information ADM_set_io_resources ADM_get_transfer_priority
            ADM_set_transfer_priority ADM_cancel_transfer ADM_get_pending_transfers
            ADM_set_qos_constraints_push ADM_define_data_operation ADM_connect_data_operation
            ADM_finalize_data_operation ADM_link_transfer_to_data_operation ADM_get_statistics)

foreach (example IN LISTS examples)
    add_executable(${example})
    target_sources(${example} PRIVATE ${example}.cpp)
    target_link_libraries(${example} PUBLIC network_engine fmt::fmt)
endforeach()

enable_testing()

add_test(ping_test ping ofi+tcp://127.0.0.1:52000)

add_test(ADM_input_test ADM_input ofi+tcp://127.0.0.1:52000 /build /build/examples)
add_test(ADM_input_test_fail ADM_input ofi+tcp://127.0.0.1:52000 /build)
set_tests_properties(ADM_input_test_fail PROPERTIES WILL_FAIL TRUE)

add_test(ADM_output_test ADM_output ofi+tcp://127.0.0.1:52000 /build/examples /build)
add_test(ADM_output_test_fail ADM_output ofi+tcp://127.0.0.1:52000 /build/examples)
set_tests_properties(ADM_output_test_fail PROPERTIES WILL_FAIL TRUE)

add_test(ADM_inout_test ADM_inout ofi+tcp://127.0.0.1:52000 /build/examples /build)
add_test(ADM_inout_test_fail ADM_inout ofi+tcp://127.0.0.1:52000 /build/examples)
set_tests_properties(ADM_inout_test_fail PROPERTIES WILL_FAIL TRUE)

add_test(ADM_adhoc_context_id_test ADM_adhoc_context_id ofi+tcp://127.0.0.1:52000 42)
add_test(ADM_adhoc_context_id_test_fail_s ADM_adhoc_context_id ofi+tcp://127.0.0.1:52000 hello)
set_tests_properties(ADM_adhoc_context_id_test_fail_s PROPERTIES WILL_FAIL TRUE)
add_test(ADM_adhoc_context_id_test_fail_a ADM_adhoc_context_id ofi+tcp://127.0.0.1:52000)
set_tests_properties(ADM_adhoc_context_id_test_fail_a PROPERTIES WILL_FAIL TRUE)

add_test(ADM_adhoc_nodes_test ADM_adhoc_nodes ofi+tcp://127.0.0.1:52000 10)
add_test(ADM_adhoc_nodes_test_fail_s ADM_adhoc_nodes ofi+tcp://127.0.0.1:52000 hello)
set_tests_properties(ADM_adhoc_nodes_test_fail_s PROPERTIES WILL_FAIL TRUE)
add_test(ADM_adhoc_nodes_test_fail_a ADM_adhoc_nodes ofi+tcp://127.0.0.1:52000)
set_tests_properties(ADM_adhoc_nodes_test_fail_a PROPERTIES WILL_FAIL TRUE)

add_test(ADM_adhoc_walltime_test ADM_adhoc_walltime ofi+tcp://127.0.0.1:52000 60)
add_test(ADM_adhoc_walltime_test_fail_s ADM_adhoc_walltime ofi+tcp://127.0.0.1:52000 hello)
set_tests_properties(ADM_adhoc_walltime_test_fail_s PROPERTIES WILL_FAIL TRUE)
add_test(ADM_adhoc_walltime_test_fail_a ADM_adhoc_walltime ofi+tcp://127.0.0.1:52000)
set_tests_properties(ADM_adhoc_walltime_test_fail_a PROPERTIES WILL_FAIL TRUE)

add_test(ADM_adhoc_access_test ADM_adhoc_access ofi+tcp://127.0.0.1:52000 read-only)
add_test(ADM_adhoc_access_test_fail_s ADM_adhoc_access ofi+tcp://127.0.0.1:52000 hello)
set_tests_properties(ADM_adhoc_access_test_fail_s PROPERTIES WILL_FAIL TRUE)
add_test(ADM_adhoc_access_test_fail_a ADM_adhoc_access ofi+tcp://127.0.0.1:52000)
set_tests_properties(ADM_adhoc_access_test_fail_a PROPERTIES WILL_FAIL TRUE)

add_test(ADM_adhoc_distribution_test ADM_adhoc_distribution ofi+tcp://127.0.0.1:52000 wide-striping)
add_test(ADM_adhoc_distribution_test_fail_a ADM_adhoc_distribution ofi+tcp://127.0.0.1:52000)
set_tests_properties(ADM_adhoc_distribution_test_fail_a PROPERTIES WILL_FAIL TRUE)

add_test(ADM_adhoc_background_flush_test1 ADM_adhoc_background_flush ofi+tcp://127.0.0.1:52000 true)
add_test(ADM_adhoc_background_flush_test2 ADM_adhoc_background_flush ofi+tcp://127.0.0.1:52000 FALSE)
add_test(ADM_adhoc_background_flush_test_fail_i ADM_adhoc_background_flush ofi+tcp://127.0.0.1:52000 10)
set_tests_properties(ADM_adhoc_background_flush_test_fail_i PROPERTIES WILL_FAIL TRUE)
add_test(ADM_adhoc_background_flush_test_fail_s ADM_adhoc_background_flush ofi+tcp://127.0.0.1:52000 hello)
set_tests_properties(ADM_adhoc_background_flush_test_fail_s PROPERTIES WILL_FAIL TRUE)
add_test(ADM_adhoc_background_flush_test_fail_a ADM_adhoc_background_flush ofi+tcp://127.0.0.1:52000)
set_tests_properties(ADM_adhoc_background_flush_test_fail_a PROPERTIES WILL_FAIL TRUE)
