################################################################################
# Copyright 2022, Barcelona Supercomputing Center (BSC), Spain                 #
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

#[=======================================================================[.rst:
FindAGIOS
---------

Find AGIOS include dirs and libraries.

Use this module by invoking find_package with the form::

  find_package(AGIOS
    [REQUIRED]            # Fail with error if yaml-cpp is not found
    )


Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``AGIOS_INCLUDE_DIRS``
  Include directories needed to use Agios.
``AGIOS_LIBRARIES``
  Libraries needed to link to Agios.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``AGIOS_INCLUDE_DIR``
  The directory containing ``agios.h``.
``AGIOS_LIBRARY``
  The path to the agios library.

#]=======================================================================]


find_path(AGIOS_INCLUDE_DIR
    NAMES agios.h
)

find_library(AGIOS_LIBRARY
    NAMES agios
)

set(AGIOS_INCLUDE_DIRS ${AGIOS_INCLUDE_DIR})
set(AGIOS_LIBRARIES ${AGIOS_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AGIOS DEFAULT_MSG AGIOS_LIBRARIES AGIOS_INCLUDE_DIRS)

mark_as_advanced(
    AGIOS_LIBRARY
    AGIOS_INCLUDE_DIR
)

