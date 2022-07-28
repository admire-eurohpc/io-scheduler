#include <fmt/format.h>
#include <admire.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_register_adhoc_storage <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};
    admire::job job{42};
    admire::adhoc_storage::context ctx{"in_job:shared", "write-only", 10, 10, false};
    admire::adhoc_storage adhoc_storage{"read-only", 10, ctx};

    try {

        [[maybe_unused]] const auto ret = admire::register_adhoc_storage(server, job, ctx, &adhoc_storage);

        // do something with job. name changed to ret to not create conflict.
        // TODO: name changed from job to ret to not create conflict.

        fmt::print(stdout, "ADM_register_adhoc_storage() remote procedure completed "
                           "successfully\n");
        exit(EXIT_SUCCESS);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_register_adhoc_storage() failed: {}\n", e.what());
        exit(EXIT_FAILURE);
    }
}
