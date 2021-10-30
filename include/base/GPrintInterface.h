#pragma once

#include <string>
#include <sstream>


namespace godzilla
{

namespace internal
{

/// All of the following are not meant to be called directly - they are called by the normal macros
/// (godzillaError(), etc.) down below
void godzillaStreamAll(std::ostringstream & ss);

template <typename T, typename... Args>
void
godzillaStreamAll(std::ostringstream & ss, T && val, Args &&... args)
{
    ss << val;
    godzillaStreamAll(ss, std::forward<Args>(args)...);
}

void
godzillaMsgRaw(const std::string & msg);

std::string
godzillaMsgFmt(const std::string & msg, const std::string & title, const std::string & color);

[[noreturn]] void
godzillaErrorRaw(std::string msg);

} // namespace internal
} // namespace godzilla


class GodzillaApp;

class GPrintInterface {
public:
    GPrintInterface(const GodzillaApp & app);

    /// Print a message on a terminal
    /// Inside static functions, you will need to explicitly scope your mooseError
    /// call - i.e. do "::mooseError(arg1, ...);".
    template <typename... Args>
    void
    godzillaPrint(unsigned int level, Args &&... args)
    {
        if (level < verbosity_level) {
            std::ostringstream oss;
            godzilla::internal::godzillaStreamAll(oss, std::forward<Args>(args)...);
            godzilla::internal::godzillaMsgRaw(oss.str());
        }
    }

    /// Emit an error message with the given stringified, concatenated args and
    /// terminate the application.  Inside static functions, you will need to
    /// explicitly scope your mooseError call - i.e. do "::mooseError(arg1, ...);".
    template <typename... Args>
    [[noreturn]] void
    godzillaError(Args &&... args)
    {
        std::ostringstream oss;
        godzilla::internal::godzillaStreamAll(oss, std::forward<Args>(args)...);
        godzilla::internal::godzillaErrorRaw(oss.str());
    }

private:
    const unsigned int & verbosity_level;
};
