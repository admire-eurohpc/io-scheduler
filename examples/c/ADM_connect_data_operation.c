#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

#define NINPUTS  10
#define NOUTPUTS 5

int
main(int argc, char* argv[]) {

    if(argc != 7) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr,
                "Usage: ADM_connect_data_operation <REMOTE_IP> <OPERATION_ID> <INPUT> <STREAM> <ARGUMENTS> <JOB_ID>\n");
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

    bool should_stream = false;
    va_list args; // FIXME placeholder
    ADM_return_t ret = ADM_connect_data_operation(server, job, inputs, outputs,
                                                  should_stream);


    if(ret != ADM_SUCCESS) {
        fprintf(stdout,
                "ADM_connect_data_operation() remote procedure not completed "
                "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_connect_data_operation() remote procedure completed "
                    "successfully\n");

cleanup:

    for(int i = 0; i < NINPUTS; ++i) {
        ADM_dataset_destroy(inputs[i]);
    }

    for(int i = 0; i < NOUTPUTS; ++i) {
        ADM_dataset_destroy(outputs[i]);
    }

    ADM_server_destroy(server);
    exit(exit_status);
}