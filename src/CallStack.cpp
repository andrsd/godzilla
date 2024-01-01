// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include <csignal>
#include "petscsys.h"

namespace godzilla {
namespace internal {

// global instance of the call stack object
static CallStack callstack;

// Call Stack Object ////

CallStack::Msg::Msg(const char * func)
{
    this->msg = fmt::format("{}", func);
    callstack.add(this);
}

CallStack::Msg::~Msg()
{
    callstack.remove(this);
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

CallStack::CallStack() : size(0) {}

void
CallStack::dump()
{
    if (this->size > 0) {
        PetscFPrintf(PETSC_COMM_WORLD, PETSC_STDERR, "Call stack:\n");
        for (int n = 0, i = this->size - 1; i >= 0; i--, n++) {
            Msg * m = this->stack[i];
            PetscFPrintf(PETSC_COMM_WORLD, PETSC_STDERR, "  #%d: %s\n", n, m->msg.c_str());
        }
    }
    else
        PetscFPrintf(PETSC_COMM_WORLD, PETSC_STDERR, "No call stack available.\n");
}

void
CallStack::add(Msg * msg)
{
    // add this object to the call stack
    if (this->size < CallStack::MAX_SIZE) {
        this->stack[this->size] = msg;
        this->size++;
    }
}

void
CallStack::remove(Msg * msg)
{
    // remove the object only if it is on the top of the call stack
    if (this->size > 0 && this->stack[this->size - 1] == msg) {
        this->size--;
        this->stack[this->size] = nullptr;
    }
}

int
CallStack::get_size() const
{
    return this->size;
}

CallStack::Msg *
CallStack::at(int idx) const
{
    return this->stack[idx];
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
