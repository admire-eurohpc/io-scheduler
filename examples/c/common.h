#ifndef SCORD_C_EXAMPLES_COMMON_H
#define SCORD_C_EXAMPLES_COMMON_H

#include <scord/types.h>

#define TESTNAME                                                               \
    (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1   \
                                      : __FILE__)

typedef struct {
    const char* name;
    bool requires_server;
    bool requires_controller;
    bool requires_data_stager;
} test_info_t;

typedef struct {
    const char* server_address;
    const char* controller_address;
    const char* data_stager_address;
} cli_args_t;

int
process_args(int argc, char* argv[], test_info_t test_info, cli_args_t* args);

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
