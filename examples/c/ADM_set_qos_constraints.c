#include <stdlib.h>
#include <stdio.h>
#include <admire.h>

int
main(int argc, char* argv[]) {

    if(argc != 6) {
        fprintf(stderr, "ERROR: no location provided\n");
        fprintf(stderr,
                "Usage: ADM_set_qos_constraints <REMOTE_IP> <SCOPE> <QOS_CLASS> <ELEMENT_ID> <CLASS_VALUE>\n");
        exit(EXIT_FAILURE);
    }

    int exit_status = EXIT_SUCCESS;
    ADM_server_t server = ADM_server_create("tcp", argv[1]);

    ADM_job_t job;
    ADM_qos_entity_t entity;
    ADM_qos_limit_t limit;
    ADM_return_t ret = ADM_set_qos_constraints(server, job, entity, limit);

    if(ret != ADM_SUCCESS) {
        fprintf(stdout,
                "ADM_set_qos_constraints() remote procedure not completed "
                "successfully\n");
        exit_status = EXIT_FAILURE;
        goto cleanup;
    }

    fprintf(stdout, "ADM_set_qos_constraints() remote procedure completed "
                    "successfully\n");

cleanup:

    ADM_server_destroy(server);
    exit(exit_status);
}
