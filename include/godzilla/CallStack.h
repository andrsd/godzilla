// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <array>
#include "fmt/format.h"

namespace godzilla {
namespace internal {

#if defined(NDEBUG)
    // In Release builds these macros do nothing
    #define CALL_STACK_MSG(...)
    #define _F_
#else
/// Place at the beginning of a method/function
///
/// @code
/// void Class::method()
/// {
///   _F_;
///  ...your code here...
/// }
/// @endcode
    #define _F_ godzilla::internal::CallStack::Msg __call_stack_msg(__PRETTY_FUNCTION__)

    #define CALL_STK_MSG0() godzilla::internal::CallStack::Msg __call_stack_msg(__PRETTY_FUNCTION__)
    #define CALL_STK_MSG1(fmt) godzilla::internal::CallStack::Msg __call_stack_msg(fmt)
    #define CALL_STK_MSG2(fmt, p1) godzilla::internal::CallStack::Msg __call_stack_msg(fmt, p1)
    #define CALL_STK_MSG3(fmt, p1, p2) \
        godzilla::internal::CallStack::Msg __call_stack_msg(fmt, p1, p2)
    #define CALL_STK_MSG4(fmt, p1, p2, p3) \
        godzilla::internal::CallStack::Msg __call_stack_msg(fmt, p1, p2, p3)
    #define CALL_STK_MSG5(fmt, p1, p2, p3, p4) \
        godzilla::internal::CallStack::Msg __call_stack_msg(fmt, p1, p2, p3, p4)
    #define CALL_STK_MSG6(fmt, p1, p2, p3, p4, p5) \
        godzilla::internal::CallStack::Msg __call_stack_msg(fmt, p1, p2, p3, p4, p5)
    #define CALL_STK_MSG7(fmt, p1, p2, p3, p4, p5, p6) \
        godzilla::internal::CallStack::Msg __call_stack_msg(fmt, p1, p2, p3, p4, p5, p6)
    #define CALL_STK_MSG8(fmt, p1, p2, p3, p4, p5, p6, p7) \
        godzilla::internal::CallStack::Msg __call_stack_msg(fmt, p1, p2, p3, p4, p5, p6, p7)
    #define CALL_STK_MSG9(fmt, p1, p2, p3, p4, p5, p6, p7, p8) \
        godzilla::internal::CallStack::Msg __call_stack_msg(fmt, p1, p2, p3, p4, p5, p6, p7, p8)
    #define CALL_STK_MSG10(fmt, p1, p2, p3, p4, p5, p6, p7, p8, p9) \
        godzilla::internal::CallStack::Msg __call_stack_msg(fmt, p1, p2, p3, p4, p5, p6, p7, p8, p9)
    #define GET_CALL_STK_MACRO(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME

/// Place at the beginning of a method/function
///
/// @code
/// void Class::method()
/// {
///   CALL_STACK_MSG();
///  ...your code here...
/// }
/// @endcode
///
/// or:
/// @code
/// void Class::method(int param)
/// {
///   CALL_STACK_MSG("msg({})", param);
///  ...your code here...
/// }
/// @endcode
    #define CALL_STACK_MSG(...)            \
        GET_CALL_STK_MACRO(_0,             \
                           ##__VA_ARGS__,  \
                           CALL_STK_MSG10, \
                           CALL_STK_MSG9,  \
                           CALL_STK_MSG8,  \
                           CALL_STK_MSG7,  \
                           CALL_STK_MSG6,  \
                           CALL_STK_MSG5,  \
                           CALL_STK_MSG4,  \
                           CALL_STK_MSG3,  \
                           CALL_STK_MSG2,  \
                           CALL_STK_MSG1,  \
                           CALL_STK_MSG0)  \
        (__VA_ARGS__)
#endif

/// Call stack object
///
/// The allocation of the stack must be static to prevent problems during out-of-memory scenario.
/// That's why we should not be using `std::vector` or similar.
class CallStack {
public:
    static const std::size_t MAX_SIZE = 256;

    /// Holds data for one call stack object
    struct Msg {
        /// Construct call stack object
        ///
        /// @param func Function name
        Msg(const char * func);

        template <typename... T>
        Msg(fmt::format_string<T...> format, T... args);

        ~Msg();

        /// Message
        std::string msg;
    };

public:
    /// Build the call stack object with defined size
    explicit CallStack();

    /// Dump the call stack objects to standard error
    void dump();

    /// Add a message to a stack
    void add(Msg * msg);

    /// Remove a message from a stack
    void remove(Msg * msg);

    /// Get size of the call stack
    int get_size() const;

    /// Get item at position `idx`
    Msg * at(int idx) const;

private:
    /// The object storing call stack objects
    std::array<Msg *, MAX_SIZE> stack;
    /// Actual size of the stack
    int size;

public:
    /// Initialize the call stack capability
    ///
    /// This hooks to SIGABRT and SIGSEGV signals
    static void initialize();
};

/// Get the call stack object
///
/// @return Call stack object
CallStack & get_callstack();

template <typename... T>
CallStack::Msg::Msg(fmt::format_string<T...> format, T... args)
{
    this->msg = fmt::format(format, std::forward<T>(args)...);
    get_callstack().add(this);
}

} // namespace internal
} // namespace godzilla
