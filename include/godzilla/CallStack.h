// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/MemoryArena.h"
#include "godzilla/Allocators.h"
#include "fmt/format.h"
#include <array>

namespace godzilla {
namespace internal {

#define _CALL_STK_MSG_CONCAT(a, b) a##b
#define _CALL_STK_MSG_NAME(a, b) _CALL_STK_MSG_CONCAT(a, b)

/// Place at the beginning of a method/function
///
/// @code
/// void Class::method()
/// {
///   CALL_STACK_MSG();
///  ...your code here...
/// }
/// @endcode
#define CALL_STACK_MSG() \
    godzilla::internal::CallStack::Msg _CALL_STK_MSG_NAME(__call_stack_msg, __COUNTER__)(__FILE__, __LINE__, __PRETTY_FUNCTION__)

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

} // namespace internal
} // namespace godzilla
