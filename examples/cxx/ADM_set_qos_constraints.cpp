#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 6) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_set_qos_constraints <REMOTE_IP> <SCOPE> <QOS_CLASS> <ELEMENT_ID> <CLASS_VALUE>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_t job{};
    ADM_qos_limit_t limit{};
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::set_qos_constraints(server, job, limit);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_set_qos_constraints() failed: {}\n",
                   e.what());
        exit(EXIT_FAILURE);
    }

    if(ret != ADM_SUCCESS) {
        fmt::print(stdout,
                   "ADM_set_qos_constraints() remote procedure not completed "
                   "successfully\n");
        exit(EXIT_FAILURE);
    }

    fmt::print(stdout, "ADM_set_qos_constraints() remote procedure completed "
                       "successfully\n");
}
