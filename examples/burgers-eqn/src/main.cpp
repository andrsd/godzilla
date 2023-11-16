#include "godzilla/App.h"
#include "godzilla/Init.h"

int
main(int argc, char * argv[])
{
    godzilla::mpi::Communicator comm;
    godzilla::Init init(argc, argv);

    godzilla::App app(comm, "burgers-eqn", argc, argv);
    app.run();

    return 0;
}
