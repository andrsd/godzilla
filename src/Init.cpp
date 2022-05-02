#include "Init.h"
#include "CallStack.h"
#include "Error.h"
#include "petscsys.h"

namespace godzilla {

Init::Init(int argc, char * argv[], MPI_Comm COMM_WORLD_IN)
{
    PetscErrorCode ierr;
    ierr = PetscInitialize(&argc, &argv, NULL, NULL);
    checkPetscError(ierr);
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
