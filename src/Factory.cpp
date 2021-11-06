#include "Factory.h"

namespace godzilla {

std::map<std::string, Factory::Entry> Factory::classes;
std::list<Object *> Factory::objects;

void
Factory::destroy()
{
    while (!objects.empty()) {
        delete objects.front();
        objects.pop_front();
    }
}

}
