#include "godzilla/Registry.h"

namespace godzilla {

Registry &
Registry::instance()
{
    static Registry registry;
    return registry;
}

} // namespace godzilla
