#include <fmt/format.h>
#include <admire.hpp>

#define NINPUTS  10
#define NOUTPUTS 5

int
main(int argc, char* argv[]) {

    if(argc != 2) {
        fmt::print(stderr, "ERROR: no server address provided\n");
        fmt::print(stderr, "Usage: ADM_update_job <SERVER_ADDRESS>\n");
        exit(EXIT_FAILURE);
    }

    admire::server server{"tcp", argv[1]};

    std::vector<admire::dataset> inputs;
    inputs.reserve(NINPUTS);
    for(int i = 0; i < NINPUTS; ++i) {
        inputs.emplace_back(fmt::format("input-dataset-{}", i));
    }

    std::vector<admire::dataset> outputs;
    outputs.reserve(NOUTPUTS);
    for(int i = 0; i < NOUTPUTS; ++i) {
        outputs.emplace_back(fmt::format("output-dataset-{}", i));
    }

    admire::job_requirements reqs{inputs, outputs};
    admire::job job{42};

    try {

        [[maybe_unused]] const auto ret = admire::update_job(server, job, reqs);

        // do something with job. 
        // TODO: name changed from job to ret to not create conflict.

        fmt::print(stdout, "ADM_update_job() remote procedure completed "
                           "successfully\n");
        exit(EXIT_SUCCESS);
    } catch(const std::exception& e) {
        fmt::print(stderr, "FATAL: ADM_update_job() failed: {}\n", e.what());
        exit(EXIT_FAILURE);
    }
}

