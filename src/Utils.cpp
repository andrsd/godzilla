#include "Utils.h"
#include "CallStack.h"
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>

namespace godzilla {
namespace utils {

bool
path_exists(const std::string & path)
{
    _F_;
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string
to_upper(const std::string & name)
{
    _F_;
    std::string upper(name);
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper;
}

std::string
to_lower(const std::string & name)
{
    _F_;
    std::string lower(name);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

bool
has_suffix(const std::string & str, const std::string & suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace utils
} // namespace godzilla
