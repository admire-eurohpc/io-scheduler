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

cmake_minimum_required(VERSION 3.19)

project(
  scord
  VERSION 0.3.6
  LANGUAGES C CXX
)

# Set default build type and also populate a list of available options
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if (is_multi_config)
  if(NOT "ASan" IN_LIST CMAKE_CONFIGURATION_TYPES)
    list(APPEND CMAKE_CONFIGURATION_TYPES ASan)
  endif()
elseif (NOT is_multi_config)
  set(default_build_type "Release")
  set(allowed_build_types ASan Debug Release MinSizeRel RelWithDebInfo)

  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowed_build_types}")

  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
      "${default_build_type}"
      CACHE STRING "Choose the type of build." FORCE
      )
  elseif (NOT CMAKE_BUILD_TYPE IN_LIST allowed_build_types)
    message(WARNING "Unknown build type '${CMAKE_BUILD_TYPE}'. "
      "Defaulting to '${default_build_type}'")
    set(CMAKE_BUILD_TYPE "${default_build_type}"
      CACHE STRING "Choose the type of build." FORCE
      )
  endif ()
endif ()

# define the desired flags for the ASan build type
set(CMAKE_C_FLAGS_ASAN
    "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer" CACHE STRING
    "Flags used by the C compiler for ASan build type or configuration." FORCE)

set(CMAKE_CXX_FLAGS_ASAN
    "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer" CACHE STRING
    "Flags used by the C++ compiler for ASan build type or configuration." FORCE)

set(CMAKE_EXE_LINKER_FLAGS_ASAN
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address" CACHE STRING
    "Linker flags to be used to create executables for ASan build type." FORCE)

set(CMAKE_SHARED_LINKER_FLAGS_ASAN
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address" CACHE STRING
    "Linker lags to be used to create shared libraries for ASan build type." FORCE)


# make sure that debug versions for targets are used (if provided) in Debug mode
set_property(GLOBAL APPEND PROPERTY DEBUG_CONFIGURATIONS Debug)
set_property(GLOBAL APPEND PROPERTY DEBUG_CONFIGURATIONS ASan)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
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

### controller bind port
set(SCORD_CTL_BIND_PORT
  "52001"
  CACHE STRING
  "Define the bind port for the ${PROJECT_NAME}-ctl controller (default: 52001)"
  )
message(STATUS "[${PROJECT_NAME}] server bind port: ${SCORD_CTL_BIND_PORT}")

set(CARGO_BIND_PORT
  "62000"
  CACHE STRING
  "Define the port through wich we should commmunicate with Cargo"
  )

option(SCORD_BUILD_EXAMPLES "Build examples (disabled by default)" OFF)

option(SCORD_BUILD_TESTS "Build tests (disabled by default)" OFF)

### REDIS_ADDRESS
set(REDIS_ADDRESS
  "tcp://127.0.0.1:6379"
  CACHE STRING
  "Define a redis address (default: tcp://127.0.0.1:6379)"
)

message(STATUS "[${PROJECT_NAME}] Redis address: ${REDIS_ADDRESS}")

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

### transport library
if (SCORD_TRANSPORT_LIBRARY STREQUAL libfabric)
  pkg_check_modules(libfabric REQUIRED IMPORTED_TARGET GLOBAL libfabric)
  add_library(transport_library ALIAS PkgConfig::libfabric)
elseif (SCORD_TRANSPORT_LIBRARY STREQUAL ucx)
  pkg_check_modules(ucx REQUIRED IMPORTED_TARGET GLOBAL ucx)
  add_library(transport_library ALIAS PkgConfig::ucx)
else ()
  message(FATAL_ERROR "Unknown transport library: ${SCORD_TRANSPORT_LIBRARY}")
endif ()

### Mercury
message(STATUS "[${PROJECT_NAME}] Checking for Mercury")
find_package(Mercury 2.0.1 REQUIRED)

### Argobots
message(STATUS "[${PROJECT_NAME}] Checking for Argobots")
find_package(Argobots 1.1 REQUIRED)

### Margo
message(STATUS "[${PROJECT_NAME}] Checking for Margo")
find_package(Margo 0.9.6 REQUIRED)

### Thallium
message(STATUS "[${PROJECT_NAME}] Checking for Thallium")
find_package(Thallium REQUIRED)

### {fmt}: required for sensible output formatting
message(STATUS "[${PROJECT_NAME}] Downloading and building {fmt}")
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG a33701196adfad74917046096bf5a2aa0ab0bb50 # v9.1.0
  GIT_SHALLOW ON
  GIT_PROGRESS ON
)

FetchContent_MakeAvailable(fmt)
set_target_properties(fmt PROPERTIES POSITION_INDEPENDENT_CODE ON)

### spdlog: required for logging
message(STATUS "[${PROJECT_NAME}] Downloading and building spdlog")
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG 7e635fca68d014934b4af8a1cf874f63989352b7 # v1.12.0
  GIT_SHALLOW ON
  GIT_PROGRESS ON
)

FetchContent_MakeAvailable(spdlog)
set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)

### rapidyaml: required for reading configuration files
message(STATUS "[${PROJECT_NAME}] Downloading and building rapidyaml")
FetchContent_Declare(
  ryml
  GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
  GIT_TAG b35ccb150282760cf5c2d316895cb86bd161ac89 #v0.5.0
  GIT_SHALLOW OFF # ensure submodules are checked out
)

FetchContent_MakeAvailable(ryml)
set_target_properties(ryml PROPERTIES POSITION_INDEPENDENT_CODE ON)

### CLI11: used for parsing command-line options
message(STATUS "[${PROJECT_NAME}] Searching for CLI11")
FetchContent_Declare(cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG 291c58789c031208f08f4f261a858b5b7083e8e2 # v2.3.2
  GIT_SHALLOW ON
  GIT_PROGRESS ON
)
FetchContent_MakeAvailable(cli11)

### expected: required for using tl::expected in the C++ library implementation
### until std::expected makes it to C++
message(STATUS "[${PROJECT_NAME}] Downloading and building tl::expected")
set(EXPECTED_BUILD_PACKAGE OFF)
set(EXPECTED_BUILD_TESTS OFF)
FetchContent_Declare(
  expected
  GIT_REPOSITORY https://github.com/TartanLlama/expected
  GIT_TAG 292eff8bd8ee230a7df1d6a1c00c4ea0eb2f0362 # v1.1.0
  GIT_SHALLOW ON
  GIT_PROGRESS ON
)

FetchContent_MakeAvailable(expected)

if (SCORD_BUILD_TESTS)

  enable_testing()

  ### catch2: required for unit testing
  message(STATUS "[${PROJECT_NAME}] Downloading and building Catch2")
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG 6e79e682b726f524310d55dec8ddac4e9c52fb5f # v3.4.0
    GIT_SHALLOW ON
    GIT_PROGRESS ON
  )

  FetchContent_MakeAvailable(Catch2)

  # Ensure that CMake can find Catch2 extra CMake modules in case
  # they are needed
  list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
endif ()

### Mark any CMake variables imported from {fmt} and spdlog as advanced, so
### that they don't appear in cmake-gui or ccmake. Similarly for FETCHCONTENT
### variables.
mark_variables_as_advanced(REGEX "^(FETCHCONTENT|fmt|FMT|spdlog|SPDLOG)_.*$")

### redisplusplus check
message(STATUS "[${PROJECT_NAME}] Checking for Redis Plus Plus")
find_package(RedisPlusPlus 1.3.3 REQUIRED)

### Cargo: required for transferring datasets between storage tiers
message(STATUS "[${PROJECT_NAME}] Checking for Cargo")
find_package(Cargo 0.3.6 REQUIRED)

message(STATUS "[${PROJECT_NAME}] Checking for Hiredis")
find_package(hiredis REQUIRED)

# ##############################################################################
# Process subdirectories
# ##############################################################################

# set compile flags
add_compile_options("-Wall" "-Wextra" "-Werror" "$<$<CONFIG:RELEASE>:-O3>")
add_compile_definitions("$<$<CONFIG:DEBUG,ASan>:SCORD_DEBUG_BUILD>")
add_compile_definitions("$<$<CONFIG:DEBUG,ASan>:LOGGER_ENABLE_DEBUG>")

add_subdirectory(cli)
add_subdirectory(etc)
add_subdirectory(src)
add_subdirectory(plugins)

if (SCORD_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif ()

if(SCORD_BUILD_TESTS)
  add_subdirectory(tests)
endif()
