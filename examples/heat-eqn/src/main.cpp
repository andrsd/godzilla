#include "godzilla/App.h"
#include "godzilla/Init.h"
#include "ConvectiveHeatFluxBC.h"
#include "HeatEquationExplicit.h"
#include "HeatEquationProblem.h"

void
registerObjects(godzilla::Registry & r)
{
    godzilla::App::registerObjects(r);
    REGISTER_OBJECT(r, ConvectiveHeatFluxBC);
    REGISTER_OBJECT(r, HeatEquationExplicit);
    REGISTER_OBJECT(r, HeatEquationProblem);
}

int
main(int argc, char * argv[])
{
    try {
        godzilla::mpi::Communicator comm(MPI_COMM_WORLD);
        godzilla::Init init(argc, argv);
        registerObjects(godzilla::App::get_registry());

        godzilla::App app(comm, "heat-eqn", argc, argv);
        app.run();

        return 0;
    }
    catch (...) {
        return -1;
    }
}
