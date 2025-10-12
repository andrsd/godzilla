// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/MemoryArena.h"
#include "petscsys.h"
#include <csignal>

namespace godzilla {
namespace internal {

/// Number of bytes reserved per call stack message (both for location and function name)
constexpr unsigned int MAX_MSG_LEN = 512;
// Memory area for allocating callstack object strings
static MemoryArena<char> callstack_arena(CallStack::MAX_SIZE * MAX_MSG_LEN);
// Custom allocator
static MemoryArenaAllocator<char> callstack_alloc(callstack_arena);
// global instance of the call stack object
static CallStack callstack;

// Call Stack Object

CallStack::Msg::Msg(const char * loc, int line_no, const char * func) :
    marker(callstack_arena.mark()),
    msg(func, callstack_alloc),
    location(loc, callstack_alloc),
    line_no(line_no)
{
    callstack.add(this);
}

CallStack::Msg::~Msg()
{
    callstack.remove(this);
    callstack_arena.rewind(this->marker);
}

// Signals

static void
sighandler(int signo)
{
    const char * sig_name[64];

    sig_name[SIGABRT] = "Abort";
    sig_name[SIGSEGV] = "Segmentation violation";
    sig_name[SIGBUS] = "Bus error";
    sig_name[SIGILL] = "Illegal instruction";

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
        for (int n = 0, i = this->size - 1; i >= 0; --i, ++n) {
            Msg * m = this->stack[i];
            PetscFPrintf(PETSC_COMM_WORLD,
                         PETSC_STDERR,
                         "  #%d: %s (%s:%d)\n",
                         n,
                         m->msg.c_str(),
                         m->location.c_str(),
                         m->line_no);
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
        ++this->size;
    }
}

void
CallStack::remove(Msg * msg)
{
    // remove the object only if it is on the top of the call stack
    if (this->size > 0 && this->stack[this->size - 1] == msg) {
        --this->size;
        this->stack[this->size] = nullptr;
    }
}

std::size_t
CallStack::get_size() const
{
    return this->size;
}

CallStack::Msg *
CallStack::at(std::size_t idx) const
{
    return this->stack[idx];
}

void
CallStack::initialize()
{
    // install our signal handlers
    signal(SIGSEGV, sighandler);
    signal(SIGABRT, sighandler);
    signal(SIGBUS, sighandler);
    signal(SIGILL, sighandler);
}

} // namespace internal
} // namespace godzilla
