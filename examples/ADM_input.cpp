#include <fmt/format.h>
#include <engine.hpp>

int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_input <ORIGIN_LOCATION> <TARGET_LOCATION>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1], argv[2]);

    fmt::print(stdout, "Calling ADM_input remote procedure on {} -> {}...\n", argv[1], argv[2]);
    endp.call("ADM_input");

    fmt::print(stdout, "ADM_input remote procedure completed successfully\n");
}