#pragma once

#include <string>
#include <sstream>
#include "base/CallStack.h"
#include <iostream>

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
godzillaErrorRaw(std::string msg, bool call_stack = false);

} // namespace internal

class App;
class Object;

class GPrintInterface {
public:
    GPrintInterface(const App & app);
    GPrintInterface(const Object * obj);

    /// Print a message on a terminal
    /// Inside static functions, you will need to explicitly scope your mooseError
    /// call - i.e. do "::mooseError(arg1, ...);".
    template <typename... Args>
    void
    godzillaPrint(unsigned int level, Args &&... args)
    {
        if (level <= this->verbosity_level) {
            std::ostringstream oss;
            internal::godzillaStreamAll(oss, this->prefix, std::forward<Args>(args)...);
            internal::godzillaMsgRaw(oss.str());
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
        internal::godzillaStreamAll(oss, this->prefix, std::forward<Args>(args)...);
        internal::godzillaErrorRaw(oss.str());
    }

    template <typename... Args>
    [[noreturn]] void
    godzillaErrorWithCallStack(Args &&... args)
    {
        std::ostringstream oss;
        internal::godzillaStreamAll(oss, this->prefix, std::forward<Args>(args)...);
        internal::godzillaErrorRaw(oss.str(), true);
    }

private:
    const unsigned int & verbosity_level;
    /// Prefix to print
    const std::string prefix;
};

} // namespace godzilla
