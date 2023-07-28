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

configure_file(scord.conf.in scord.conf)
configure_file(scord-ctl.conf.in scord-ctl.conf @ONLY)

# install the configuration file to sysconfdir (normally /etc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/scord.conf
              ${CMAKE_CURRENT_BINARY_DIR}/scord-ctl.conf
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}
)

# create empty 'scord' directories for logs and pidfiles in the appropriate
# locations (e.g. /var/run/scord/, /var/log/scord/, etc.
install(
  DIRECTORY
  DESTINATION ${CMAKE_INSTALL_RUNSTATEDIR}/${CMAKE_PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_LOCALSTATEDIR}/log/${CMAKE_PROJECT_NAME}
)
