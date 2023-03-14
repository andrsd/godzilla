#include "PrintInterface.h"
#include "CallStack.h"
#include "Object.h"
#include "App.h"
#include "mpi/Communicator.h"

namespace godzilla {

PrintInterface::PrintInterface(const Object * obj) :
    proc_id(obj->get_processor_id()),
    verbosity_level(obj->get_app()->get_verbosity_level()),
    prefix(obj->get_name())
{
    _F_;
}

PrintInterface::PrintInterface(const App * app) :
    proc_id(app->get_comm().rank()),
    verbosity_level(app->get_verbosity_level()),
    prefix(app->get_name())
{
    _F_;
}

PrintInterface::PrintInterface(const mpi::Communicator & comm,
                               const unsigned int & verbosity_level,
                               std::string prefix) :
    proc_id(comm.rank()),
    verbosity_level(verbosity_level),
    prefix(std::move(prefix))
{
}

} // namespace godzilla
