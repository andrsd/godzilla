#include "base/CallStack.h"
#include <signal.h>
#include <stdlib.h>
#include <iostream>


namespace godzilla {
namespace internal {

// global instance of the call stack object
static CallStack callstack(256);

// Call Stack Object ////

CallStackObj::CallStackObj(int ln, const char *func, const char *file)
{
    this->line = ln;
    this->func = func;
    this->file = file;

    // add this object to the call stack
    if (callstack.size < callstack.max_size) {
        callstack.stack[callstack.size] = this;
        callstack.size++;
    }
}

CallStackObj::~CallStackObj()
{
    // remove the object only if it is on the top of the call stack
    if (callstack.size > 0 && callstack.stack[callstack.size - 1] == this) {
        callstack.size--;
        callstack.stack[callstack.size] = NULL;
    }
}

// Signals ////

static
void sighandler(int signo)
{
    const char *sig_name[64];

    sig_name[SIGABRT] = "Abort";
    sig_name[SIGSEGV] = "Segmentation violation";

    std::cerr << "Caught signal " << signo << " (" << sig_name[signo] << ")" << std::endl;
    std::cerr << std::endl;
    callstack.dump();
    exit(-2);
}

void callstack_initialize()
{
    // install our signal handlers
    signal(SIGSEGV, sighandler);
    signal(SIGABRT, sighandler);
}

void callstack_finalize()
{
}

// Call Stack

CallStack &
getCallstack()
{
    return callstack;
}

CallStack::CallStack(int max_size)
{
    this->max_size = max_size;
    this->size = 0;
    this->stack = new CallStackObj *[max_size];

    // initialize signals
    callstack_initialize();
}

CallStack::~CallStack()
{
    delete [] this->stack;
}

void
CallStack::dump()
{
    if (this->size > 0) {
        std::cerr << "Call stack:" << std::endl;
        for (int i = this->size - 1; i >= 0; i--)
            std::cerr << "  " << this->stack[i]->file << ":" << this->stack[i]->line << ": " << this->stack[i]->func << std::endl;
    }
    else {
        std::cerr << "No call stack available." << std::endl;
    }
}

} // internal
} // godzilla
