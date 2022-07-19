#include "fmt/format.h"
#include "admire.hpp"


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_remove_pfs_storage <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_storage_t pfs_storage{};
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::remove_pfs_storage(server, job, pfs_storage);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_remove_pfs_storage() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_remove_pfs_storage() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_remove_pfs_storage() remote procedure completed "
                       "successfully\n");
}
