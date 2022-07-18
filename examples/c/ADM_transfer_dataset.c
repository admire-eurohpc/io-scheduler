#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr, "Usage: ADM_transfer_dataset <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_job_t job;
    ADM_dataset_t** sources = NULL;
    ADM_dataset_t** targets = NULL;
    ADM_qos_limit_t** limits = NULL;
    ADM_transfer_mapping_t mapping = ADM_MAPPING_ONE_TO_ONE;
    ADM_transfer_t tx_handle;
    ADM_return_t ret = ADM_transfer_dataset(server, job, sources, targets,
                                            limits, mapping, &tx_handle);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout, "ADM_transfer_dataset() remote procedure not completed "
                        "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_transfer_dataset() remote procedure completed "
                    "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}