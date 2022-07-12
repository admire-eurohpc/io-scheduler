#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 5) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_set_io_resources <REMOTE_IP> <TIER_ID> <RESOURCES> <JOB_ID>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_storage_t tier{};
    ADM_storage_resources_t resources{};
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::set_io_resources(server, job, tier, resources);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_set_io_resources() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_set_io_resources() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_set_io_resources() remote procedure completed "
                       "successfully\n");
}
