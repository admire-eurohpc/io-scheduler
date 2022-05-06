#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 4) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_inout <REMOTE_IP> <ORIGIN_LOCATION> <TARGET_LOCATION>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();


    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(stdout,
               "Calling ADM_inout remote procedure on {} : {} -> {} ...\n",
               argv[1], argv[2], argv[3]);
    ADM_inout_in_t in;
    in.origin = argv[2];
    in.target = argv[3];
    ADM_inout_out_t out;

    endp.call("ADM_inout", &in, &out);

    if(out.ret < 0) {
        fmt::print(stdout,
                   "ADM_inout remote procedure not completed successfully\n");
    } else {
        fmt::print(stdout,
                   "ADM_inout remote procedure completed successfully\n");
    }
}
