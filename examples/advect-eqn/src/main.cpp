#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "godzilla/Registry.h"
#include "AdvectionEquation.h"
#include "InflowBC.h"
#include "OutflowBC.h"

godzilla::Registry registry;

void
register_objects(godzilla::Registry & r)
{
    godzilla::App::register_objects(r);
    REGISTER_OBJECT(r, AdvectionEquation);
    REGISTER_OBJECT(r, InflowBC);
    REGISTER_OBJECT(r, OutflowBC);
}

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm(MPI_COMM_WORLD);
        godzilla::Init init(argc, argv);
        register_objects(registry);

        godzilla::App app(comm, registry, "advect-eqn", argc, argv);
        app.run();

        return 0;
    }
    catch (...) {
        return -1;
    }
}
