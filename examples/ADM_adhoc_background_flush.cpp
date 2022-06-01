#include <fmt/format.h>
#include <engine.hpp>

bool
string_to_convert(std::string s) {
    if(s == "true" || s == "TRUE" || s == "True") {
        return true;
    } else if(s == "false" || s == "FALSE" || s == "False") {
        return false;
    } else {
        throw std::invalid_argument("ERROR: Incorrect input value. Please try again.\n");
    }
}

int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_adhoc_background_flush <REMOTE_IP> <TRUE_OR_FALSE>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_adhoc_background_flush remote procedure on {} -> flush true/false: {} ...\n",
            argv[1], argv[2]);
    ADM_adhoc_background_flush_in_t in;

    try {
        in.b_flush = string_to_convert(argv[2]);
    } catch(const std::invalid_argument& ia) {
        fmt::print(
            stderr,
            "ERROR: Incorrect input value. Please introduce TRUE/FALSE value. \n");
        exit(EXIT_FAILURE);
    }

    ADM_adhoc_background_flush_out_t out;

    endp.call("ADM_adhoc_background_flush", &in, &out);

    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_adhoc_background_flush remote procedure not completed successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(
                stdout,
                "ADM_adhoc_background_flush remote procedure completed successfully\n");
    }
}