#include "Utils.h"
#include "CallStack.h"
#include <algorithm>
#include <sys/stat.h>
#include <chrono>
#include <fmt/printf.h>

namespace godzilla {
namespace utils {

bool
path_exists(const std::string & path)
{
    _F_;
    struct stat buffer = {};
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
    _F_;
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool
has_prefix(const std::string & str, const std::string & prefix)
{
    _F_;
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

std::string
human_time(PetscLogDouble time)
{
    _F_;
    using namespace std::chrono;
    duration<double, std::micro> us(time * 1e6);
    auto h = duration_cast<hours>(us);
    us -= h;
    auto m = duration_cast<minutes>(us);
    us -= m;
    auto s = duration_cast<seconds>(us);
    us -= s;
    auto ms = duration_cast<milliseconds>(us);
    std::string tm;
    if (h.count() > 0)
        tm += fmt::format(" {}h", h.count());
    if (m.count() > 0)
        tm += fmt::format(" {}m", m.count());
    if ((s.count() > 0) || (h.count() == 0 && m.count() == 0)) {
        tm += fmt::format(" {}", s.count());
        if (ms.count() > 0)
            tm += fmt::format(".{}", ms.count());
        tm += fmt::format("s");
    }
    return tm.substr(1);
}

} // namespace utils
} // namespace godzilla
