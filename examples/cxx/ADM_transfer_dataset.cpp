#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 7) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_transfer_dataset <REMOTE_IP> <SOURCE_LOCATION> <DESTINATION_LOCATION>"
                "<QOS_CONSTRAINTS> <DISTRIBUTION> <JOB_ID>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_dataset_t** sources = nullptr;
    ADM_dataset_t** targets = nullptr;
    ADM_qos_limit_t** limits = nullptr;
    ADM_tx_mapping_t mapping = ADM_MAPPING_ONE_TO_ONE;
    ADM_transfer_handle_t tx_handle{};
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::transfer_dataset(server, job, sources, targets, limits,
                                       mapping, &tx_handle);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_cancel_transfer() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_transfer_dataset() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(stdout, "ADM_transfer_dataset() remote procedure completed "
                           "successfully\n");
    }
}
