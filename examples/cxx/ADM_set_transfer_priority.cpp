#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr,
                   "Usage: ADM_set_transfer_priority <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_transfer_t tx{};
    int incr = 42;
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::set_transfer_priority(server, job, tx, incr);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_set_transfer_priority() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_set_transfer_priority() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_set_transfer_priority() remote procedure completed "
                       "successfully\n");
}
