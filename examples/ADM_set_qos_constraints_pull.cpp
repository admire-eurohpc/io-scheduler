#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 4) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_set_qos_constraints_pull <REMOTE_IP> <SCOPE> <ELEMENT_ID>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_set_qos_constraints_pull remote procedure on {} with scope {} and element id {} ...\n",
            argv[1], argv[2], argv[3]);
    ADM_set_qos_constraints_pull_in_t in;
    in.scope = argv[2];
    try {
        in.element_id = std::stoi(argv[3]);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }

    ADM_set_qos_constraints_pull_out_t out;

    endp.call("ADM_set_qos_constraints_pull", &in, &out);


    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_set_qos_constraints_pull remote procedure not completed successfully\n");
    } else {
        fmt::print(
                stdout,
                "ADM_set_qos_constraints_pull remote procedure completed successfully\n");
    }
}
