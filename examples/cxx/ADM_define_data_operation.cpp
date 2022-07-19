#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr,
                   "Usage: ADM_define_data_operation <SERVER_ADDRESS> \n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    const char* path = "/tmpxxxxx";
    ADM_data_operation_t op;
    va_list args; // FIXME: placeholder
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::define_data_operation(server, job, path, &op, args);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_define_data_operation() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_define_data_operation() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_define_data_operation() remote procedure completed "
                       "successfully\n");
}
