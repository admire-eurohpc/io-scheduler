#include "common.hpp"

std::vector<admire::node>
prepare_nodes(size_t n) {
    std::vector<admire::node> nodes;
    nodes.reserve(n);
    for(size_t i = 0; i < n; ++i) {
        nodes.emplace_back(fmt::format("node-{:02d}", i));
    }

    return nodes;
}

std::vector<admire::dataset>
prepare_datasets(const std::string& pattern, size_t n) {
    std::vector<admire::dataset> datasets;
    datasets.reserve(n);
    for(size_t i = 0; i < n; ++i) {
        datasets.emplace_back(fmt::format(fmt::runtime(pattern), i));
    }

    return datasets;
}

std::vector<admire::qos::limit>
prepare_qos_limits(size_t n) {

    std::vector<admire::qos::limit> limits;
    limits.reserve(n);

    for(size_t i = 0; i < n; ++i) {
        limits.emplace_back(admire::qos::subclass::bandwidth, 50);
    }

    return limits;
}
