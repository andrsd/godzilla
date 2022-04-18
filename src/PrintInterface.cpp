#include "PrintInterface.h"
#include "Object.h"
#include "App.h"

namespace godzilla {

PrintInterface::PrintInterface(const App & app) :
    verbosity_level(app.get_verbosity_level()),
    prefix(""),
    pi_comm(app.get_comm())
{
    _F_;
}

PrintInterface::PrintInterface(const Object * obj) :
    verbosity_level(dynamic_cast<const App &>(obj->getApp()).get_verbosity_level()),
    prefix(obj->getName() + ": "),
    pi_comm(obj->comm())
{
    _F_;
}

} // namespace godzilla
