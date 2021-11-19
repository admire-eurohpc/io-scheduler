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

# ##############################################################################
# Define the CMake project and configure CMake
# ##############################################################################

# FetchContent_MakeAvailable() was introduced in 3.14
cmake_minimum_required(VERSION 3.14)

project(
  scord
  VERSION 0.1.0
  LANGUAGES CXX
)

# Set default build type and also populate a list of available options
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(NOT is_multi_config)
  set(default_build_type "Release")
  set(allowed_build_types Debug Release MinSizeRel RelWithDebInfo)

  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowed_build_types}")

  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        "${default_build_type}"
        CACHE STRING "Choose the type of build." FORCE
    )
  elseif(NOT CMAKE_BUILD_TYPE IN_LIST allowed_build_types)
    message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
  endif()
endif()

# make sure that debug versions for targets are used (if provided) in Debug mode
set_property(GLOBAL APPEND PROPERTY DEBUG_CONFIGURATIONS Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "[${PROJECT_NAME}] Project version: ${PROJECT_VERSION}")
configure_file(src/version.hpp.in src/version.hpp)

# FetchContent defines FetchContent_Declare() and FetchContent_MakeAvailable()
# which are used to download some dependencies
include(FetchContent)

# GNUInstallDirs defines variables such as BINDIR, SBINDIR, SYSCONFDIR, etc.
# that are substituted when generating defaults.cpp below
include(GNUInstallDirs)

# Make sure that CMake can find our internal modules
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Import some convenience functions
include(scord-utils)

# ##############################################################################
# Project configuration options
# ##############################################################################

### transport library
set(SCORD_TRANSPORT_LIBRARY
    "libfabric"
    CACHE STRING
          "Transport library used by ${PROJECT_NAME} (default: libfabric)"
)
set_property(CACHE SCORD_TRANSPORT_LIBRARY PROPERTY STRINGS libfabric ucx)
message(
  STATUS "[${PROJECT_NAME}] Transport library: ${SCORD_TRANSPORT_LIBRARY}"
)

### server transport protocol
set(SCORD_TRANSPORT_PROTOCOL
    "tcp"
    CACHE
      STRING
      "Change the default transport protocol for the ${PROJECT_NAME} server (default: tcp)"
)
message(
  STATUS
    "[${PROJECT_NAME}] server default transport protocol: ${SCORD_TRANSPORT_PROTOCOL}"
)

### server bind address
set(SCORD_BIND_ADDRESS
    "127.0.0.1"
    CACHE
      STRING
      "Define the bind address for the ${PROJECT_NAME} server (default: 127.0.0.1)"
)
message(STATUS "[${PROJECT_NAME}] server bind address: ${SCORD_BIND_ADDRESS}")

### server bind port
set(SCORD_BIND_PORT
    "52000"
    CACHE STRING
          "Define the bind port for the ${PROJECT_NAME} server (default: 52000)"
)
message(STATUS "[${PROJECT_NAME}] server bind port: ${SCORD_BIND_PORT}")

option(SCORD_BUILD_EXAMPLES "Build examples (disabled by default)" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ##############################################################################
# Check for and/or download dependencies
# ##############################################################################

### some dependencies don't provide CMake modules, but rely on pkg-config
### instead, make sure that pkg-config is available
find_package(PkgConfig REQUIRED)

### boost libraries: required for processing program options
message(STATUS "[${PROJECT_NAME}] Checking for boost libraries")
find_package(Boost 1.53 REQUIRED COMPONENTS program_options)

### yaml-cpp: required for reading configuration files
message(STATUS "[${PROJECT_NAME}] Checking for yaml-cpp")
find_package(YAMLCpp 0.6.2 REQUIRED)

### transport library
if(SCORD_TRANSPORT_LIBRARY STREQUAL libfabric)
  pkg_check_modules(libfabric REQUIRED IMPORTED_TARGET GLOBAL libfabric)
  add_library(transport_library ALIAS PkgConfig::libfabric)
elseif(SCORD_TRANSPORT_LIBRARY STREQUAL ucx)
  pkg_check_modules(ucx REQUIRED IMPORTED_TARGET GLOBAL ucx)
  add_library(transport_library ALIAS PkgConfig::ucx)
else()
  message(FATAL_ERROR "Unknown transport library: ${SCORD_TRANSPORT_LIBRARY}")
endif()

### Mercury
message(STATUS "[${PROJECT_NAME}] Checking for Mercury")
find_package(Mercury 2.0.1 REQUIRED)

### Argobots
message(STATUS "[${PROJECT_NAME}] Checking for Argobots")
find_package(Argobots 1.1 REQUIRED)

### Margo
message(STATUS "[${PROJECT_NAME}] Checking for Margo")
find_package(Margo 0.9.6 REQUIRED)

### {fmt}: required for sensible output formatting
message(STATUS "[${PROJECT_NAME}] Downloading and building {fmt}")
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG d141cdbeb0fb422a3fb7173b285fd38e0d1772dc # v8.0.1
  GIT_SHALLOW ON
  GIT_PROGRESS ON
)

FetchContent_MakeAvailable(fmt)

### spdlog: required for logging
message(STATUS "[${PROJECT_NAME}] Downloading and building spdlog")
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG eb3220622e73a4889eee355ffa37972b3cac3df5 # v1.9.2
  GIT_SHALLOW ON
  GIT_PROGRESS ON
)

FetchContent_MakeAvailable(spdlog)

### Mark any CMake variables imported from {fmt} and spdlog as advanced, so
### that they don't appear in cmake-gui or ccmake. Similarly for FETCHCONTENT
### variables.
mark_variables_as_advanced(REGEX "^(FETCHCONTENT|fmt|FMT|spdlog|SPDLOG)_.*$")

# ##############################################################################
# Process subdirectories
# ##############################################################################
add_subdirectory(etc)
add_subdirectory(src)

if(SCORD_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
