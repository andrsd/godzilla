#include "PoissonApp.h"
#include "godzilla/Init.h"
#include "godzilla/Exception.h"

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm;
        godzilla::Init init(argc, argv);

        PoissonApp app(comm, "poisson", argc, argv);
        app.set_verbosity_level(9);
        return app.run();
    }
    catch (godzilla::Exception & e) {
        fmt::println("{}", e.what());
        fmt::println("");
        godzilla::print_call_stack(e.get_call_stack());
        return -1;
    }
    catch (...) {
        return -1;
    }
}
