#include "PrintInterface.h"
#include "CallStack.h"
#include "Object.h"
#include "App.h"

namespace godzilla {

PrintInterface::PrintInterface(const Object * obj) :
    proc_id(obj->get_processor_id()),
    verbosity_level(obj->get_app()->get_verbosity_level()),
    prefix(obj->get_name())
{
    _F_;
}

PrintInterface::PrintInterface(const App * app) :
    proc_id(app->get_comm_rank()),
    verbosity_level(app->get_verbosity_level()),
    prefix(app->get_name())
{
    _F_;
}

PrintInterface::PrintInterface(MPI_Comm comm,
                               const unsigned int & verbosity_level,
                               const std::string & prefix) :
    proc_id(0),
    verbosity_level(verbosity_level),
    prefix(prefix)
{
    MPI_Comm_rank(comm, &this->proc_id);
}

} // namespace godzilla
