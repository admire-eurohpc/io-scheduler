#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

int
main(int argc, char* argv[]) {

        if(argc != 3) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr, "Usage: ADM_deploy_adhoc_storage <REMOTE_IP> "
                           "<JOB_REQS>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_job_t job;
    ADM_storage_t adhoc_storage;

    ADM_return_t ret = ADM_deploy_adhoc_storage(server, job,
                         adhoc_storage);


    if(ret != ADM_SUCCESS) {
        fprintf(stdout, "ADM_deploy_adhoc_storage() remote procedure not completed "
                        "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_deploy_adhoc_storage() remote procedure completed "
                    "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}