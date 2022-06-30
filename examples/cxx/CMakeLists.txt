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

list(APPEND examples_cxx
            ADM_ping
            ADM_register_job ADM_update_job ADM_remove_job
            ADM_register_adhoc_storage ADM_update_adhoc_storage
            ADM_remove_adhoc_storage ADM_deploy_adhoc_storage
            # ADM_in_situ_ops ADM_in_transit_ops
            ADM_transfer_dataset
            ADM_set_dataset_information ADM_set_io_resources ADM_get_transfer_priority
            ADM_set_transfer_priority ADM_cancel_transfer ADM_get_pending_transfers
            ADM_set_qos_constraints ADM_get_qos_constraints ADM_define_data_operation ADM_connect_data_operation
            ADM_finalize_data_operation ADM_link_transfer_to_data_operation ADM_get_statistics)

foreach (example IN LISTS examples_cxx)
    add_executable(${example}_cxx)
    target_sources(${example}_cxx PRIVATE ${example}.cpp)
    target_link_libraries(${example}_cxx
      PUBLIC network_engine fmt::fmt adm_iosched)
    set_target_properties(${example}_cxx PROPERTIES OUTPUT_NAME ${example})
endforeach()
