#include <fmt/format.h>
#include <engine.hpp>


int
main(int argc, char* argv[]) {

    if(argc != 7) {
        fmt::print(stderr, "ERROR: no location provided\n");
        fmt::print(
                stderr,
                "Usage: ADM_transfer_dataset <REMOTE_IP> <SOURCE_LOCATION> <DESTINATION_LOCATION>"
                "<QOS_CONSTRAINTS> <DISTRIBUTION> <JOB_ID>\n");
        exit(EXIT_FAILURE);
    }

    scord::network::rpc_client rpc_client{"tcp"};
    rpc_client.register_rpcs();


    auto endp = rpc_client.lookup(argv[1]);

    fmt::print(
            stdout,
            "Calling ADM_transfer_dataset remote procedure on {} : {} -> {} using "
            " qos constraints {}, distribution {} and job id {} ...\n",
            argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
    ADM_transfer_dataset_in_t in;
    in.source = argv[2];
    in.destination = argv[3];
    in.qos_constraints = argv[4];
    in.distribution = argv[5];
    try {
        in.job_id = std::stoi(argv[6]);
    } catch(const std::exception& e) {
        fmt::print(stdout, "ERROR: Incorrect input type. Please try again.\n");
        exit(EXIT_FAILURE);
    }
    ADM_transfer_dataset_out_t out;

    endp.call("ADM_transfer_dataset", &in, &out);


    if(out.ret < 0) {
        fmt::print(
                stdout,
                "ADM_transfer_dataset remote procedure not completed successfully\n");
    } else {
        fmt::print(
                stdout,
                "ADM_transfer_dataset remote procedure completed successfully\n");
    }
}
