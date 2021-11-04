#include "base/App.h"
#include "base/Init.h"

int
main(int argc, char * argv[])
{
    MPI_Comm comm = MPI_COMM_WORLD;
    godzilla::Init init(argc, argv, comm);

    godzilla::App app("poisson", comm);
    app.create();
    app.parseCommandLine(argc, argv);
    app.run();

    return 0;
}
