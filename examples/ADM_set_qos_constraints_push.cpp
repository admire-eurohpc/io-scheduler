#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 6) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_set_qos_constraints_push <REMOTE_IP> <SCOPE> <QOS_CLASS> <ELEMENT_ID> <CLASS_VALUE>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_set_qos_constraints_push remote procedure on {} with scope {}, QoS class {}, element id {} and class value {} ...\n",
            argv[1], argv[2], argv[3], argv[4], argv[5]);
    ADM_set_qos_constraints_push_in_t in;
    in.scope = argv[2];
    in.qos_class = argv[3];
    try {
        in.element_id = std::stoi(argv[4]);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    in.class_value = argv[4];

    ADM_set_qos_constraints_push_out_t out;

    endp.call("ADM_set_qos_constraints_push", &in, &out);


    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_set_qos_constraints_push remote procedure not completed successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(
                stdout,
                "ADM_set_qos_constraints_push remote procedure completed successfully\n");
    }
}
