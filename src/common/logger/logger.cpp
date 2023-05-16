/******************************************************************************
 * Copyright 2021-2023, Barcelona Supercomputing Center (BSC), Spain
 *
 * This software was partially supported by the EuroHPC-funded project ADMIRE
 *   (Project ID: 956748, https://www.admire-eurohpc.eu).
 *
 * This file is part of scord.
 *
 * scord is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * scord is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with scord.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include <utils/utils.hpp>
#include <cstdarg>
#include "logger.hpp"
#include "logger.h"

void
logger_setup(const char* ident, logger_type type, const char* log_file) {
    constexpr auto get_cxx_type = [](logger_type t) {
        switch(t) {
            case CONSOLE_LOGGER:
                return logger::logger_type::console;
            case CONSOLE_COLOR_LOGGER:
                return logger::logger_type::console_color;
            case FILE_LOGGER:
                return logger::logger_type::file;
            case SYSLOG_LOGGER:
                return logger::logger_type::syslog;
            default:
                return logger::logger_type::console;
        }
    };
    logger::create_global_logger(ident, get_cxx_type(type), log_file);
}

void
logger_log(enum logger_level level, const char* fmt, ...) {

    using logger::logger;

    if(const auto logger = logger::get_global_logger(); logger) {

        std::array<char, LOGGER_MSG_MAX_LEN> msg; // NOLINT
        va_list args;
        va_start(args, fmt);
        vsnprintf(msg.data(), msg.size(), fmt, args);
        va_end(args);

        switch(level) {
            case info:
                logger->info(msg.data());
                break;
            case debug:
                logger->debug(msg.data());
                break;
            case warn:
                logger->warn(msg.data());
                break;
            case error:
                logger->error(msg.data());
                break;
            case critical:
                logger->critical(msg.data());
                break;
        }
    }
}

void
logger_destroy() {
    using logger::logger;

    if(logger::get_global_logger()) {
        ::logger::destroy_global_logger();
    }
}
