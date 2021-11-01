#include "problems/GPetscFENonlinearProblem.h"
#include "base/CallStack.h"
#include "mesh/GMesh.h"

#include "petscdm.h"
#include "petscdmlabel.h"
#include "petscds.h"
#include "petscviewerhdf5.h"

registerMooseObject("GodzillaApp", GPetscFENonlinearProblem);


static PetscErrorCode
zero_fn(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar *u, void *ctx)
{
  u[0] = 0.0;
  return 0;
}

static PetscErrorCode
quadratic_u_2d(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar *u, void *ctx)
{
  *u = x[0] * x[0] + x[1] * x[1];
  return 0;
}

static void
f0_u(PetscInt dim, PetscInt Nf, PetscInt NfAux,
     const PetscInt uOff[], const PetscInt uOff_x[], const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[],
     const PetscInt aOff[], const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
     PetscReal t, const PetscReal x[], PetscInt numConstants, const PetscScalar constants[], PetscScalar f0[])
{
  f0[0] = 4.0;
}

/* gradU[comp*dim+d] = {u_x, u_y} or {u_x, u_y, u_z} */
static void
f1_u(PetscInt dim, PetscInt Nf, PetscInt NfAux,
     const PetscInt uOff[], const PetscInt uOff_x[], const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[],
     const PetscInt aOff[], const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
     PetscReal t, const PetscReal x[], PetscInt numConstants, const PetscScalar constants[], PetscScalar f1[])
{
  PetscInt d;
  for (d = 0; d < dim; ++d)
    f1[d] = u_x[d];
}

/* < \nabla v, \nabla u + {\nabla u}^T >
   This just gives \nabla u, give the perdiagonal for the transpose */
static void
g3_uu(PetscInt dim, PetscInt Nf, PetscInt NfAux,
      const PetscInt uOff[], const PetscInt uOff_x[], const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[],
      const PetscInt aOff[], const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
      PetscReal t, PetscReal u_tShift, const PetscReal x[], PetscInt numConstants, const PetscScalar constants[], PetscScalar g3[])
{
  PetscInt d;
  for (d = 0; d < dim; ++d)
    g3[d * dim + d] = 1.0;
}

///

InputParameters
GPetscFENonlinearProblem::validParams()
{
    InputParameters params = GPetscNonlinearProblem::validParams();
    params.addPrivateParam<GMesh *>("_gmesh");
    return params;
}

GPetscFENonlinearProblem::GPetscFENonlinearProblem(const InputParameters & parameters) :
    GPetscNonlinearProblem(parameters),
    mesh(*getParam<GMesh *>("_gmesh"))
{
    _F_;
}

GPetscFENonlinearProblem::~GPetscFENonlinearProblem()
{
    _F_;
    PetscFEDestroy(&this->fe);
}

const DM &
GPetscFENonlinearProblem::getDM()
{
    return this->mesh.getDM();
}

void
GPetscFENonlinearProblem::setupProblem()
{
    _F_;
    PetscErrorCode ierr;

    const DM & dm = this->getDM();

    // setup discretization
    DMGetDimension(dm, &this->dim);
    ierr = PetscFECreateLagrange(comm().get(), this->dim, 1, PETSC_FALSE, 1, PETSC_DETERMINE, &this->fe);
    ierr = DMSetField(dm, 0, NULL, (PetscObject) this->fe);

    PetscSection section;
    DMGetSection(dm, &section);
    PetscSectionSetFieldName(section, 0, "u");

    ierr = DMSetAdjacency(dm, 0, PETSC_FALSE, PETSC_TRUE);

    ierr = DMCreateDS(dm);
    ierr = DMGetDS(dm, &this->ds);

    // setup problem
    PetscDSSetResidual(ds, 0, f0_u, f1_u);
    PetscDSSetJacobian(ds, 0, 0, NULL, NULL, NULL, g3_uu);

    const PetscInt id = 1;
    const PetscInt ids[] = { 1, 2, 3, 4 };
    PetscFunc *exact_funcs[1];
    exact_funcs[0] = quadratic_u_2d;
    PetscDSSetExactSolution(ds, 0, exact_funcs[0], this);

    DMLabel label;
    DMGetLabel(dm, "Face Sets", &label);

    DMAddBoundary(dm,
        DM_BC_ESSENTIAL,
        "1", label,
        4, ids,
        0, 0, NULL,
        (void (*)(void)) exact_funcs[0],
        NULL, this,
        NULL);

    ierr = DMPlexSetSNESLocalFEM(dm, this, this, this);
}

void
GPetscFENonlinearProblem::setupCallbacks()
{
    _F_;
    PetscErrorCode ierr;

    ierr = SNESSetFunction(this->snes, this->r, NULL, NULL);
    ierr = SNESSetJacobian(this->snes, this->A, this->J, NULL, NULL);
}

void
GPetscFENonlinearProblem::allocateObjects()
{
    _F_;
}

void
GPetscFENonlinearProblem::setInitialGuess()
{
    PetscErrorCode ierr;

    DM dm = this->getDM();
    this->initial_guess[0] = quadratic_u_2d;
    ierr = DMProjectFunction(dm, 0.0, this->initial_guess, NULL, INSERT_VALUES, this->x);
}

void
GPetscFENonlinearProblem::out()
{
    const DM & dm = getDM();

    PetscViewer viewer;
    PetscViewerCreate(PETSC_COMM_SELF, &viewer);
    PetscViewerSetType(viewer, PETSCVIEWERVTK);
    PetscViewerFileSetMode(viewer, FILE_MODE_WRITE);
    PetscViewerFileSetName(viewer, "out.vtk");
    DMView(dm, viewer);
    VecView(this->x, viewer);
    PetscViewerDestroy(&viewer);
}
