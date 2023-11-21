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

#[=======================================================================[.rst:
FindSlurm
---------

Find Slurm include dirs and libraries.

Use this module by invoking find_package with the form::

  find_package(Slurm
    [version] [EXACT]     # Minimum or EXACT version e.g. 0.6.2
    [REQUIRED]            # Fail with error if Slurm is not found
    )

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``Slurm::Slurm``
  The Slurm library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``Slurm_FOUND``
  True if the system has the Slurm library.
``Slurm_VERSION``
  The version of the Slurm library which was found.
``Slurm_INCLUDE_DIRS``
  Include directories needed to use Slurm.
``Slurm_LIBRARIES``
  Libraries needed to link to Slurm.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``SLURM_INCLUDE_DIR``
  The directory containing ``slurm.h``.
``SLURM_LIBRARY``
  The path to the Slurm library.

#]=======================================================================]
find_package(PkgConfig)
pkg_check_modules(PC_SLURM QUIET SLURM)

find_path(
  SLURM_INCLUDE_DIR
  NAMES slurm/slurm.h
  PATHS
    ENV SLURM_ROOT
    "/opt/slurm"
  PATH_SUFFIXES include
)

find_library(SLURM_LIBRARY NAMES slurm

PATHS  
    ENV SLURM_ROOT 
    ENV LD_LIBRARY_PATH
    "/opt/slurm"
  PATH_SUFFIXES
    "lib"
    "lib64"
)

mark_as_advanced(SLURM_INCLUDE_DIR SLURM_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Slurm
  FOUND_VAR Slurm_FOUND
  REQUIRED_VARS SLURM_LIBRARY SLURM_INCLUDE_DIR
  VERSION_VAR Slurm_VERSION
)

if(Slurm_FOUND)
  set(Slurm_INCLUDE_DIRS ${SLURM_INCLUDE_DIR})
  set(Slurm_LIBRARIES ${SLURM_LIBRARY})
  if(NOT TARGET Slurm::Slurm)
    add_library(Slurm::Slurm UNKNOWN IMPORTED)
    set_target_properties(
      Slurm::Slurm
      PROPERTIES IMPORTED_LOCATION "${SLURM_LIBRARY}"
                 INTERFACE_INCLUDE_DIRECTORIES "${SLURM_INCLUDE_DIR}"
    )
  endif()
endif()
