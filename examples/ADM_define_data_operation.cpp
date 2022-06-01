#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 5) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_define_data_operation <REMOTE_IP> <PATH> <OPERATION_ID> <ARGUMENTS> \n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_define_data_operation remote procedure on {} -> {} with operation id {} and arguments {} ...\n",
            argv[1], argv[2], argv[3], argv[4]);

    ADM_define_data_operation_in_t in;
    in.path = argv[2];
    try {
        in.operation_id = std::stoi(argv[3]);
    } catch(const std::exception& e) {
        fmt::print(stderr, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    in.arguments = argv[4];

    ADM_define_data_operation_out_t out;

    endp.call("ADM_define_data_operation", &in, &out);


    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_define_data_operation remote procedure not completed successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(
                stdout,
                "ADM_define_data_operation remote procedure completed successfully\n");
    }
}
