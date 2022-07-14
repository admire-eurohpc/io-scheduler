#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

#define NINPUTS  10
#define NOUTPUTS 5

int
main(int argc, char* argv[]) {

    if(argc != 5) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr,
                "Usage: ADM_define_data_operation <REMOTE_IP> <PATH> <OPERATION_ID> <ARGUMENTS>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;

    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_job_t job;
    ADM_data_operation_t op_handle;
    const char* path = "/tmpxxxxx";
    va_list args; // FIXME placeholder

    ADM_return_t ret = ADM_define_data_operation(server, job, path, &op_handle);


    if(ret != ADM_SUCCESS) {
        fprintf(stdout,
                "ADM_define_data_operation() remote procedure not completed "
                "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_define_data_operation() remote procedure completed "
                    "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}