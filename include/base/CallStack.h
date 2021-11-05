#pragma once


namespace godzilla {

/// Place at the begining of a method/function
///
/// @code
/// void Class::method()
/// {
///   _F_
///  ...your code here...
/// }
/// @endcode
#define _F_ CallStackObj __call_stack_obj(__LINE__, __PRETTY_FUNCTION__, __FILE__);

/// Holds data for one call stack object
///
struct CallStackObj {
    CallStackObj(int ln, const char *func, const char *file);
    ~CallStackObj();

    /// line number in the file
    int line;
    /// file
    const char *file;
    /// function name
    const char *func;
};

/// Call stack object
///
class CallStack {
public:
    CallStack(int max_size = 32);
    virtual ~CallStack();

    /// dump the call stack objects to standard error
    void dump();

protected:
    CallStackObj **stack;
    int size;
    int max_size;

    friend class CallStackObj;
};

CallStack &getCallstack();

}
