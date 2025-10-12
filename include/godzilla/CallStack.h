// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/MemoryArena.h"
#include "godzilla/Allocators.h"
#include "fmt/format.h"
#include <array>

namespace godzilla {
namespace internal {

#define _GET_CALL_STK_MACRO(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define _CALL_STK_MSG_COUNT_ARGS_IMPL(...) _GET_CALL_STK_MACRO(__VA_ARGS__)
#define _CALL_STK_MSG_COUNT_ARGS(...) \
    _CALL_STK_MSG_COUNT_ARGS_IMPL(_, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// clang-format off
#define _CALL_STK_MSG0() \
    godzilla::internal::CallStack::Msg __call_stack_msg##__COUNTER__(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#define _CALL_STK_MSG1(fmt) \
    godzilla::internal::CallStack::Msg __call_stack_msg##__COUNTER__(__FILE__, __LINE__, fmt)
#define _CALL_STK_MSG2(fmt, p1) \
    godzilla::internal::CallStack::Msg __call_stack_msg##__COUNTER__(__FILE__, __LINE__, fmt, p1)
#define _CALL_STK_MSG3(fmt, p1, p2) \
    godzilla::internal::CallStack::Msg __call_stack_msg##__COUNTER__(__FILE__, __LINE__, fmt, p1, p2)
#define _CALL_STK_MSG4(fmt, p1, p2, p3) \
    godzilla::internal::CallStack::Msg __call_stack_msg##__COUNTER__(__FILE__, __LINE__, fmt, p1, p2, p3)
#define _CALL_STK_MSG5(fmt, p1, p2, p3, p4) \
    godzilla::internal::CallStack::Msg __call_stack_msg##__COUNTER__(__FILE__, __LINE__, fmt, p1, p2, p3, p4)
#define _CALL_STK_MSG6(fmt, p1, p2, p3, p4, p5) \
    godzilla::internal::CallStack::Msg __call_stack_msg##__COUNTER__(__FILE__, __LINE__, fmt, p1, p2, p3, p4, p5)
#define _CALL_STK_MSG7(fmt, p1, p2, p3, p4, p5, p6) \
    godzilla::internal::CallStack::Msg __call_stack_msg##__COUNTER__(__FILE__, __LINE__, fmt, p1, p2, p3, p4, p5, p6)
#define _CALL_STK_MSG8(fmt, p1, p2, p3, p4, p5, p6, p7) \
    godzilla::internal::CallStack::Msg __call_stack_msg##__COUNTER__(__FILE__, __LINE__, fmt, p1, p2, p3, p4, p5, p6, p7)
#define _CALL_STK_MSG9(fmt, p1, p2, p3, p4, p5, p6, p7, p8) \
    godzilla::internal::CallStack::Msg __call_stack_msg##__COUNTER__(__FILE__, __LINE__, fmt, p1, p2, p3, p4, p5, p6, p7, p8)
// clang-format on

#define _CALL_STK_MSG_CHOOSER2(count) _CALL_STK_MSG##count
#define _CALL_STK_MSG_CHOOSER(count) _CALL_STK_MSG_CHOOSER2(count)

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
#define CALL_STACK_MSG(...) \
    _CALL_STK_MSG_CHOOSER(_CALL_STK_MSG_COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

extern MemoryArena<char> callstack_arena;
extern MemoryArenaAllocator<char> callstack_alloc;

/// Call stack object
///
/// The allocation of the stack must be static to prevent problems during out-of-memory scenario.
/// That's why we should not be using `std::vector` or similar.
class CallStack {
public:
    static const std::size_t MAX_SIZE = 256;

    /// String that wll be allocated in our dedicated memory arena
    using String = std::basic_string<char, std::char_traits<char>, MemoryArenaAllocator<char>>;

    /// Holds data for one call stack object
    struct Msg {
        /// Construct call stack object
        ///
        /// @param location File name
        /// @param line_no Line number
        /// @param func Function name
        Msg(const char * location, int line_no, const char * func);

        template <typename... T>
        Msg(const char * location, int line_no, fmt::format_string<T...> format, T... args);

        ~Msg();

        MemoryArena<char>::Marker marker;
        /// Message
        String msg;
        /// Location
        String location;
        /// Line number
        int line_no;
    };

public:
    /// Build the call stack object with defined size
    CallStack();

    /// Dump the call stack objects to standard error
    void dump();

    /// Add a message to a stack
    void add(Msg * msg);

    /// Remove a message from a stack
    void remove(Msg * msg);

    /// Get size of the call stack
    std::size_t get_size() const;

    /// Get item at position `idx`
    Msg * at(std::size_t idx) const;

private:
    /// The object storing call stack objects
    std::array<Msg *, MAX_SIZE> stack;
    /// Actual size of the stack
    std::size_t size;

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
CallStack::Msg::Msg(const char * loc, int ln, fmt::format_string<T...> format, T... args) :
    marker(callstack_arena.mark()),
    msg(fmt::format(format, std::forward<T>(args)...), callstack_alloc),
    location(loc, callstack_alloc),
    line_no(ln)

{
    get_callstack().add(this);
}

} // namespace internal
} // namespace godzilla
