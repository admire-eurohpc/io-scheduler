#ifndef SCORD_COMMON_H
#define SCORD_COMMON_H

#include <stdio.h>
#include "common.h"
#include "admire_types.h"

ADM_dataset_t*
prepare_datasets(const char* pattern, size_t n) {

    ADM_dataset_t* datasets = calloc(n, sizeof(ADM_dataset_t));

    if(!datasets) {
        return NULL;
    }

    for(size_t i = 0; i < n; ++i) {
        // const char* pattern = "input-dataset-%d";
        size_t len = snprintf(NULL, 0, pattern, i);
        char* id = (char*) alloca(len + 1);
        snprintf(id, len + 1, pattern, i);
        datasets[i] = ADM_dataset_create(id);
    }

    return datasets;
}

void
destroy_datasets(ADM_dataset_t datasets[], size_t n) {

    for(size_t i = 0; i < n; ++i) {
        if(datasets[i]) {
            ADM_dataset_destroy(datasets[i]);
        }
    }

    free(datasets);
}

#endif // SCORD_COMMON_H
