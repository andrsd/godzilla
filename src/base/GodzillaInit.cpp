#include "base/GodzillaInit.h"
#include "petscsys.h"

GodzillaInit::GodzillaInit(int argc, char * argv[], MPI_Comm COMM_WORLD_IN)
{
    PetscErrorCode ierr;
    ierr = PetscInitialize(&argc, &argv, NULL, NULL);
    // get rid of PETSc error handler
    PetscPopSignalHandler();
}

GodzillaInit::~GodzillaInit()
{
    PetscFinalize();
}
