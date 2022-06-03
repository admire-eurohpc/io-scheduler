#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 4) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_get_qos_constraints <REMOTE_IP> <SCOPE> <ELEMENT_ID>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_get_qos_constraints remote procedure on {} with scope {} and element id {} ...\n",
            argv[1], argv[2], argv[3]);
    ADM_get_qos_constraints_in_t in;
    in.scope = argv[2];
    try {
        in.element_id = std::stoi(argv[3]);
    } catch(const std::exception& e) {
        fmt::print(stderr, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }

    ADM_get_qos_constraints_out_t out;

    endp.call("ADM_get_qos_constraints", &in, &out);

    if(out.ret < 0) {
        fmt::print(
                stderr,
                "ADM_get_qos_constraints remote procedure not completed successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(
                stdout,
                "ADM_get_qos_constraints remote procedure completed successfully\n");
    }
}
