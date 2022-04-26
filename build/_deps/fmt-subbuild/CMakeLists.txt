# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.17.0)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(fmt-populate NONE)

include(ExternalProject)
ExternalProject_Add(fmt-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/fmtlib/fmt" "GIT_TAG" "d141cdbeb0fb422a3fb7173b285fd38e0d1772dc" "GIT_SHALLOW" "ON" "GIT_PROGRESS" "ON"
                    SOURCE_DIR          "/home/bscuser/new/io-scheduler/build/_deps/fmt-src"
                    BINARY_DIR          "/home/bscuser/new/io-scheduler/build/_deps/fmt-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)
