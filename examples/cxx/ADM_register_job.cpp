#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_register_job <REMOTE_IP> <JOB_REQS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_requirements_t reqs{};

    try {

        [[maybe_unused]] const auto job = admire::register_job(server, reqs);

        // do something with job

        fmt::print(stdout, "ADM_register_job() remote procedure completed "
                           "successfully\n");
        exit(EXIT_SUCCESS);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_register_job() failed: {}\n", e.what());
        exit(EXIT_FAILURE);
    }
}
