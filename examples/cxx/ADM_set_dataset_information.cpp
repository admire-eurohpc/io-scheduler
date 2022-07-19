#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr,
                   "Usage: ADM_set_dataset_information <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_dataset_t target{};
    ADM_dataset_info_t info{};
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::set_dataset_information(server, job, target, info);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_set_dataset_information() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(
                stdout,
                "ADM_set_dataset_information() remote procedure not completed "
                "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout,
               "ADM_set_dataset_information() remote procedure completed "
               "successfully\n");
}
