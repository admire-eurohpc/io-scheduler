#ifndef SCORD_CXX_EXAMPLES_COMMON_HPP
#define SCORD_CXX_EXAMPLES_COMMON_HPP

#include <vector>
#include <scord/types.hpp>

std::vector<scord::node>
prepare_nodes(size_t n);

std::vector<scord::dataset>
prepare_datasets(const std::string& pattern, size_t n);

std::vector<scord::qos::limit>
prepare_qos_limits(size_t n);

#endif // SCORD_CXX_EXAMPLES_COMMON_HPP
