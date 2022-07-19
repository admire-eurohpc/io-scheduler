#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr,
                "Usage: ADM_link_transfer_to_data_operation <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_job_t job;
    ADM_data_operation_t op;
    ADM_transfer_t transfer;
    bool should_stream = false;
    va_list args;
    ADM_return_t ret = ADM_link_transfer_to_data_operation(
            server, job, op, transfer, should_stream, args);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout,
                "ADM_link_transfer_to_data_operation() remote procedure not completed "
                "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout,
            "ADM_link_transfer_to_data_operation() remote procedure completed "
            "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}