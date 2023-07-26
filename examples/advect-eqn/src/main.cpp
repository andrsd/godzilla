#include "App.h"
#include "Init.h"

int
main(int argc, char * argv[])
{
    godzilla::mpi::Communicator comm(MPI_COMM_WORLD);
    godzilla::Init init(argc, argv);

    godzilla::App app("advect-eqn", comm, argc, argv);
    app.run();

    return 0;
}
