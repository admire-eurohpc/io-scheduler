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
#include <scord/types.hpp>

struct query_config {
    std::string progname;
    std::string server_address;
    std::string controller_address;
    std::string stager_address;
    std::uint32_t slurm_id{};
    std::uint32_t adhocid{};
    std::string nodes;
    std::string adhocfs;
    std::string inputs;
    std::string outputs;
    std::string function;
};


/* Function that delimits a std::string into a vector of strings, using a
 * specified delimiter */
std::vector<std::string>
split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while(std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


query_config
parse_command_line(int argc, char* argv[]) {

    query_config cfg;

    cfg.progname = std::filesystem::path{argv[0]}.filename().string();

    CLI::App app{"Scord adhoc client [Register a job, does adhoc fs actions]",
                 cfg.progname};

    app.add_option("-s,--server", cfg.server_address, "Server address")
            ->option_text("ADDRESS")
            ->required();
    app.add_option("-c,--controller", cfg.controller_address,
                   "Controller address")
            ->option_text("CONTROLLERADDRESS")
            ->required();
    app.add_option("-d,--stager", cfg.stager_address, "Cargo address")
            ->option_text("CARGOADDRESS")
            ->required();
    app.add_option("-j,--slurm_id", cfg.slurm_id, "Slurm ID")->required();

    app.add_option("-n,--nodes", cfg.nodes, "Nodes");

    app.add_option("-a,--adhocfs", cfg.adhocfs, "Adhoc FS type")->required();

    app.add_option("--adhocid", cfg.adhocid, "Adhoc ID");

    app.add_option("-i,--inputs", cfg.inputs,
                   "Input dataset {lustre:/a,gekkofs:/b;lustre/a1...}");

    app.add_option("-o,--outputs", cfg.outputs, "Output dataset");

    app.add_option("-f,--function", cfg.function,
                   "Function {create, stage-in, stage-out, destroy}")
            ->required();

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

        // Step 1 : If function is create, register job and adhoc fs server
        if(cfg.function == "create") {

            // Step 1a : Define job resources

            // separate SLURM nodes string into vector of nodes
            auto v_nodes = split(cfg.nodes, ',');
            std::vector<scord::node> nodes;
            for(auto& node : v_nodes) {
                nodes.push_back(scord::node{node});
            }

            scord::job::resources job_resources(nodes);

            // Step 1b : Define adhoc_storage
            /* type needs to process the sctring in cfg.adhocfs */
            auto type = scord::adhoc_storage::type::gekkofs;
            if(cfg.adhocfs == "gekkofs") {
                type = scord::adhoc_storage::type::gekkofs;
            } else if(cfg.adhocfs == "hercules") {
                type = scord::adhoc_storage::type::hercules;
            } else if(cfg.adhocfs == "expand") {
                type = scord::adhoc_storage::type::expand;
            } else {
                throw std::runtime_error(
                        fmt::format("Invalid adhoc fs type: {}", cfg.adhocfs));
            }


            std::string adhoc_name = cfg.adhocfs + std::to_string(cfg.slurm_id);
            scord::adhoc_storage::resources resources{nodes};


            scord::adhoc_storage::ctx ctx{
                    cfg.controller_address,
                    cfg.stager_address,
                    scord::adhoc_storage::execution_mode::separate_new,
                    scord::adhoc_storage::access_type::read_write,
                    100,
                    false};


            scord::adhoc_storage adhoc_storage = register_adhoc_storage(
                    srv, adhoc_name, type, ctx, resources);


            fmt::print("AdhocStorage ID: {}\n", adhoc_storage.id());

            auto path = deploy_adhoc_storage(srv, adhoc_storage);


            // Step 1c : Define job_requirements
            /*

            job::requirements::requirements(
            std::vector<scord::dataset_route> inputs,
            std::vector<scord::dataset_route> outputs,
            std::vector<scord::dataset_route> expected_outputs)
        : m_inputs(std::move(inputs)), m_outputs(std::move(outputs)),
          m_expected_outputs(std::move(expected_outputs)) {}

    job::requirements::requirements(
            std::vector<scord::dataset_route> inputs,
            std::vector<scord::dataset_route> outputs,
            std::vector<scord::dataset_route> expected_outputs,
            scord::adhoc_storage adhoc_storage)
            */


            /* Separate inputs into vector of inputs */

            std::vector<scord::dataset_route> inputs;
            auto v_routes_in = split(cfg.inputs, ';');

            for(auto& src_dst : v_routes_in) {
                auto route = split(src_dst, ',');

                inputs.push_back(scord::dataset_route(
                        scord::dataset{route[0]}, scord::dataset{route[1]}));
            }

            /* Separate outputs into vector of outputs */
            std::vector<scord::dataset_route> outputs;
            auto v_routes_out = split(cfg.outputs, ';');

            for(auto& src_dst : v_routes_out) {
                auto route = split(src_dst, ',');

                outputs.push_back(scord::dataset_route(
                        scord::dataset{route[0]}, scord::dataset{route[1]}));
            }


            scord::job::requirements job_requirements{
                    inputs, outputs, std::vector<scord::dataset_route>{},
                    adhoc_storage};

            auto job = scord::register_job(srv, job_resources, job_requirements,
                                           cfg.slurm_id);


            // Now Tranfer Datasets

            // convert inputs to split inputs (src, dst)
            std::vector<scord::dataset> inputs_src, inputs_dst;

            for(auto& route : inputs) {
                inputs_src.push_back(route.source());
                inputs_dst.push_back(route.destination());
            }

            scord::transfer_datasets(srv, job, inputs_src, inputs_dst,
                                     std::vector<scord::qos::limit>{},
                                     scord::transfer::mapping::n_to_n);
        }

    } catch(const std::exception& ex) {
        fmt::print(stderr, "Error: {}\n", ex.what());
        return EXIT_FAILURE;
    }
}
