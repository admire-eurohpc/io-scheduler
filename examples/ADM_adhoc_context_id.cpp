#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_adhoc_context_id <REMOTE_IP> <CONTEXT_ID>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(stdout, "Calling ADM_adhoc_context_id remote procedure on {} -> access method: {} ...\n", argv[1], argv[2]);
    ADM_adhoc_context_id_in_t in;

    try {
        in.context_id = stoi(argv[2]);
    } catch (const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }

    ADM_adhoc_context_id_out_t out;

    endp.call("ADM_adhoc_context_id",&in, &out);

    if (out.ret == true){
        fmt::print(stdout, "ADM_adhoc_context_id remote procedure completed successfully\n");
    }else{
        fmt::print(stdout, "ADM_adhoc_context_id remote procedure not completed successfully\n");
    }
}