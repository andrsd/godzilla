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
#ifdef GODZILLA_WITH_PERF_LOG
    PerfLog::init();
#endif
    // get rid of PETSc error handler
    PetscPopSignalHandler();
    PetscPopErrorHandler();
    internal::CallStack::initialize();
}

Init::Init(int argc, char * argv[])
{
    PetscInitialize(&argc, &argv, nullptr, nullptr);
#ifdef GODZILLA_WITH_PERF_LOG
    PerfLog::init();
#endif
    // get rid of PETSc error handler
    PetscPopSignalHandler();
    PetscPopErrorHandler();
    internal::CallStack::initialize();
}

Init::~Init()
{
    PetscFinalize();
}

} // namespace godzilla
