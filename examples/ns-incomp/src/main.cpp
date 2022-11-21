#include "App.h"
#include "Init.h"

int
main(int argc, char * argv[])
{
    MPI_Comm comm = MPI_COMM_WORLD;
    godzilla::Init init(argc, argv, comm);

    godzilla::App app("ns-incomp", comm);
    app.parse_command_line(argc, argv);
    app.run();

    return 0;
}
