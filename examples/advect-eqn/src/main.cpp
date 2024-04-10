#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "godzilla/Registry.h"
#include "AdvectionEquation.h"
#include "InflowBC.h"
#include "OutflowBC.h"

void
registerObjects(godzilla::Registry & r)
{
    godzilla::App::registerObjects(r);
    REGISTER_OBJECT(r, AdvectionEquation);
    REGISTER_OBJECT(r, InflowBC);
    REGISTER_OBJECT(r, OutflowBC);
}

int
main(int argc, char * argv[])
{
    try {
        godzilla::mpi::Communicator comm(MPI_COMM_WORLD);
        godzilla::Init init(argc, argv);
        registerObjects(godzilla::App::get_registry());

        godzilla::App app(comm, "advect-eqn", argc, argv);
        app.run();

        return 0;
    }
    catch (...) {
        return -1;
    }
}
