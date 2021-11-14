#include "App.h"
#include "Init.h"

int
main(int argc, char * argv[])
{
    MPI_Comm comm = MPI_COMM_WORLD;
    godzilla::Init init(argc, argv, comm);

    godzilla::App app("heat-eqn", comm);
    app.create();
    app.parseCommandLine(argc, argv);
    app.run();

    return 0;
}
