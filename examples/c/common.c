#ifndef SCORD_COMMON_H
#define SCORD_COMMON_H

#include <stdio.h>
#include <string.h>
#include "common.h"

int
process_args(int argc, char* argv[], test_info_t test_info, cli_args_t* args) {

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
        fprintf(stderr, "ERROR: missing arguments\n");
        fprintf(stderr, "Usage: %s%s%s%s\n", test_info.name,
                test_info.requires_server ? " <SERVER_ADDRESS>" : "",
                test_info.requires_controller ? " <CONTROLLER_ADDRESS>" : "",
                test_info.requires_data_stager ? " <DATA_STAGER_ADDRESS>" : "");
        return -1;
    }

    args->server_address = test_info.requires_server ? argv[1] : NULL;
    args->controller_address = test_info.requires_controller ? argv[2] : NULL;

    return 0;
}


ADM_node_t*
prepare_nodes(size_t n) {

    ADM_node_t* nodes = calloc(n, sizeof(ADM_node_t));

    if(!nodes) {
        return NULL;
    }

    for(size_t i = 0; i < n; ++i) {
        size_t len = snprintf(NULL, 0, "node-%02zu", i);
        char* id = (char*) alloca(len + 1);
        snprintf(id, len + 1, "node-%02zu", i);
        nodes[i] = ADM_node_create(id, ADM_NODE_REGULAR);
        if(!nodes[i]) {
            return NULL;
        }
    }

    return nodes;
}

void
destroy_nodes(ADM_node_t nodes[], size_t n) {

    if(!nodes) {
        return;
    }

    for(size_t i = 0; i < n; ++i) {
        if(nodes[i]) {
            ADM_node_destroy(nodes[i]);
        }
    }

    free(nodes);
}

ADM_dataset_t*
prepare_datasets(const char* pattern, size_t n) {

    ADM_dataset_t* datasets = calloc(n, sizeof(ADM_dataset_t));

    if(!datasets) {
        return NULL;
    }

    for(size_t i = 0; i < n; ++i) {
        size_t len = snprintf(NULL, 0, pattern, i);
        char* id = (char*) alloca(len + 1);
        snprintf(id, len + 1, pattern, i);
        datasets[i] = ADM_dataset_create(id);
        if(!datasets[i]) {
            return NULL;
        }
    }

    return datasets;
}

void
destroy_datasets(ADM_dataset_t datasets[], size_t n) {

    if(!datasets) {
        return;
    }

    for(size_t i = 0; i < n; ++i) {
        if(datasets[i]) {
            ADM_dataset_destroy(datasets[i]);
        }
    }

    free(datasets);
}

ADM_qos_limit_t*
prepare_qos_limits(size_t n) {

    ADM_qos_limit_t* limits = calloc(n, sizeof(ADM_qos_limit_t));

    if(!limits) {
        return NULL;
    }

    for(size_t i = 0; i < n; ++i) {
        limits[i] = ADM_qos_limit_create(NULL, ADM_QOS_CLASS_BANDWIDTH, 50);
        if(!limits[i]) {
            return NULL;
        }
    }

    return limits;
}

void
destroy_qos_limits(ADM_qos_limit_t* limits, size_t n) {

    if(!limits) {
        return;
    }

    for(size_t i = 0; i < n; ++i) {
        if(limits[i]) {
            ADM_qos_limit_destroy(limits[i]);
        }
    }

    free(limits);
}


#endif // SCORD_COMMON_H
