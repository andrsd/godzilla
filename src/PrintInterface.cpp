#include "PrintInterface.h"
#include "CallStack.h"
#include "Object.h"
#include "App.h"

namespace godzilla {

PrintInterface::PrintInterface(const Object * obj) :
    proc_id(obj->get_processor_id()),
    verbosity_level(obj->get_app().get_verbosity_level()),
    prefix(obj->get_name())
{
    _F_;
}

} // namespace godzilla
