#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 4) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_get_qos_constraints <REMOTE_IP> <SCOPE> <ELEMENT_ID>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    ADM_job_handle_t job{};
    ADM_qos_scope_t scope{};
    ADM_qos_entity_t entity{};
    ADM_limit_t* limits;
    ADM_return_t ret = ADM_SUCCESS;

    try {
        ret = admire::get_qos_constraints(server, job, scope, entity, &limits);
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
}
