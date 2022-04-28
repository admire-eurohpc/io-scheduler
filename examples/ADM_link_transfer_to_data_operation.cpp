#include <fmt/format.h>
#include <engine.hpp>

int
String2Convert(std::string var) {
    if(var == "true" || var == "TRUE") {
        return 0;
    } else if(var == "false" || var == "FALSE") {
        return 1;
    } else {
        return -1;
    }
}

int
main(int argc, char* argv[]) {

    if(argc != 7) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_link_transfer_to_data_operation <REMOTE_IP> <OPERATION_ID> <TRANSFER_ID> <STREAM> <ARGUMENTS> <JOB_ID>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_link_transfer_to_data_operation remote procedure on {} with operation id {}, transfer id {}, stream {}, arguments {} and job id {} ...\n",
            argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
    ADM_link_transfer_to_data_operation_in_t in;
    try {
        in.operation_id = std::stoi(argv[2]);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    try {
        in.transfer_id = std::stoi(argv[3]);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    try {
        in.stream = String2Convert(argv[4]);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input value. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    in.arguments = argv[5];
    try {
        in.job_id = std::stoi(argv[6]);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }

    ADM_link_transfer_to_data_operation_out_t out;

    endp.call("ADM_link_transfer_to_data_operation", &in, &out);


    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_link_transfer_to_data_operation remote procedure not completed successfully\n");
    } else {
        fmt::print(
                stdout,
                "ADM_link_transfer_to_data_operation remote procedure completed successfully\n");
    }
}