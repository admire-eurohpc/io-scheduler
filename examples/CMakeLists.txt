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

add_executable(ping)
target_sources(ping PRIVATE ping.cpp)

target_link_libraries(ping PUBLIC network_engine fmt::fmt)

add_executable(ADM_input)
target_sources(ADM_input PRIVATE ADM_input.cpp)

target_link_libraries(ADM_input PUBLIC network_engine fmt::fmt)

add_executable(ADM_output)
target_sources(ADM_output PRIVATE ADM_output.cpp)

target_link_libraries(ADM_output PUBLIC network_engine fmt::fmt)

add_executable(ADM_inout)
target_sources(ADM_inout PRIVATE ADM_inout.cpp)

target_link_libraries(ADM_inout PUBLIC network_engine fmt::fmt)

add_executable(ADM_adhoc_nodes)
target_sources(ADM_adhoc_nodes PRIVATE ADM_adhoc_nodes.cpp)

target_link_libraries(ADM_adhoc_nodes PUBLIC network_engine fmt::fmt)

add_executable(ADM_adhoc_access)
target_sources(ADM_adhoc_access PRIVATE ADM_adhoc_access.cpp)

target_link_libraries(ADM_adhoc_access PUBLIC network_engine fmt::fmt)

add_executable(ADM_adhoc_context)
target_sources(ADM_adhoc_context PRIVATE ADM_adhoc_context.cpp)

target_link_libraries(ADM_adhoc_context PUBLIC network_engine fmt::fmt)

add_executable(ADM_adhoc_context_id)
target_sources(ADM_adhoc_context_id PRIVATE ADM_adhoc_context_id.cpp)

target_link_libraries(ADM_adhoc_context_id PUBLIC network_engine fmt::fmt)

add_executable(ADM_adhoc_walltime)
target_sources(ADM_adhoc_walltime PRIVATE ADM_adhoc_walltime.cpp)

target_link_libraries(ADM_adhoc_walltime PUBLIC network_engine fmt::fmt)

add_executable(ADM_adhoc_distribution)
target_sources(ADM_adhoc_distribution PRIVATE ADM_adhoc_distribution.cpp)

target_link_libraries(ADM_adhoc_distribution PUBLIC network_engine fmt::fmt)

add_executable(ADM_adhoc_background_flush)
target_sources(ADM_adhoc_background_flush PRIVATE ADM_adhoc_background_flush.cpp)

target_link_libraries(ADM_adhoc_background_flush PUBLIC network_engine fmt::fmt)

add_executable(ADM_in_situ_ops)
target_sources(ADM_in_situ_ops PRIVATE ADM_in_situ_ops.cpp)

target_link_libraries(ADM_in_situ_ops PUBLIC network_engine fmt::fmt)

add_executable(ADM_in_transit_ops)
target_sources(ADM_in_transit_ops PRIVATE ADM_in_transit_ops.cpp)

target_link_libraries(ADM_in_transit_ops PUBLIC network_engine fmt::fmt)

add_executable(ADM_transfer_dataset)
target_sources(ADM_transfer_dataset PRIVATE ADM_transfer_dataset.cpp)

target_link_libraries(ADM_transfer_dataset PUBLIC network_engine fmt::fmt)

add_executable(ADM_set_dataset_information)
target_sources(ADM_set_dataset_information PRIVATE ADM_set_dataset_information.cpp)

target_link_libraries(ADM_set_dataset_information PUBLIC network_engine fmt::fmt)

add_executable(ADM_set_io_resources)
target_sources(ADM_set_io_resources PRIVATE ADM_set_io_resources.cpp)

target_link_libraries(ADM_set_io_resources PUBLIC network_engine fmt::fmt)

add_executable(ADM_get_transfer_priority)
target_sources(ADM_get_transfer_priority PRIVATE ADM_get_transfer_priority.cpp)

target_link_libraries(ADM_get_transfer_priority PUBLIC network_engine fmt::fmt)

add_executable(ADM_set_transfer_priority)
target_sources(ADM_set_transfer_priority PRIVATE ADM_set_transfer_priority.cpp)

target_link_libraries(ADM_set_transfer_priority PUBLIC network_engine fmt::fmt)

add_executable(ADM_cancel_transfer)
target_sources(ADM_cancel_transfer PRIVATE ADM_cancel_transfer.cpp)

target_link_libraries(ADM_cancel_transfer PUBLIC network_engine fmt::fmt)

add_executable(ADM_get_pending_transfers)
target_sources(ADM_get_pending_transfers PRIVATE ADM_get_pending_transfers.cpp)

target_link_libraries(ADM_get_pending_transfers PUBLIC network_engine fmt::fmt)

add_executable(ADM_set_qos_constraints)
target_sources(ADM_set_qos_constraints PRIVATE ADM_set_qos_constraints.cpp)

target_link_libraries(ADM_set_qos_constraints PUBLIC network_engine fmt::fmt)


