#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 3) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_get_transfer_priority <REMOTE_IP> <TRANSFER_ID>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_get_transfer_priority remote procedure on {} with transfer id {} ...\n",
            argv[1], argv[2]);
    ADM_get_transfer_priority_in_t in;
    try {
        in.transfer_id = std::stoi(argv[2]);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    ADM_get_transfer_priority_out_t out;

    endp.call("ADM_get_transfer_priority", &in, &out);


    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_get_transfer_priority remote procedure not completed successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(
                stdout,
                "ADM_get_transfer_priority remote procedure completed successfully\n");
    }
}
