#ifndef SCORD_C_EXAMPLES_COMMON_H
#define SCORD_C_EXAMPLES_COMMON_H

#include <admire_types.h>

ADM_dataset_t*
prepare_datasets(const char* pattern, size_t n);

void
destroy_datasets(ADM_dataset_t datasets[], size_t n);

#endif // SCORD_C_EXAMPLES_COMMON_H
