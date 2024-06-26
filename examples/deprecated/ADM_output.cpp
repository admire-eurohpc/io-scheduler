#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 4) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_output <REMOTE_IP> <ORIGIN_LOCATION> <TARGET_LOCATION>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();


    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(stdout,
               "Calling ADM_output remote procedure on {} : {} -> {} ...\n",
               argv[1], argv[2], argv[3]);
    ADM_output_in_t in;
    in.origin = argv[2];
    in.target = argv[3];
    ADM_output_out_t out;

    endp.call("ADM_output", &in, &out);

    if(out.ret < 0) {
        fmt::print(stdout,
                   "ADM_output remote procedure not completed successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(stdout,
                   "ADM_output remote procedure completed successfully\n");
    }
}
