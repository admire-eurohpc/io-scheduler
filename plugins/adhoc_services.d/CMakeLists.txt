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

list(APPEND ADHOC_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/gekkofs.sh" 
"${CMAKE_CURRENT_SOURCE_DIR}/expand.sh"
"${CMAKE_CURRENT_SOURCE_DIR}/dataclay.sh"
"${CMAKE_CURRENT_SOURCE_DIR}/hercules.sh")

# copy adhoc scripts to the build directory so that they can be used  by tests
foreach (ADHOC_SCRIPT ${ADHOC_SCRIPTS})
  get_filename_component(ADHOC_SCRIPT_NAME ${ADHOC_SCRIPT} NAME)
  configure_file(${ADHOC_SCRIPT} ${CMAKE_CURRENT_BINARY_DIR}/${ADHOC_SCRIPT_NAME} @ONLY
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                     GROUP_READ GROUP_EXECUTE
                     WORLD_READ WORLD_EXECUTE)
endforeach()

# install adhoc scripts to `<datadir>/scord/adhoc_services.d/`
# (normally /usr/share/scord/adhoc_services.d/)
get_filename_component(INSTALL_DESTINATION ${CMAKE_CURRENT_SOURCE_DIR} NAME)

install(
  FILES ${ADHOC_SCRIPTS}
  DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/${INSTALL_DESTINATION}
  PERMISSIONS
    OWNER_EXECUTE OWNER_WRITE OWNER_READ
    GROUP_EXECUTE GROUP_READ
    WORLD_EXECUTE WORLD_READ
)
