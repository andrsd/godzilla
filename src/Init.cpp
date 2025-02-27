// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Init.h"
#include "godzilla/CallStack.h"
#include "godzilla/PerfLog.h"
#include "petscsys.h"

namespace godzilla {

Init::Init()
{
    int argc = 0;
    char ** argv = { nullptr };
    PetscInitialize(&argc, &argv, nullptr, nullptr);
    perf_log::init();
    // get rid of PETSc error handler
    PetscPopSignalHandler();
    PetscPopErrorHandler();
    //
    PetscOptionsSetValue(NULL, "-options_left", "no");
    internal::CallStack::initialize();
}

Init::Init(int argc, char * argv[])
{
    PetscInitialize(&argc, &argv, nullptr, nullptr);
    perf_log::init();
    // get rid of PETSc error handler
    PetscPopSignalHandler();
    PetscPopErrorHandler();
    //
    PetscOptionsSetValue(NULL, "-options_left", "no");
    internal::CallStack::initialize();
}

Init::~Init()
{
    PetscFinalize();
}

} // namespace godzilla
