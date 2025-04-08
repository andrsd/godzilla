// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Init.h"
#include "godzilla/Registry.h"
#include "godzilla/App.h"
#include "mpicpp-lite/mpicpp-lite.h"

namespace mpi = mpicpp_lite;

godzilla::Registry registry;

/// Application that dumps the parameters in the registry to a YAML file
///
/// @note This is used for generating the documentation of the parameters
class ParamDumperApp : public godzilla::App {
public:
    ParamDumperApp(const mpi::Communicator & comm, godzilla::Registry & registry) :
        App(comm, registry, "param_dumper", {})
    {
        godzilla::App::register_objects(registry);
    }

    void
    run() override
    {
        export_parameters_yaml();
    }
};

int
main(int argc, char * argv[])
{
    try {
        godzilla::mpi::Communicator comm(MPI_COMM_WORLD);
        godzilla::Init init;
        ParamDumperApp app(comm, registry);
        app.run();
        return 0;
    }
    catch (const std::exception & e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
