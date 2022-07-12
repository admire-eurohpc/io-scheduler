#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

int
main(int argc, char* argv[]) {

    if(argc != 5) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr,
                "Usage: ADM_set_io_resources <REMOTE_IP> <TIER_ID> <RESOURCES> <JOB_ID>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_job_t job;
    ADM_storage_t tier;
    ADM_storage_resources_t resources;
    ADM_return_t ret = ADM_set_io_resources(server, job, tier, resources);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout, "ADM_set_io_resources() remote procedure not completed "
                        "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_set_io_resources() remote procedure completed "
                    "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}