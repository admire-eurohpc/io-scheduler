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

add_library(config STATIC)

target_include_directories(
  config PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}
)
set_property(TARGET config PROPERTY POSITION_INDEPENDENT_CODE ON)

configure_file(defaults.cpp.in defaults.cpp)

target_sources(
  config
  PRIVATE config.hpp
        config_schema.hpp
          defaults.hpp
          ${CMAKE_CURRENT_BINARY_DIR}/defaults.cpp
        file_options/file_options.hpp
        file_options/schema.hpp
        file_options/options_description.hpp
        file_options/yaml_parser.hpp
          keywords.hpp
          parsers.cpp
          parsers.hpp
          settings.cpp
          settings.hpp
)

target_link_libraries(config PRIVATE utils YAMLCpp::YAMLCpp)
