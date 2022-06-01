#include <fmt/format.h>
#include <engine.hpp>

bool
string_to_convert(std::string s) {
    if(s == "true" || s == "TRUE" || s == "True") {
        return true;
    } else if(s == "false" || s == "FALSE" || s == "False") {
        return false;
    } else {
        throw std::invalid_argument(
                "ERROR: Incorrect input value. Please try again.\n");
    }
}

int
main(int argc, char* argv[]) {

    if(argc != 7) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_connect_data_operation <REMOTE_IP> <OPERATION_ID> <INPUT> <STREAM> <ARGUMENTS> <JOB_ID>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_connect_data_operation remote procedure on {} with operation id {}, input {}, stream {}, arguments {} and job id {} ...\n",
            argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
    ADM_connect_data_operation_in_t in;
    try {
        in.operation_id = std::stoi(argv[2]);
    } catch(const std::exception& e) {
        fmt::print(stderr, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    in.input = argv[3];
    try {
        in.stream = string_to_convert(argv[4]);
    } catch(const std::invalid_argument& ia) {
        fmt::print(stderr, "ERROR: Incorrect input value. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    in.arguments = argv[5];
    try {
        in.job_id = std::stoi(argv[6]);
    } catch(const std::exception& e) {
        fmt::print(
                stderr,
                "ERROR: ERROR: Incorrect input value. Please introduce TRUE/FALSE value. \n");
        exit(EXIT_FAILURE);
    }

    ADM_connect_data_operation_out_t out;

    endp.call("ADM_connect_data_operation", &in, &out);


    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_connect_data_operation remote procedure not completed successfully\n");
        exit(EXIT_FAILURE);
    } else {
        fmt::print(
                stdout,
                "ADM_connect_data_operation remote procedure completed successfully\n");
    }
}
