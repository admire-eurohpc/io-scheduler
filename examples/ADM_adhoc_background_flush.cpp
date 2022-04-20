#include <fmt/format.h>
#include <engine.hpp>

bool 
String2Convert(std::string var){   
    if(var == "true" || var = "TRUE")      
        return true;   
    else if(var == "false" || var = "FALSE")      
        return false;    
    }else{
        exit(EXIT_FAILURE);
    }

int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(stderr, "Usage: ADM_adhoc_background_flush <REMOTE_IP> <TRUE_OR_FALSE>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(stdout, "Calling ADM_adhoc_background_flush remote procedure on {} -> node numbers: {} ...\n", argv[1], argv[2]);
    ADM_adhoc_background_flush_in_t in;

    try {
        in.b_flush = String2Convert(argv[2]);
    } catch (const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    
    ADM_adhoc_background_flush_out_t out;

    endp.call("ADM_adhoc_background_flush",&in, &out);

    if (out.ret == true){
        fmt::print(stdout, "ADM_adhoc_background_flush remote procedure completed successfully\n");
    }else{
        fmt::print(stdout, "ADM_adhoc_background_flush remote procedure not completed successfully\n");
    }
}