#include "PrintInterface.h"
#include "Object.h"
#include "App.h"

namespace godzilla {

PrintInterface::PrintInterface(const Object * obj) :
    verbosity_level(obj->get_app().get_verbosity_level()),
    prefix(obj->get_name() + ": ")
{
    _F_;
}

} // namespace godzilla
