################################################################################
# Copyright 2021-2022, Barcelona Supercomputing Center (BSC), Spain                 #
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
FindRedisPlusPlus
---------

Find RedisPlusPlus include dirs and libraries.

Use this module by invoking find_package with the form::

  find_package(RedisPlusPlus
    [version] [EXACT]     # Minimum or EXACT version e.g. 0.6.2
    [REQUIRED]            # Fail with error if yaml-cpp is not found
    )

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``RedisPlusPlus::RedisPlusPlus``
  The RedisPlusPlus library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``RedisPlusPlus_FOUND``
  True if the system has the RedisPlusPlus library.
``RedisPlusPlus_VERSION``
  The version of the RedisPlusPlus library which was found.
``RedisPlusPlus_INCLUDE_DIRS``
  Include directories needed to use RedisPlusPlus.
``RedisPlusPlus_LIBRARIES``
  Libraries needed to link to RedisPlusPlus.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``RedisPlusPlus_INCLUDE_DIR``
  The directory containing ``redis++.h``.
``RedisPlusPlus_LIBRARY``
  The path to the RedisPlusPlus library.

#]=======================================================================]


find_package(PkgConfig)
pkg_check_modules(PC_RedisPlusPlus QUIET redisplusplus)

find_path(RedisPlusPlus_INCLUDE_DIR
  NAMES sw/redis++/redis++.h
  PATHS ${PC_RedisPlusPlus_INCLUDE_DIRS}
  PATH_SUFFIXES RedisPlusPlus
)

find_library(RedisPlusPlus_LIBRARY
  NAMES redis++
  PATHS ${PC_RedisPlusPlus_LIBRARY_DIRS}
)

mark_as_advanced(
  RedisPlusPlus_INCLUDE_DIR
  RedisPlusPlus_LIBRARY
)

set(RedisPlusPlus_VERSION ${PC_RedisPlusPlus_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RedisPlusPlus
  FOUND_VAR RedisPlusPlus_FOUND
  REQUIRED_VARS
    RedisPlusPlus_LIBRARY
    RedisPlusPlus_INCLUDE_DIR
  VERSION_VAR RedisPlusPlus_VERSION
)

if(RedisPlusPlus_FOUND)
  set(RedisPlusPlus_LIBRARIES ${RedisPlusPlus_LIBRARY})
  set(RedisPlusPlus_INCLUDE_DIRS ${RedisPlusPlus_INCLUDE_DIR})
  set(RedisPlusPlus_DEFINITIONS ${PC_RedisPlusPlus_CFLAGS_OTHER})
endif()

if(RedisPlusPlus_FOUND AND NOT TARGET RedisPlusPlus::RedisPlusPlus)
  add_library(RedisPlusPlus::RedisPlusPlus UNKNOWN IMPORTED)
  set_target_properties(RedisPlusPlus::RedisPlusPlus PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${RedisPlusPlus_LIBRARY}"
    INTERFACE_COMPILE_DEFINITIONS "${PC_RedisPlusPlus_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${RedisPlusPlus_INCLUDE_DIR}"
)
endif()
