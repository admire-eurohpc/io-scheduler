#include "common.hpp"

std::vector<admire::dataset>
prepare_datasets(const std::string& pattern, size_t n) {
    std::vector<admire::dataset> datasets;
    datasets.reserve(n);
    for(size_t i = 0; i < n; ++i) {
        datasets.emplace_back(fmt::format(pattern, i));
    }

    return datasets;
}
