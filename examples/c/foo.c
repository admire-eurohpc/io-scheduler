#include <admire.h>

#define LOG_ERROR(mid, msg)

int
main() {
    ADM_server_t scord_server =
            ADM_server_create("tcp", "ofi+tcp://127.0.0.1:52000");
    if(!scord_server) {
        LOG_ERROR(mid, "Could not initialize Scord server");
    }

    ADM_adhoc_context_t adhoc_ctx;
    adhoc_ctx.c_mode = ADM_ADHOC_MODE_IN_JOB_SHARED;
    adhoc_ctx.c_access = ADM_ADHOC_ACCESS_RDWR;
    adhoc_ctx.c_nodes = 3;
    adhoc_ctx.c_walltime = 3600;
    adhoc_ctx.c_should_bg_flush = 0;

    /* no inputs or outputs */
    ADM_job_requirements_t scord_reqs;
    scord_reqs = ADM_job_requirements_create(NULL, 0, NULL, 0, &adhoc_ctx);
    if(!scord_reqs) {
        LOG_ERROR(mid, "Could not initialize Scord job requirements");
    }

    ADM_job_t scord_job;
    ADM_register_job(scord_server, scord_reqs, &scord_job);

    ADM_job_requirements_destroy(scord_reqs);
    ADM_server_destroy(scord_server);
}
