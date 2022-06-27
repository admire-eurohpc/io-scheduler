#include <fmt/format.h>
#include <admire.hpp>

int
main(int argc, char* argv[]) {

    if(argc != 4) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_get_statistics <REMOTE_IP> <JOB_ID> <JOB_STEP> \n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_job_stats_t* stats = nullptr;
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::get_statistics(server, job, &stats);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_cancel_transfer() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_cancel_transfer() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_cancel_transfer() remote procedure completed "
                       "successfully\n");
}
