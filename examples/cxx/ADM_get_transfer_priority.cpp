#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_get_transfer_priority <REMOTE_IP> <TRANSFER_ID>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_transfer_t tx{};
    ADM_transfer_priority_t priority;
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::get_transfer_priority(server, job, tx, &priority);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_get_transfer_priority() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_get_transfer_priority() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_get_transfer_priority() remote procedure completed "
                       "successfully\n");
}
