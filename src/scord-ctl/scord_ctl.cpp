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

#include <filesystem>
#include <fmt/format.h>
#include <exception>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <CLI/CLI.hpp>

#include <version.hpp>
#include "rpc_server.hpp"

namespace fs = std::filesystem;
using namespace std::literals;


int
main(int argc, char* argv[]) {

    struct {
        std::optional<fs::path> output_file;
        std::string address;
    } cli_args;

    const auto progname = fs::path{argv[0]}.filename().string();

    CLI::App app{"", progname};

    // force logging messages to file
    app.add_option("-o,--output", cli_args.output_file,
                   "Write any output to FILENAME rather than sending it to the "
                   "console")
            ->option_text("FILENAME");

    app.add_option("-l,--listen", cli_args.address,
                   "Address or interface to bind the daemon to. If using "
                   "`libfabric`,\n"
                   "the address is typically in the form of:\n\n"
                   "  ofi+<protocol>[://<hostname,IP,interface>:<port>]\n\n"
                   "Check `fi_info` to see the list of available protocols.\n")
            ->option_text("ADDRESS")
            ->required();

    app.add_flag_function(
            "-v,--version",
            [&](auto /*count*/) {
                fmt::print("{} {}\n", progname, scord::version_string);
                std::exit(EXIT_SUCCESS);
            },
            "Print version and exit");

    CLI11_PARSE(app, argc, argv);

    try {
        scord_ctl::rpc_server srv(progname, cli_args.address, false,
                                  fs::current_path());
        if(cli_args.output_file) {
            srv.configure_logger(logger::logger_type::file,
                                 *cli_args.output_file);
        }
        return srv.run();
    } catch(const std::exception& ex) {
        fmt::print(stderr,
                   "An unhandled exception reached the top of main(), "
                   "{} will exit:\n  what():  {}\n",
                   progname, ex.what());
        return EXIT_FAILURE;
    }
}
