#include "PoissonFENonlinearProblem.h"
#include "base/CallStack.h"

#include "petscdm.h"
#include "petscdmlabel.h"

registerMooseObject("GodzillaApp", PoissonFENonlinearProblem);


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
PoissonFENonlinearProblem::validParams()
{
    InputParameters params = GPetscFENonlinearProblem::validParams();
    return params;
}

PoissonFENonlinearProblem::PoissonFENonlinearProblem(const InputParameters & parameters) :
    GPetscFENonlinearProblem(parameters)
{
    _F_;
}

PoissonFENonlinearProblem::~PoissonFENonlinearProblem()
{
}

void
PoissonFENonlinearProblem::onSetFields()
{
    _F_;
    PetscInt order = 1;
    this->u_id = addField("u", 1, order);
}

void
PoissonFENonlinearProblem::onSetWeakForm()
{
    setResidualBlock(this->u_id, f0_u, f1_u);
    setJacobianBlock(this->u_id, this->u_id, NULL, NULL, NULL, g3_uu);
}

void
PoissonFENonlinearProblem::setupBoundaryConditions()
{
    const DM & dm = getDM();

    PetscFunc *exact_funcs[1];
    exact_funcs[0] = quadratic_u_2d;

    DMLabel label;
    DMGetLabel(dm, "marker", &label);
    const PetscInt id = 1;

    // DMLabel label;
    // DMGetLabel(dm, "Face Sets", &label);
    // const PetscInt ids[] = { 0, 1, 2, 3 };

    PetscDSAddBoundary(this->ds,
        DM_BC_ESSENTIAL,
        "1", label,
        1, &id,
        // 4, ids,
        0, 0, NULL,
        (void (*)(void)) exact_funcs[0],
        NULL, this,
        NULL);
}

void
PoissonFENonlinearProblem::onSetInitialConditions()
{
    _F_;
    PetscErrorCode ierr;
    setInitialCondition(zero_fn);
}

void
PoissonFENonlinearProblem::out()
{
    _F_;
    const DM & dm = getDM();

    // PetscViewer viewer;
    // PetscViewerCreate(PETSC_COMM_SELF, &viewer);
    // PetscViewerSetType(viewer, PETSCVIEWERVTK);
    // PetscViewerFileSetMode(viewer, FILE_MODE_WRITE);
    // PetscViewerFileSetName(viewer, "out.vtk");
    // DMView(dm, viewer);
    // VecView(this->x, viewer);
    // PetscViewerDestroy(&viewer);

    PetscFunc *exact_funcs[1];
    exact_funcs[0] = quadratic_u_2d;
    PetscReal error;
    DMComputeL2Diff(dm, 0.0, exact_funcs, NULL, this->x, &error);
    std::cerr << "error = " << std::scientific << error << std::endl;
}
