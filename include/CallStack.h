#pragma once

namespace godzilla {
namespace internal {

/// Place at the begining of a method/function
///
/// @code
/// void Class::method()
/// {
///   _F_
///  ...your code here...
/// }
/// @endcode
#define _F_ \
    godzilla::internal::CallStack::Obj __call_stack_obj(__LINE__, __PRETTY_FUNCTION__, __FILE__);

/// Call stack object
///
/// The allocation of the stack must be static to prevent problems during out-of-memory scenario.
/// That's why we should not be using `std::vector` or similar.
class CallStack {
public:
    /// Build the call stack object with defined size
    ///
    /// @param max_size The maximum number of call stack objects to handle
    CallStack(int max_size = 32);
    virtual ~CallStack();

    /// Dump the call stack objects to standard error
    void dump();

public:
    /// Holds data for one call stack object
    struct Obj {
        /// Construct call stack object
        ///
        /// @param ln Line number
        /// @param func Function name
        /// @param file File name
        Obj(int ln, const char * func, const char * file);
        ~Obj();

        /// Line number in the file
        int line;
        /// File name
        const char * file;
        /// Function name
        const char * func;
    };

protected:
    /// The object storing call stack objects
    Obj ** stack;
    /// Actual size of the stack
    int size;
    /// Maximum size of the stack
    int max_size;

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
