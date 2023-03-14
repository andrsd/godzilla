#include "GodzillaConfig.h"
#include "Init.h"
#include "CallStack.h"
#include "PerfLog.h"
#include "petscsys.h"

namespace godzilla {

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
