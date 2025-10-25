#include "TestApp.h"
#include "godzilla/Registry.h"
#include "GTestFENonlinearProblem.h"
#include "GTest2FieldsFENonlinearProblem.h"

using namespace godzilla;

Registry registry;

TestApp::TestApp() : App(mpi::Communicator(MPI_COMM_WORLD), ::registry, "godzilla", {})
{
    App::register_objects(::registry);
    ::registry.add<GTestFENonlinearProblem>("GTestFENonlinearProblem");
    ::registry.add<GTest2FieldsFENonlinearProblem>("GTest2FieldsFENonlinearProblem");
}
