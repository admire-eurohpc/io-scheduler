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
# Configure CMake
# ##############################################################################

# FetchContent_MakeAvailable() was introduced in 3.14
cmake_minimum_required(VERSION 3.14)

# Set default build type and also populate a list of available options
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "[hermes] Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# ##############################################################################
# Define the CMake project
# ##############################################################################
project(
  scord
  VERSION 0.1.0
  LANGUAGES CXX
)

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
list(
  PREPEND
  CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

# ##############################################################################
# Project configuration options
# ##############################################################################

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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ##############################################################################
# Check for and/or download dependencies
# ##############################################################################

### boost libraries: required for processing program options
message(STATUS "[${PROJECT_NAME}] Checking for boost libraries")
find_package(
  Boost
  1.53
  REQUIRED
  COMPONENTS program_options
)

### yaml-cpp: required for reading configuration files
message(STATUS "[${PROJECT_NAME}] Checking for yaml-cpp")
find_package(
  YAMLCpp
  0.6.2
  REQUIRED
)

message(STATUS "[${PROJECT_NAME}] Checking for Mercury")
find_package(
  Mercury
  2.0.1
  REQUIRED
)

message(STATUS "[${PROJECT_NAME}] Checking for Argobots")
find_package(
  Argobots
  1.1
  REQUIRED
)

message(STATUS "[${PROJECT_NAME}] Checking for Margo")
find_package(
  Margo
  0.9.6
  REQUIRED
)


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

# ##############################################################################
# Process subdirectories
# ##############################################################################
add_subdirectory(etc)
add_subdirectory(src)
