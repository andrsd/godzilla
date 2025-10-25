#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/Init.h"
#include "ConvectiveHeatFluxBC.h"
#include "HeatEquationExplicit.h"
#include "HeatEquationProblem.h"

using namespace godzilla;

int
main(int argc, char * argv[])
{
    try {
        mpi::Communicator comm;
        Init init(argc, argv);

        App app(comm, "heat-eqn");

        app.run();

        return 0;
    }
    catch (Exception & e) {
        fmt::println("{}", e.what());
        print_call_stack(e.get_call_stack());
        return -1;
    }
    catch (...) {
        return -1;
    }
}
