#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

int
main(int argc, char* argv[]) {

        if(argc != 4) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr, "Usage: ADM_get_statistics <REMOTE_IP> <JOB_ID> <JOB_STEP>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_job_t job;
    ADM_job_stats_t* stats = NULL;

    ADM_return_t ret = ADM_get_statistics(server, job, &stats);


    if(ret != ADM_SUCCESS) {
        fprintf(stdout, "ADM_get_statistics() remote procedure not completed "
                        "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_get_statistics() remote procedure completed "
                    "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}