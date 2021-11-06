#include "Godzilla.h"
#include "PoissonNonlinearProblem.h"
#include "CallStack.h"
#include "petscdmda.h"
#include "petscvec.h"


using namespace godzilla;

registerObject(PoissonNonlinearProblem);

InputParameters
PoissonNonlinearProblem::validParams()
{
    InputParameters params = GNonlinearProblem::validParams();
    return params;
}

PoissonNonlinearProblem::PoissonNonlinearProblem(const InputParameters & parameters) :
    GNonlinearProblem(parameters)
{
    _F_;
}

PoissonNonlinearProblem::~PoissonNonlinearProblem()
{
    MatDestroy(&this->Jr);
    VecDestroy(&this->b);
}

void
PoissonNonlinearProblem::allocateObjects()
{
    _F_;
    GNonlinearProblem::allocateObjects();

    const DM dm = getDM();
    PetscErrorCode ierr;
    ierr = VecDuplicate(this->x, &this->b);
    ierr = VecSet(this->b, 1.0);
    ierr = DMCreateMatrix(dm, &this->Jr);
}

PetscErrorCode
PoissonNonlinearProblem::formMatrix(Mat jac)
{
    _F_;
    const DM dm = getDM();

    const PetscInt STENCIL_SIZE = 5;

    PetscInt nrows = 0;
    MatStencil row;
    MatStencil col[STENCIL_SIZE];
    PetscScalar v[STENCIL_SIZE];

    DMDALocalInfo  info;
    DMDAGetLocalInfo(dm, &info);
    PetscScalar hx = 1.0 / (PetscReal) (info.mx - 1);
    PetscScalar hy = 1.0 / (PetscReal) (info.my - 1);
    PetscScalar hxdhy = hx / hy;
    PetscScalar hydhx = hy / hx;

    MatStencil *rows;
    PetscMalloc1(info.ym * info.xm, &rows);

    // Compute entries for the locally owned part of the Jacobian.
    //  - Currently, all PETSc parallel matrix formats are partitioned by
    //    contiguous chunks of rows across the processors.
    //  - Each processor needs to insert only elements that it owns
    //    locally (but any non-local elements will be sent to the
    //    appropriate processor during matrix assembly).
    //  - Here, we set all entries for a particular row at once.
    //  - We can set matrix entries either using either
    //    MatSetValuesLocal() or MatSetValues(), as discussed above.
    for (PetscInt j = info.ys; j < info.ys + info.ym; j++) {
        for (PetscInt i = info.xs; i < info.xs + info.xm; i++) {
            row.j = j;
            row.i = i;
            if (i == 0 || j == 0 || i == info.mx - 1 || j == info.my - 1) {
                // boundary points
                v[0] = 2.0 * (hydhx + hxdhy);
                MatSetValuesStencil(jac, 1, &row, 1, &row, v, INSERT_VALUES);
                rows[nrows].i = i;
                rows[nrows++].j = j;
            }
            else {
                // interior grid points
                v[0] = -hxdhy;                col[0].j = j - 1; col[0].i = i;
                v[1] = -hydhx;                col[1].j = j;     col[1].i = i - 1;
                v[2] = 2.0 * (hydhx + hxdhy); col[2].j = row.j; col[2].i = row.i;
                v[3] = -hydhx;                col[3].j = j;     col[3].i = i + 1;
                v[4] = -hxdhy;                col[4].j = j + 1; col[4].i = i;
                MatSetValuesStencil(jac, 1, &row, STENCIL_SIZE, col, v, INSERT_VALUES);
            }
        }
    }

    // Assemble matrix
    MatAssemblyBegin(jac, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(jac, MAT_FINAL_ASSEMBLY);

    MatZeroRowsColumnsStencil(jac, nrows, rows, 2.0 * (hydhx + hxdhy), NULL, NULL);
    PetscFree(rows);

    // Tell the matrix we will never add a new nonzero location to the matrix.
    // If we do, it will generate an error.
    MatSetOption(jac, MAT_NEW_NONZERO_LOCATION_ERR, PETSC_TRUE);
    return 0;
}

PetscErrorCode
PoissonNonlinearProblem::computeResidualCallback(Vec x, Vec f)
{
    _F_;
    formMatrix(this->Jr);
    MatMult(this->Jr, x, f);
    return 0;
}

PetscErrorCode
PoissonNonlinearProblem::computeJacobianCallback(Vec x, Mat J, Mat Jp)
{
    _F_;
    formMatrix(Jp);
    return 0;
}

void
PoissonNonlinearProblem::solve()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSolve(this->snes, this->b, this->x);
    ierr = SNESGetConvergedReason(this->snes, &this->converged_reason);
}
