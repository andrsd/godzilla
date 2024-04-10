#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "BurgersEquation.h"

void
registerObjects(godzilla::Registry & r)
{
    godzilla::App::registerObjects(r);
    REGISTER_OBJECT(r, BurgersEquation);
}

int
main(int argc, char * argv[])
{
    try {
        godzilla::mpi::Communicator comm;
        godzilla::Init init(argc, argv);
        registerObjects(godzilla::App::get_registry());

        godzilla::App app(comm, "burgers-eqn", argc, argv);
        app.run();

        return 0;
    }
    catch (...) {
        return -1;
    }
}
