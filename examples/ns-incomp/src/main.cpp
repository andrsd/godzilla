#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "NSIncompressibleProblem.h"

godzilla::Registry registry;

void
register_objects(godzilla::Registry & r)
{
    godzilla::App::register_objects(r);
    REGISTER_OBJECT(r, NSIncompressibleProblem);
}

int
main(int argc, char * argv[])
{
    try {
        godzilla::mpi::Communicator comm(MPI_COMM_WORLD);
        godzilla::Init init(argc, argv);
        register_objects(registry);

        godzilla::App app(comm, registry, "ns-incomp", argc, argv);
        app.run();

        return 0;
    }
    catch (...) {
        return -1;
    }
}
