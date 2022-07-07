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

#[=======================================================================[.rst:
FindHiRedis
---------

Find hiredis include dirs and libraries.

Use this module by invoking find_package with the form::

  find_package(HiRedis
    [version] [EXACT]     # Minimum or EXACT version e.g. 0.6.2
    [REQUIRED]            # Fail with error if yaml-cpp is not found
    )

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``HIRedis::HiRedis``
  The HiRedis library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``HiRedis_FOUND``
  True if the system has the HiRedis library.
``HiRedis_VERSION``
  The version of the HiRedis library which was found.
``HiRedis_INCLUDE_DIRS``
  Include directories needed to use HiRedis.
``HiRedis_LIBRARIES``
  Libraries needed to link to HiRedis.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``HiRedis_INCLUDE_DIR``
  The directory containing ``hiredis.h``.
``HiRedis_LIBRARY``
  The path to the HiRedis library.

#]=======================================================================]


find_package(PkgConfig)
pkg_check_modules(PC_HiRedis QUIET hiredis)

find_path(HiRedis_INCLUDE_DIR
  NAMES hiredis/hiredis.h
  PATHS ${PC_HiRedis_INCLUDE_DIRS}
  PATH_SUFFIXES HiRedis
)

find_library(HiRedis_LIBRARY
  NAMES hiredis
  PATHS ${PC_HiRedis_LIBRARY_DIRS}
)

mark_as_advanced(
  HiRedis_INCLUDE_DIR
  HiRedis_LIBRARY
)

set(HiRedis_VERSION ${PC_HiRedis_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HiRedis
  FOUND_VAR HiRedis_FOUND
  REQUIRED_VARS
    HiRedis_LIBRARY
    HiRedis_INCLUDE_DIR
  VERSION_VAR HiRedis_VERSION
)

if(HiRedis_FOUND)
  set(HiRedis_LIBRARIES ${HiRedis_LIBRARY})
  set(HiRedis_INCLUDE_DIRS ${HiRedis_INCLUDE_DIR})
  set(HiRedis_DEFINITIONS ${PC_HiRedis_CFLAGS_OTHER})
endif()

if(HiRedis_FOUND AND NOT TARGET HiRedis::HiRedis)
  add_library(HiRedis::HiRedis UNKNOWN IMPORTED)
  set_target_properties(HiRedis::HiRedis PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${HiRedis_LIBRARY}"
    INTERFACE_COMPILE_DEFINITIONS "${PC_HiRedis_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${HiRedis_INCLUDE_DIR}"
)
endif()
