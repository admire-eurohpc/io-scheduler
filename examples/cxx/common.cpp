#include "common.hpp"

using std::string_literals::operator""s;

cli_args
process_args(int argc, char* argv[], const test_info& test_info) {

    int required_args = 1;

    if(test_info.requires_server) {
        ++required_args;
    }

    if(test_info.requires_controller) {
        ++required_args;
    }

    if(test_info.requires_data_stager) {
        ++required_args;
    }

    if(argc != required_args) {
        fmt::print(stderr, "ERROR: missing arguments\n");
        fmt::print(stderr, "Usage: {}{}{}{}\n", test_info.name,
                   test_info.requires_server ? " <SERVER_ADDRESS>" : "",
                   test_info.requires_controller ? " <CONTROLLER_ADDRESS>" : "",
                   test_info.requires_data_stager ? " <DATA_STAGER_ADDRESS>"
                                                  : "");
        exit(EXIT_FAILURE);
    }

    return cli_args{test_info.requires_server ? std::string{argv[1]} : ""s,
                    test_info.requires_controller ? std::string{argv[2]} : ""s,
                    test_info.requires_data_stager ? std::string{argv[3]}
                                                   : ""s};
}

std::vector<scord::node>
prepare_nodes(size_t n) {
    std::vector<scord::node> nodes;
    nodes.reserve(n);
    for(size_t i = 0; i < n; ++i) {
        nodes.emplace_back(fmt::format("node-{:02d}", i));
    }

    return nodes;
}

std::vector<scord::dataset>
prepare_datasets(const std::string& pattern, size_t n) {
    std::vector<scord::dataset> datasets;
    datasets.reserve(n);
    for(size_t i = 0; i < n; ++i) {
        datasets.emplace_back(fmt::format(fmt::runtime(pattern), i));
    }

    return datasets;
}

std::vector<scord::qos::limit>
prepare_qos_limits(size_t n) {

    std::vector<scord::qos::limit> limits;
    limits.reserve(n);

    for(size_t i = 0; i < n; ++i) {
        limits.emplace_back(scord::qos::subclass::bandwidth, 50);
    }

    return limits;
}
