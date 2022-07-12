#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

int
main(int argc, char* argv[]) {

    if(argc != 4) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr,
                "Usage: ADM_set_transfer_priority <REMOTE_IP> <TRANSFER_ID> <N_POSITIONS>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_job_t job;
    ADM_transfer_t tx_handle;
    int incr = 42;
    ADM_return_t ret = ADM_set_transfer_priority(server, job,
                          tx_handle, incr);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout,
                "ADM_set_transfer_priority() remote procedure not completed "
                "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_set_transfer_priority() remote procedure completed "
                    "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}