#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_update_job <REMOTE_IP> <JOB_REQS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_job_requirements_t reqs{};
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::update_job(server, job, reqs);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_update_job() failed: {}\n", e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout, "ADM_update_job() remote procedure not completed "
                           "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_update_job() remote procedure completed "
                       "successfully\n");
}
