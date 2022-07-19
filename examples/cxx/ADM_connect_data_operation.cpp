#include <fmt/format.h>
#include <admire.hpp>

bool
string_to_convert(std::string s) {
    if(s == "true" || s == "TRUE" || s == "True") {
        return true;
    } else if(s == "false" || s == "FALSE" || s == "False") {
        return false;
    } else {
        throw std::invalid_argument(
                "ERROR: Incorrect input value. Please try again.\n");
    }
}

int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr,
                   "Usage: ADM_connect_data_operation <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_dataset_t input{};
    ADM_dataset_t output{};
    bool should_stream = false;
    va_list args; // FIXME placeholder
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::connect_data_operation(server, job, input, output,
                                             should_stream, args);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_connect_data_operation() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(
                stdout,
                "ADM_connect_data_operation() remote procedure not completed "
                "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout,
               "ADM_connect_data_operation() remote procedure completed "
               "successfully\n");
}
