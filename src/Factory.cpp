#include "Factory.h"
#include "CallStack.h"

namespace godzilla {

std::map<std::string, Factory::Entry> Factory::classes;
std::list<Object *> Factory::objects;
std::list<InputParameters *> Factory::params;

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
