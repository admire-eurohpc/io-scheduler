#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr,
                   "Usage: ADM_adhoc_context <REMOTE_IP> <EXECUTION_MODE>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_adhoc_context remote procedure on {} -> access method: {} ...\n",
            argv[1], argv[2]);
    ADM_adhoc_context_in_t in;
    in.context = argv[2];
    ADM_adhoc_context_out_t out;

    endp.call("ADM_adhoc_context", &in, &out);

    if(out.ret < 0 || out.adhoc_context < 0) {
        fmt::print(
                stdout,
                "ADM_adhoc_context remote procedure not completed successfully\n");
    } else {
        fmt::print(
                stdout,
                "ADM_adhoc_context remote procedure completed successfully\n");
    }
}