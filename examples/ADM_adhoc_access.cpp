#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_adhoc_access <REMOTE_IP> <ACCES_METHOD>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(stdout, "Calling ADM_adhoc_access remote procedure on {} -> access method: {} ...\n", argv[1], argv[2]);
    ADM_adhoc_access_in_t in;
    in.access = argv[2];
    ADM_adhoc_access_out_t out;

    endp.call("ADM_adhoc_access",&in, &out);

    if (out.ret == true){
        fmt::print(stdout, "ADM_output remote procedure completed successfully\n");
    }else{
        fmt::print(stdout, "ADM_output remote procedure not completed successfully\n");
    }
}