#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr,
                   "Usage: ADM_in_transit_ops <REMOTE_IP> <IN_TRANSIT_OPS>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_in_transit_ops remote procedure on {} -> access method: {} ...\n",
            argv[1], argv[2]);
    ADM_in_transit_ops_in_t in;
    in.in_transit = argv[2];
    ADM_in_transit_ops_out_t out;

    endp.call("ADM_in_transit_ops", &in, &out);

    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_in_transit_ops remote procedure not completed successfully\n");
    } else {
        fmt::print(
                stdout,
                "ADM_in_transit_ops remote procedure completed successfully\n");
    }
}