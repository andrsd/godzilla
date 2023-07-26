#include "App.h"
#include "Init.h"

int
main(int argc, char * argv[])
{
    godzilla::mpi::Communicator comm;
    godzilla::Init init(argc, argv);

    godzilla::App app("burgers-eqn", comm, argc, argv);
    app.run();

    return 0;
}
