#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

#define NINPUTS  10
#define NOUTPUTS 5

int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr, "Usage: ADM_update_job <REMOTE_IP> <JOB_REQS>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_job_t job;

    ADM_dataset_t inputs[NINPUTS];

    for(int i = 0; i < NINPUTS; ++i) {
        const char* pattern = "input-dataset-%d";
        size_t n = snprintf(NULL, 0, pattern, i);
        char* id = (char*) malloc(n + 1);
        snprintf(id, n+1, pattern, i);
        inputs[i] = ADM_dataset_create(id);
    }

    ADM_dataset_t outputs[NOUTPUTS];

    for(int i = 0; i < NOUTPUTS; ++i) {
        const char* pattern = "output-dataset-%d";
        size_t n = snprintf(NULL, 0, pattern, i);
        char* id = (char*) malloc(n + 1);
        snprintf(id, n+1, pattern, i);
        outputs[i] = ADM_dataset_create(id);
    }

    ADM_job_requirements_t reqs = ADM_job_requirements_create(
            inputs, NINPUTS, outputs, NOUTPUTS, NULL);
    ADM_return_t ret = ADM_update_job(server, job, reqs);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout, "ADM_update_job() remote procedure not completed "
                        "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_update_job() remote procedure completed "
                    "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}