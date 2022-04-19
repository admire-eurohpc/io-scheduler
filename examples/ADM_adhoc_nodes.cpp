#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_adhoc_nodes <REMOTE_IP> <NUMBER_OF_NODES>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(stdout, "Calling ADM_adhoc_nodes remote procedure on {} -> node numbers: {} ...\n", argv[1], argv[2]);
    ADM_adhoc_nodes_in_t in;
    in.nodes = argv[2];
    ADM_adhoc_nodes_out_t out;

    endp.call("ADM_adhoc_nodes",&in, &out);

    if (out.ret == true){
        fmt::print(stdout, "ADM_output remote procedure completed successfully\n");
    }else{
        fmt::print(stdout, "ADM_output remote procedure not completed successfully\n");
    }
}