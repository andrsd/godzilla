#include "base/Init.h"
#include "petscsys.h"


namespace godzilla {

Init::Init(int argc, char * argv[], MPI_Comm COMM_WORLD_IN)
{
    PetscErrorCode ierr;
    ierr = PetscInitialize(&argc, &argv, NULL, NULL);
    // get rid of PETSc error handler
    PetscPopSignalHandler();
}

Init::~Init()
{
    PetscFinalize();
}

}
