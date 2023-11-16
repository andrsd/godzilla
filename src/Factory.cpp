#include "Factory.h"
#include "CallStack.h"

namespace godzilla {

void
Factory::destroy()
{
    _F_;
    while (!objects.empty()) {
        delete objects.front();
        objects.pop_front();
    }
    while (!params.empty()) {
        delete params.front();
        params.pop_front();
    }
}

} // namespace godzilla
