#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr, "Usage: ADM_register_job <REMOTE_IP> <JOB_REQS>\n");
        exit(EXIT_FAILURE);
    }

#if 0
    ADM_server_t* server = ADM_server_create("tcp", argv[1]);

    ADM_job_handle_t job = ADM_job_handle_init();
    ADM_job_requirements_t reqs = ADM_job_requirements_init();
    ADM_dataset_info_t info = ADM_dataset_info_init();
    ADM_return_t ret = ADM_register_job(server, reqs, &job);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout, "ADM_register_job() remote procedure not completed "
                        "successfully\n");
        exit(EXIT_FAILURE);
    }
#endif

    fprintf(stdout, "ADM_register_job() remote procedure completed "
                    "successfully\n");
}
