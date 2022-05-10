#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 5) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_set_io_resources <REMOTE_IP> <TIER_ID> <RESOURCES> <JOB_ID>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();

    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_set_io_resources remote procedure on {} with tier id {}, resources {} and"
            " job id {} ...\n",
            argv[1], argv[2], argv[3], argv[4]);
    ADM_set_io_resources_in_t in;
    try {
        in.tier_id = std::stoi(argv[2]);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    in.resources = argv[3];
    try {
        in.job_id = std::stoi(argv[4]);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    ADM_set_io_resources_out_t out;

    endp.call("ADM_set_io_resources", &in, &out);


    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_set_io_resources remote procedure not completed successfully\n");
    } else {
        fmt::print(
                stdout,
                "ADM_set_io_resources remote procedure completed successfully\n");
    }
}
