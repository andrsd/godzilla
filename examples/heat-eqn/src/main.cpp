#include "godzilla/App.h"
#include "godzilla/Init.h"

int
main(int argc, char * argv[])
{
    godzilla::mpi::Communicator comm(MPI_COMM_WORLD);
    godzilla::Init init(argc, argv);

    godzilla::App app(comm, "heat-eqn", argc, argv);
    app.run();

    return 0;
}
