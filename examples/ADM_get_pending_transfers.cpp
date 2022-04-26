#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_get_pending_transfers <REMOTE_IP>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(stdout,
               "Calling ADM_get_pending_transfers remote procedure on {} ...\n",
               argv[1]);
    ADM_get_pending_transfers_in_t in;
    in.value = NULL;
    ADM_get_pending_transfers_out_t out;

    endp.call("ADM_get_pending_transfers", &in, &out);


    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_get_pending_transfers remote procedure not completed successfully\n");
    } else {
        fmt::print(
                stdout,
                "ADM_get_pending_transfers remote procedure completed successfully\n");
    }
}