#include "TestApp.h"
#include "godzilla/Registry.h"

Registry registry;

TestApp::TestApp() : App(mpi::Communicator(MPI_COMM_WORLD), ::registry, "godzilla", {})
{
    App::register_objects(::registry);
}
