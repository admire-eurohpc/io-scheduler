/******************************************************************************
 * Copyright 2021, Barcelona Supercomputing Center (BSC), Spain
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
#include <boost/program_options.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <exception>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <version.hpp>
#include <net/server.hpp>
#include <net/proto/rpc_types.h>
#include <config/settings.hpp>
#include "rpc_handlers.hpp"
#include "env.hpp"

#include <agios.h>

namespace fs = std::filesystem;
namespace bpo = boost::program_options;
using namespace std::literals;

void
print_version(const std::string& progname) {
    fmt::print("{} {}\n", progname, scord::version_string);
}

void
print_help(const std::string& progname,
           const bpo::options_description& opt_desc) {
    fmt::print("Usage: {} [options]\n\n", progname);
    fmt::print("{}", opt_desc);
}

std::unordered_map<std::string, std::string>
load_envs() {

    std::unordered_map<std::string, std::string> envs;

    if(const auto p = std::getenv(scord::env::LOG);
       p && !std::string{p}.empty() && std::string{p} != "0") {
        if(const auto log_file = std::getenv(scord::env::LOG_OUTPUT)) {
            envs.emplace(scord::env::LOG_OUTPUT, log_file);
        }
    }

    return envs;
}

int
main(int argc, char* argv[]) {

    scord::config::settings cfg;

    // define the command line options allowed
    bpo::options_description opt_desc("Options");
    opt_desc.add_options()
            // run in foreground
            (",f",
             bpo::bool_switch()->default_value(false)->notifier(
                     [&](const bool& flag_value) {
                         cfg.daemonize(!flag_value);
                     }),
             "foreground operation")

            // force logging messages to the console
            ("force-console,C",
             bpo::value<std::string>()
                     ->implicit_value("")
                     ->zero_tokens()
                     ->notifier([&](const std::string&) {
                         cfg.log_file(fs::path{});
                         cfg.use_console(true);
                     }),
             "override any logging options defined in configuration files and "
             "send all daemon output to the console")

            // use provided configuration file instead of the system-wide
            // configuration file defined when building the daemon
            ("config-file,c",
             bpo::value<fs::path>()
                     ->value_name("FILENAME")
                     ->implicit_value("")
                     ->notifier([&](const std::string& filename) {
                         cfg.config_file(filename);
                     }),
             "override the system-wide configuration file with FILENAME")

            // print the daemon version
            ("version,v",
             bpo::value<std::string>()->implicit_value("")->zero_tokens(),
             "print version string")

            // print help
            ("help,h",
             bpo::value<std::string>()->implicit_value("")->zero_tokens(),
             "produce help message");

    // parse the command line
    bpo::variables_map vm;

    try {
        bpo::store(bpo::parse_command_line(argc, argv, opt_desc), vm);

        // the --help and --version arguments are special, since we want
        // to process them even if the global configuration file doesn't exist
        if(vm.count("help")) {
            print_help(cfg.progname(), opt_desc);
            return EXIT_SUCCESS;
        }

        if(vm.count("version")) {
            print_version(cfg.progname());
            return EXIT_SUCCESS;
        }

        const fs::path config_file = (vm.count("config-file") == 0)
                                             ? cfg.config_file()
                                             : vm["config-file"].as<fs::path>();

        if(!fs::exists(config_file)) {
            fmt::print(stderr,
                       "Failed to access daemon configuration file {}\n",
                       config_file);
            return EXIT_FAILURE;
        }

        try {
            cfg.load_from_file(config_file);
        } catch(const std::exception& ex) {
            fmt::print(stderr,
                       "Failed reading daemon configuration file:\n"
                       "    {}\n",
                       ex.what());
            return EXIT_FAILURE;
        }

        // override settings from the configuration file with settings
        // from environment variables
        const auto env_opts = load_envs();

        if(const auto& it = env_opts.find(scord::env::LOG_OUTPUT);
           it != env_opts.end()) {
            cfg.log_file(it->second);
        }

        // calling notify() here basically invokes all define notifiers, thus
        // overriding any configuration loaded from the global configuration
        // file with its command-line counterparts if provided (for those
        // options where this is available)
        bpo::notify(vm);
    } catch(const bpo::error& ex) {
        fmt::print(stderr, "ERROR: {}\n\n", ex.what());
        return EXIT_FAILURE;
    }

    try {
        scord::network::server daemon(cfg);

        daemon.set_handler("ADM_ping"s, scord::network::handlers::ping);
        daemon.set_handler("ADM_register_adhoc_storage"s,
                           scord::network::handlers::register_adhoc_storage);
        daemon.set_handler("ADM_update_adhoc_storage"s,
                           scord::network::handlers::update_adhoc_storage);
        daemon.set_handler("ADM_remove_adhoc_storage"s,
                           scord::network::handlers::remove_adhoc_storage);
        daemon.set_handler("ADM_register_job"s,
                           scord::network::handlers::register_job);

#if 0
        const auto rpc_registration_cb = [](auto&& ctx) {
            LOGGER_INFO(" * Registering RPCs handlers...");

            REGISTER_RPC(ctx, "ADM_ping", void, ADM_ping_out_t, ADM_ping, true);

            REGISTER_RPC(ctx, "ADM_register_job", ADM_register_job_in_t,
                         ADM_register_job_out_t, ADM_register_job, true);
            REGISTER_RPC(ctx, "ADM_update_job", ADM_update_job_in_t,
                         ADM_update_job_out_t, ADM_update_job, true);
            REGISTER_RPC(ctx, "ADM_remove_job", ADM_remove_job_in_t,
                         ADM_remove_job_out_t, ADM_remove_job, true);

            REGISTER_RPC(ctx, "ADM_register_adhoc_storage",
                         ADM_register_adhoc_storage_in_t,
                         ADM_register_adhoc_storage_out_t,
                         ADM_register_adhoc_storage, true);
            REGISTER_RPC(ctx, "ADM_update_adhoc_storage",
                         ADM_update_adhoc_storage_in_t,
                         ADM_update_adhoc_storage_out_t,
                         ADM_update_adhoc_storage, true);
            REGISTER_RPC(ctx, "ADM_remove_adhoc_storage",
                         ADM_remove_adhoc_storage_in_t,
                         ADM_remove_adhoc_storage_out_t,
                         ADM_remove_adhoc_storage, true);

            REGISTER_RPC(ctx, "ADM_deploy_adhoc_storage",
                         ADM_deploy_adhoc_storage_in_t,
                         ADM_deploy_adhoc_storage_out_t,
                         ADM_deploy_adhoc_storage, true);

            REGISTER_RPC(ctx, "ADM_register_pfs_storage",
                         ADM_register_pfs_storage_in_t,
                         ADM_register_pfs_storage_out_t,
                         ADM_register_pfs_storage, true);
            REGISTER_RPC(
                    ctx, "ADM_update_pfs_storage", ADM_update_pfs_storage_in_t,
                    ADM_update_pfs_storage_out_t, ADM_update_pfs_storage, true);
            REGISTER_RPC(
                    ctx, "ADM_remove_pfs_storage", ADM_remove_pfs_storage_in_t,
                    ADM_remove_pfs_storage_out_t, ADM_remove_pfs_storage, true);

            REGISTER_RPC(ctx, "ADM_input", ADM_input_in_t, ADM_input_out_t,
                         ADM_input, true);

            REGISTER_RPC(ctx, "ADM_output", ADM_output_in_t, ADM_output_out_t,
                         ADM_output, true);

            REGISTER_RPC(ctx, "ADM_inout", ADM_inout_in_t, ADM_inout_out_t,
                         ADM_inout, true);

            REGISTER_RPC(ctx, "ADM_adhoc_context", ADM_adhoc_context_in_t,
                         ADM_adhoc_context_out_t, ADM_adhoc_context, true);

            REGISTER_RPC(ctx, "ADM_adhoc_context_id", ADM_adhoc_context_id_in_t,
                         ADM_adhoc_context_id_out_t, ADM_adhoc_context_id,
                         true);

            REGISTER_RPC(ctx, "ADM_adhoc_nodes", ADM_adhoc_nodes_in_t,
                         ADM_adhoc_nodes_out_t, ADM_adhoc_nodes, true);

            REGISTER_RPC(ctx, "ADM_adhoc_walltime", ADM_adhoc_walltime_in_t,
                         ADM_adhoc_walltime_out_t, ADM_adhoc_walltime, true);

            REGISTER_RPC(ctx, "ADM_adhoc_access", ADM_adhoc_access_in_t,
                         ADM_adhoc_access_out_t, ADM_adhoc_access, true);

            REGISTER_RPC(
                    ctx, "ADM_adhoc_distribution", ADM_adhoc_distribution_in_t,
                    ADM_adhoc_distribution_out_t, ADM_adhoc_distribution, true);

            REGISTER_RPC(ctx, "ADM_adhoc_background_flush",
                         ADM_adhoc_background_flush_in_t,
                         ADM_adhoc_background_flush_out_t,
                         ADM_adhoc_background_flush, true);

            REGISTER_RPC(ctx, "ADM_in_situ_ops", ADM_in_situ_ops_in_t,
                         ADM_in_situ_ops_out_t, ADM_in_situ_ops, true);

            REGISTER_RPC(ctx, "ADM_in_transit_ops", ADM_in_transit_ops_in_t,
                         ADM_in_transit_ops_out_t, ADM_in_transit_ops, true);

            REGISTER_RPC(
                    ctx, "ADM_transfer_datasets", ADM_transfer_datasets_in_t,
                    ADM_transfer_datasets_out_t, ADM_transfer_datasets, true);

            REGISTER_RPC(ctx, "ADM_set_dataset_information",
                         ADM_set_dataset_information_in_t,
                         ADM_set_dataset_information_out_t,
                         ADM_set_dataset_information, true);

            REGISTER_RPC(ctx, "ADM_set_io_resources", ADM_set_io_resources_in_t,
                         ADM_set_io_resources_out_t, ADM_set_io_resources,
                         true);

            REGISTER_RPC(ctx, "ADM_get_transfer_priority",
                         ADM_get_transfer_priority_in_t,
                         ADM_get_transfer_priority_out_t,
                         ADM_get_transfer_priority, true);

            REGISTER_RPC(ctx, "ADM_set_transfer_priority",
                         ADM_set_transfer_priority_in_t,
                         ADM_set_transfer_priority_out_t,
                         ADM_set_transfer_priority, true);

            REGISTER_RPC(ctx, "ADM_cancel_transfer", ADM_cancel_transfer_in_t,
                         ADM_cancel_transfer_out_t, ADM_cancel_transfer, true);

            REGISTER_RPC(ctx, "ADM_get_pending_transfers",
                         ADM_get_pending_transfers_in_t,
                         ADM_get_pending_transfers_out_t,
                         ADM_get_pending_transfers, true);

            REGISTER_RPC(ctx, "ADM_set_qos_constraints",
                         ADM_set_qos_constraints_in_t,
                         ADM_set_qos_constraints_out_t, ADM_set_qos_constraints,
                         true);

            REGISTER_RPC(ctx, "ADM_get_qos_constraints",
                         ADM_get_qos_constraints_in_t,
                         ADM_get_qos_constraints_out_t, ADM_get_qos_constraints,
                         true);

            REGISTER_RPC(ctx, "ADM_define_data_operation",
                         ADM_define_data_operation_in_t,
                         ADM_define_data_operation_out_t,
                         ADM_define_data_operation, true);

            REGISTER_RPC(ctx, "ADM_connect_data_operation",
                         ADM_connect_data_operation_in_t,
                         ADM_connect_data_operation_out_t,
                         ADM_connect_data_operation, true);

            REGISTER_RPC(ctx, "ADM_finalize_data_operation",
                         ADM_finalize_data_operation_in_t,
                         ADM_finalize_data_operation_out_t,
                         ADM_finalize_data_operation, true);

            REGISTER_RPC(ctx, "ADM_link_transfer_to_data_operation",
                         ADM_link_transfer_to_data_operation_in_t,
                         ADM_link_transfer_to_data_operation_out_t,
                         ADM_link_transfer_to_data_operation, true);

            REGISTER_RPC(ctx, "ADM_get_statistics", ADM_get_statistics_in_t,
                         ADM_get_statistics_out_t, ADM_get_statistics, true);

            // TODO: add internal RPCs for communication with scord-ctl
        };
#endif

        return daemon.run();
    } catch(const std::exception& ex) {
        fmt::print(stderr,
                   "An unhandled exception reached the top of main(), "
                   "{} will exit:\n  what():  {}\n",
                   cfg.progname(), ex.what());
        return EXIT_FAILURE;
    }
}
