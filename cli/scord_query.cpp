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

#include <fmt/format.h>
#include <filesystem>
#include <CLI/CLI.hpp>
#include <scord/scord.hpp>

struct query_config {
    std::string progname;
    std::string server_address;
    std::uint32_t job_id{};
};

query_config
parse_command_line(int argc, char* argv[]) {

    query_config cfg;

    cfg.progname = std::filesystem::path{argv[0]}.filename().string();

    CLI::App app{"Scord metainfo client", cfg.progname};

    app.add_option("-s,--server", cfg.server_address, "Server address")
            ->option_text("ADDRESS")
            ->required();
    app.add_option("job_id", cfg.job_id, "Job ID")->required();

    try {
        app.parse(argc, argv);
        return cfg;
    } catch(const CLI::ParseError& ex) {
        std::exit(app.exit(ex));
    }
}

auto
parse_address(const std::string& address) {
    const auto pos = address.find("://");
    if(pos == std::string::npos) {
        throw std::runtime_error(fmt::format("Invalid address: {}", address));
    }

    const auto protocol = address.substr(0, pos);
    return std::make_pair(protocol, address);
}


int
main(int argc, char* argv[]) {

    using namespace std::chrono_literals;

    query_config cfg = parse_command_line(argc, argv);

    try {
        const auto [protocol, address] = parse_address(cfg.server_address);

        scord::server srv{protocol, address};

        scord::job_info info =
                scord::query(scord::server{protocol, address}, cfg.job_id);

        fmt::print(stdout,
                   "Job metadata:\n"
                   "  adhoc_controller_address: {}\n"
                   "  io_procs: {}\n",
                   info.adhoc_controller_address(), info.io_procs());


    } catch(const std::exception& ex) {
        fmt::print(stderr, "Error: {}\n", ex.what());
        return EXIT_FAILURE;
    }
}
