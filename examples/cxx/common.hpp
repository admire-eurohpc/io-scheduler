#ifndef SCORD_CXX_EXAMPLES_COMMON_HPP
#define SCORD_CXX_EXAMPLES_COMMON_HPP

#include <vector>
#include <scord/types.hpp>

#define NJOB_NODES   50
#define NADHOC_NODES 25
#define NINPUTS      10
#define NOUTPUTS     5
#define NEXPOUTPUTS  1

#define TESTNAME                                                               \
    (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1   \
                                      : __FILE__)

struct test_info {
    std::string name;
    bool requires_server;
    bool requires_controller;
    bool requires_data_stager;
};

struct cli_args {
    std::string server_address;
    std::string controller_address;
    std::string data_stager_address;
};

cli_args
process_args(int argc, char* argv[], const test_info& test_info);

std::vector<scord::node>
prepare_nodes(size_t n);

std::vector<scord::dataset>
prepare_datasets(const std::string& pattern, size_t n);

std::vector<scord::dataset_route>
prepare_routes(const std::string& pattern, size_t n);

std::vector<scord::qos::limit>
prepare_qos_limits(size_t n);

#endif // SCORD_CXX_EXAMPLES_COMMON_HPP
