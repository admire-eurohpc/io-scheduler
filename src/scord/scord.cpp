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
#include "defaults.hpp"

#include <ryml.hpp>
#include <ryml_std.hpp>

namespace fs = std::filesystem;
using namespace std::literals;

class config_yaml : public CLI::Config {
public:
    std::string
    to_config(const CLI::App*, bool, bool, std::string) const final {
        return {};
    }

    std::vector<CLI::ConfigItem>
    parse_node(const ryml::ConstNodeRef& node,
               const std::string& parent_name = "",
               const std::vector<std::string>& prefix = {}) const {
        std::vector<CLI::ConfigItem> results;

        if(node.is_map()) {
            for(const auto& c : node.children()) {
                auto copy_prefix = prefix;
                if(!parent_name.empty()) {
                    copy_prefix.push_back(parent_name);
                }

                std::string name;
                if(c.has_key()) {
                    ryml::from_chars(c.key(), &name);
                }

                const auto sub_results = parse_node(c, name, copy_prefix);
                results.insert(results.end(), sub_results.begin(),
                               sub_results.end());
            }
            return results;
        }

        if(!parent_name.empty()) {
            auto& res = results.emplace_back();
            res.name = parent_name;
            res.parents = prefix;

            if(node.is_seq()) {
                for(const auto& c : node.children()) {
                    if(c.has_val()) {
                        std::string value{c.val().data(), c.val().size()};
                        res.inputs.push_back(value);
                    }
                }
                return results;
            }

            if(node.is_keyval()) {
                std::string value{node.val().data(), node.val().size()};
                res.inputs = {value};
                return results;
            }
        }

        throw CLI::ConversionError("Missing name");
    }

    std::vector<CLI::ConfigItem>
    from_config(std::istream& input) const final {
        std::string input_str{std::istreambuf_iterator<char>(input),
                              std::istreambuf_iterator<char>()};
        ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(input_str));
        return parse_node(tree.crootref());
    }
};

int
main(int argc, char* argv[]) {

    struct {
        bool foreground = !scord::config::defaults::daemonize;
        logger::logger_type log_type = logger::logger_type::console_color;
        std::optional<fs::path> output_file;
        std::optional<fs::path> rundir;
        std::optional<std::string> address;
        std::optional<std::string> redis_address;
    } cli_args;

    const auto progname = fs::path{argv[0]}.filename().string();

    CLI::App app{"scord - An orchestrator for HPC I/O activity", progname};
    app.config_formatter(std::make_shared<config_yaml>());

    app.add_flag("-f,--foreground", cli_args.foreground, "Run in foreground");
    app.add_flag_callback(
            "-C,--force-console",
            [&]() {
                cli_args.log_type = logger::logger_type::console_color;
                cli_args.output_file = std::nullopt;
            },
            "Override any logging options defined in the configuration file "
            "and send all daemon output to the console");

    app.set_config("-c,--config-file", scord::config::defaults::config_file,
                   "Ignore the system-wide configuration file and use the "
                   "configuration provided by FILENAME",
                   /*config_required=*/true)
            ->option_text("FILENAME")
            ->check(CLI::ExistingFile);

    // force logging messages to file
    app.add_option_function<std::string>(
               "-o,--output",
               [&](const std::string& val) {
                   cli_args.log_type = logger::logger_type::file;
                   cli_args.output_file = fs::path{val};
               },
               "Write any output to FILENAME console")
            ->option_text("FILENAME")
            ->envname("SCORD_LOG_OUTPUT")
            ->excludes("-C");

    app.add_flag_callback(
            "-v,--version",
            [&]() {
                fmt::print("{} {}\n", progname, scord::version_string);
                std::exit(EXIT_SUCCESS);
            },
            "Print version string and exit");

    // options accepted by configuration file
    auto global_settings = app.add_subcommand("global_settings")
                                   ->configurable(true)
                                   ->group("");
    global_settings->add_option_function<std::string>(
            "--logfile", [&](const std::string& val) {
                cli_args.log_type = logger::logger_type::file;
                cli_args.output_file = fs::path{val};
            });
    global_settings->add_option("--rundir", cli_args.rundir);
    global_settings->add_option("--address", cli_args.address);

    global_settings->add_option("--redisaddress", cli_args.redis_address);

    CLI11_PARSE(app, argc, argv);

    // ->required(true) doesn't work with configurable subcommands
    if(!cli_args.address) {
        fmt::print(stderr,
                   "{}: error: required option 'address' missing "
                   "from configuration file\n",
                   progname);
        return EXIT_FAILURE;
    }

    try {
        scord::rpc_server srv(progname, *cli_args.address, !cli_args.foreground,
                              cli_args.rundir.value_or(fs::current_path()), *cli_args.redis_address);
        srv.configure_logger(cli_args.log_type, cli_args.output_file);
        srv.init_redis();
        return srv.run();
    } catch(const std::exception& ex) {
        fmt::print(stderr,
                   "{}: error: an unhandled exception reached the top "
                   "of main(), {} will exit:\n"
                   "  what():  {}\n",
                   progname, progname, ex.what());
        return EXIT_FAILURE;
    }
}
