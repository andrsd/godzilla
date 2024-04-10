#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "NSIncompressibleProblem.h"

void
registerObjects(godzilla::Registry & r)
{
    godzilla::App::registerObjects(r);
    REGISTER_OBJECT(r, NSIncompressibleProblem);
}

int
main(int argc, char * argv[])
{
    try {
        godzilla::mpi::Communicator comm(MPI_COMM_WORLD);
        godzilla::Init init(argc, argv);
        registerObjects(godzilla::App::get_registry());

        godzilla::App app(comm, "ns-incomp", argc, argv);
        app.run();

        return 0;
    }
    catch (...) {
        return -1;
    }
}
