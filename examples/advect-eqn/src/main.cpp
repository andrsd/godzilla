#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "AdvectionEquation.h"
#include "InflowBC.h"
#include "OutflowBC.h"

void
registerObjects(godzilla::Registry & r)
{
    REGISTER_OBJECT2(r, AdvectionEquation);
    REGISTER_OBJECT2(r, InflowBC);
    REGISTER_OBJECT2(r, OutflowBC);
    // r.add<AdvectionEquation>("AdvectionEquation");
}

int
main(int argc, char * argv[])
{
    godzilla::mpi::Communicator comm(MPI_COMM_WORLD);
    godzilla::Init init(argc, argv);

    registerObjects(godzilla::App::get_registry());

    godzilla::App app(comm, "advect-eqn", argc, argv);
    app.run();

    return 0;
}
