#include "Validation.h"
#include "Utils.h"
#include "CallStack.h"

namespace godzilla {
namespace validation {

/// Check that `param` attains one of the `options`
///
/// @return `true` if name is one f the options, `false` otherwise
/// @param value Value to test
/// @param options Possible options that value can have
bool
in(const std::string & value, const std::vector<std::string> & options)
{
    _F_;
    std::string v = utils::to_lower(value);
    for (auto & o : options)
        if (v == utils::to_lower(o))
            return true;
    return false;
}

} // namespace validation
} // namespace godzilla
