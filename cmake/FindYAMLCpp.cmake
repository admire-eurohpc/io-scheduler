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
FindYAMLCpp
---------

Find yaml-cpp include dirs and libraries.

Use this module by invoking find_package with the form::

  find_package(YAMLCpp
    [version] [EXACT]     # Minimum or EXACT version e.g. 0.6.2
    [REQUIRED]            # Fail with error if yaml-cpp is not found
    )

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``YAMLCpp::YAMLCpp``
  The YAMLCpp library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``YAMLCpp_FOUND``
  True if the system has the YAMLCpp library.
``YAMLCpp_VERSION``
  The version of the YAMLCpp library which was found.
``YAMLCpp_INCLUDE_DIRS``
  Include directories needed to use YAMLCpp.
``YAMLCpp_LIBRARIES``
  Libraries needed to link to YAMLCpp.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``YAMLCpp_INCLUDE_DIR``
  The directory containing ``yaml.h``.
``YAMLCpp_LIBRARY``
  The path to the YAMLCpp library.

#]=======================================================================]

# yaml-cpp already provides a config file, but sadly it doesn't define
# a target with the appropriate properties. Fortunately, it also provides a
# pkg-config .pc file that we can use to fetch the required library properties
# and wrap them neatly into the 'YAMLCpp' target we provide
find_package(PkgConfig)
pkg_check_modules(PC_YAMLCpp QUIET yaml-cpp)

find_path(YAMLCpp_INCLUDE_DIR
  NAMES yaml-cpp/yaml.h
  PATHS ${PC_YAMLCpp_INCLUDE_DIRS}
  PATH_SUFFIXES YAMLCpp
)

find_library(YAMLCpp_LIBRARY
  NAMES yaml-cpp
  PATHS ${PC_YAMLCpp_LIBRARY_DIRS}
)

mark_as_advanced(
  YAMLCpp_INCLUDE_DIR
  YAMLCpp_LIBRARY
)

set(YAMLCpp_VERSION ${PC_YAMLCpp_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YAMLCpp
  FOUND_VAR YAMLCpp_FOUND
  REQUIRED_VARS
    YAMLCpp_LIBRARY
    YAMLCpp_INCLUDE_DIR
  VERSION_VAR YAMLCpp_VERSION
)

if(YAMLCpp_FOUND)
  set(YAMLCpp_LIBRARIES ${YAMLCpp_LIBRARY})
  set(YAMLCpp_INCLUDE_DIRS ${YAMLCpp_INCLUDE_DIR})
  set(YAMLCpp_DEFINITIONS ${PC_YAMLCpp_CFLAGS_OTHER})
endif()

if(YAMLCpp_FOUND AND NOT TARGET YAMLCpp::YAMLCpp)
  add_library(YAMLCpp::YAMLCpp UNKNOWN IMPORTED)
  set_target_properties(YAMLCpp::YAMLCpp PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${YAMLCpp_LIBRARY}"
    INTERFACE_COMPILE_DEFINITIONS "${PC_YAMLCpp_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${YAMLCpp_INCLUDE_DIR}"
)
endif()
