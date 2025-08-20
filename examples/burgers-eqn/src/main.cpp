#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "BurgersEquation.h"

godzilla::Registry registry;

void
register_objects(godzilla::Registry & r)
{
    godzilla::App::register_objects(r);
    REGISTER_OBJECT(r, BurgersEquation);
}

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm;
        godzilla::Init init(argc, argv);
        register_objects(registry);

        godzilla::App app(comm, registry, "burgers-eqn", argc, argv);
        app.run();

        return 0;
    }
    catch (...) {
        return -1;
    }
}
