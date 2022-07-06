#include <fmt/format.h>
#include <admire.hpp>

int
main(int argc, char* argv[]) {

    if(argc != 7) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_link_transfer_to_data_operation <REMOTE_IP> <OPERATION_ID> <TRANSFER_ID> <STREAM> <ARGUMENTS> <JOB_ID>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_data_operation_t op_handle;
    bool should_stream = false;
    va_list args;
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::link_transfer_to_data_operation(server, job, op_handle,
                                                      should_stream, args);
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
