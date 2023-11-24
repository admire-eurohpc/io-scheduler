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
    std::uint32_t job_id{};
    std::uint32_t adhocid{};
    std::string nodes;
    std::string adhocfs;
    std::string inputs;
    std::string outputs;
    std::string function;
    std::uint32_t qos{};
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
    app.add_option("--job_id", cfg.job_id, "Job ID (for subsequent ops)");

    app.add_option("-n,--nodes", cfg.nodes, "Nodes");

    app.add_option("-a,--adhocfs", cfg.adhocfs, "Adhoc FS type")->required();

    app.add_option("--adhocid", cfg.adhocid, "Adhoc ID");

    app.add_option("-i,--inputs", cfg.inputs,
                   "Input dataset {lustre:/a,gekkofs:/b;lustre/a1...}");

    app.add_option("-o,--outputs", cfg.outputs, "Output dataset");

    app.add_option("-q,--qos", cfg.qos, "QoS MB/s");

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

auto
create_adhoc_type_from_name(const std::string& name) {
    if(name == "gekkofs") {
        return scord::adhoc_storage::type::gekkofs;
    } else if(name == "hercules") {
        return scord::adhoc_storage::type::hercules;
    } else if(name == "expand") {
        return scord::adhoc_storage::type::expand;
    } else if(name == "dataclay") {
        return scord::adhoc_storage::type::dataclay;
    } else {
        throw std::runtime_error(
                fmt::format("Invalid adhoc fs type: {}", name));
    }
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

            auto type = create_adhoc_type_from_name(cfg.adhocfs);

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

            auto path = deploy_adhoc_storage(srv, adhoc_storage);
            fmt::print("{},{}\n", path, adhoc_storage.id());

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

            return job.id();
        }

        if(cfg.function == "stage-in") {
            // As the job is registered, we can now transfer datasets. Get
            // inputs from requirements (TODO)

            // Step 2 : If function is stage-in, transfer datasets
            // convert inputs to split inputs (src, dst)
            std::vector<scord::dataset> inputs_src, inputs_dst;

            auto v_routes_in = split(cfg.inputs, ';');

            for(auto& src_dst : v_routes_in) {
                auto route = split(src_dst, ',');

                inputs_src.push_back(scord::dataset{route[0]});
                inputs_dst.push_back(scord::dataset{route[1]});
            }

            scord::job job(cfg.job_id, cfg.slurm_id);
            std::vector<scord::qos::limit> v_qos;
            if(cfg.qos) {
                scord::qos::limit limit{scord::qos::subclass::bandwidth,
                                        cfg.qos};
                v_qos.push_back(limit);
            }

            auto transfer = scord::transfer_datasets(
                    srv, job, inputs_src, inputs_dst, v_qos,
                    scord::transfer::mapping::n_to_n);
            return transfer.id();
        }

        if(cfg.function == "wait") {
            // Wait for transfer operation to finish
            const scord::transfer transfer = scord::transfer{cfg.slurm_id};
            scord::job job(cfg.job_id, cfg.slurm_id);

            auto response = scord::query_transfer(srv, job, transfer);

            while(response.status() != scord::transfer_state::type::finished) {
                std::this_thread::sleep_for(1s);
                response = scord::query_transfer(srv, job, transfer);
                if(scord::transfer_state::type::failed == response.status()) {
                    fmt::print("Transfer failed\n");
                    return EXIT_FAILURE;
                }
            }
        }

        if(cfg.function == "stage-out") {
            // Step 3 : If function is stage-out, transfer datasets
            // convert inputs to split inputs (src, dst)
            std::vector<scord::dataset> outputs_src, outputs_dst;

            auto v_routes_out = split(cfg.outputs, ';');

            for(auto& src_dst : v_routes_out) {
                auto route = split(src_dst, ',');

                outputs_src.push_back(scord::dataset{route[0]});
                outputs_dst.push_back(scord::dataset{route[1]});
            }

            scord::job job(cfg.job_id, cfg.slurm_id);
            std::vector<scord::qos::limit> v_qos;
            if(cfg.qos) {
                scord::qos::limit limit{scord::qos::subclass::bandwidth,
                                        cfg.qos};
                v_qos.push_back(limit);
            }

            auto transfer = scord::transfer_datasets(
                    srv, job, outputs_src, outputs_dst, v_qos,
                    scord::transfer::mapping::n_to_n);
            return transfer.id();
        }

        if(cfg.function == "destroy") {
            // Step 4 : If function is destroy, terminate adhoc fs server
            // Build a scord::adhoc_storage object with the adhocid
            auto type = create_adhoc_type_from_name(cfg.adhocfs);
            scord::adhoc_storage::resources resources;
            scord::adhoc_storage::ctx ctx;

            scord::adhoc_storage adhoc_storage(type, "", cfg.adhocid, ctx,
                                               resources);
            terminate_adhoc_storage(srv, adhoc_storage);
        }

    } catch(const std::exception& ex) {
        fmt::print(stderr, "Error: {}\n", ex.what());
        return EXIT_FAILURE;
    }
}
