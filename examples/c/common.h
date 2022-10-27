#ifndef SCORD_C_EXAMPLES_COMMON_H
#define SCORD_C_EXAMPLES_COMMON_H

#include <admire_types.h>

ADM_node_t*
prepare_nodes(size_t n);

void
destroy_nodes(ADM_node_t nodes[], size_t n);

ADM_dataset_t*
prepare_datasets(const char* pattern, size_t n);

void
destroy_datasets(ADM_dataset_t datasets[], size_t n);

ADM_qos_limit_t*
prepare_qos_limits(size_t n);

void
destroy_qos_limits(ADM_qos_limit_t limits[], size_t n);

#endif // SCORD_C_EXAMPLES_COMMON_H
