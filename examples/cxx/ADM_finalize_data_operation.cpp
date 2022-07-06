#include <fmt/format.h>
#include <admire.hpp>

int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_finalize_data_operation <REMOTE_IP> <OPERATION_ID> \n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_data_operation_t op_handle{};
    ADM_data_operation_status_t status;
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::finalize_data_operation(server, job, op_handle, &status);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_finalize_data_operation() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(
                stdout,
                "ADM_finalize_data_operation() remote procedure not completed "
                "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout,
               "ADM_finalize_data_operation() remote procedure completed "
               "successfully\n");
}
