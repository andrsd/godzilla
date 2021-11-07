#include "Utils.h"
#include "CallStack.h"
#include <unistd.h>
#include <sys/stat.h>

namespace godzilla {
namespace utils {

bool
pathExists(const std::string & path)
{
    _F_;
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string
toUpper(const std::string & name)
{
    _F_;
    std::string upper(name);
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper;
}

std::string
toLower(const std::string & name)
{
    _F_;
    std::string lower(name);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

}
}
