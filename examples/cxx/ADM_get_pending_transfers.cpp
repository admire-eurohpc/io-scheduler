#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_get_pending_transfers <REMOTE_IP>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_handle_t job{};
    ADM_transfer_handle_t** tx_handles = nullptr;
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::get_pending_transfers(server, job, tx_handles);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_get_pending_transfers() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_get_pending_transfers() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_get_pending_transfers() remote procedure completed "
                       "successfully\n");
}
