#include "CallStack.h"
#include <csignal>
#include "petscsys.h"

namespace godzilla {
namespace internal {

// global instance of the call stack object
static CallStack callstack(256);

// Call Stack Object ////

CallStack::Obj::Obj(int ln, const char * func, const char * file)
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

CallStack::Obj::~Obj()
{
    // remove the object only if it is on the top of the call stack
    if (callstack.size > 0 && callstack.stack[callstack.size - 1] == this) {
        callstack.size--;
        callstack.stack[callstack.size] = nullptr;
    }
}

// Signals ////

static void
sighandler(int signo)
{
    const char * sig_name[64];

    sig_name[SIGABRT] = "Abort";
    sig_name[SIGSEGV] = "Segmentation violation";

    PetscFPrintf(PETSC_COMM_WORLD,
                 PETSC_STDERR,
                 "Caught signal %d (%s)\n\n",
                 signo,
                 sig_name[signo]);
    callstack.dump();
    exit(-2);
}

// Call Stack

CallStack &
get_callstack()
{
    return callstack;
}

CallStack::CallStack(int max_size)
{
    this->max_size = max_size;
    this->size = 0;
    this->stack = new Obj *[max_size];
}

CallStack::~CallStack()
{
    delete[] this->stack;
}

void
CallStack::dump()
{
    if (this->size > 0) {
        PetscFPrintf(PETSC_COMM_WORLD, PETSC_STDERR, "Call stack:\n");
        for (int n = 0, i = this->size - 1; i >= 0; i--, n++) {
            Obj * o = this->stack[i];
            PetscFPrintf(PETSC_COMM_WORLD,
                         PETSC_STDERR,
                         "  #%d: %s:%d: %s\n",
                         n,
                         o->file,
                         o->line,
                         o->func);
        }
    }
    else
        PetscFPrintf(PETSC_COMM_WORLD, PETSC_STDERR, "No call stack available.\n");
}

void
CallStack::initialize()
{
    // install our signal handlers
    signal(SIGSEGV, sighandler);
    signal(SIGABRT, sighandler);
}

} // namespace internal
} // namespace godzilla
