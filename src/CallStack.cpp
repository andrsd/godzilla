// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include "petscsys.h"
#include <csignal>
#include <fmt/core.h>

namespace godzilla {
namespace internal {

// global instance of the call stack object
static CallStack call_stack;

} // namespace internal

// Call Stack Object

CallStack::Frame::Frame(const std::source_location location) :
    file(location.file_name()),
    line(location.line()),
    function(location.function_name())
{
    internal::call_stack.add(this);
}

CallStack::Frame::~Frame()
{
    internal::call_stack.remove(this);
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
    internal::call_stack.dump();
    exit(-2);
}

// Call Stack

CallStack &
get_callstack()
{
    return internal::call_stack;
}

CallStack::CallStack() : size(0) {}

void
CallStack::dump()
{
#ifndef NDEBUG
    if (this->size > 0) {
        PetscFPrintf(PETSC_COMM_WORLD, PETSC_STDERR, "Call stack:\n");
        for (int n = 0, i = this->size - 1; i >= 0; --i, ++n) {
            auto * m = this->stack[i];
            PetscFPrintf(PETSC_COMM_WORLD,
                         PETSC_STDERR,
                         "  #%d: %s (%s:%d)\n",
                         n,
                         m->function.c_str(),
                         m->file.c_str(),
                         m->line);
        }
    }
    else
        PetscFPrintf(PETSC_COMM_WORLD, PETSC_STDERR, "No call stack available.\n");
#endif
}

void
CallStack::add(Frame * frame)
{
    // add this object to the call stack
    if (this->size < CallStack::MAX_SIZE) {
        this->stack[this->size] = frame;
        ++this->size;
    }
}

void
CallStack::remove(Frame * msg)
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

CallStack::Frame *
CallStack::operator[](std::size_t idx) const
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

void
print_call_stack(const std::source_location loc)
{
    for (int i = internal::call_stack.get_size() - 1; i >= 0; i--) {
        const auto & frame = internal::call_stack[i];
        if (loc.function_name() != frame->function)
            fmt::println(stderr, "  at {} ({}:{})", frame->function, frame->file, frame->line);
    }
}

} // namespace godzilla
