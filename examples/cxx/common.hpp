#ifndef SCORD_CXX_EXAMPLES_COMMON_HPP
#define SCORD_CXX_EXAMPLES_COMMON_HPP

#include <vector>
#include <scord/types.hpp>

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

std::vector<scord::qos::limit>
prepare_qos_limits(size_t n);

#endif // SCORD_CXX_EXAMPLES_COMMON_HPP
