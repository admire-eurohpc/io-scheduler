#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no server address provided\n");
        fmt::print(stderr, "Usage: ADM_remove_job <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    admire::job job{42};

    try {
        [[maybe_unused]] const auto ret = admire::remove_job(server, job);
        fmt::print(stdout, "ADM_remove_job() remote procedure completed "
                           "successfully\n");
        exit(EXIT_SUCCESS);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ADM_remove_job() remote procedure not completed "
                           "successfully\n");
        exit(EXIT_FAILURE);
    }
}
