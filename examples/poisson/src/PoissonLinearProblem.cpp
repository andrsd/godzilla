#include "PoissonLinearProblem.h"
#include "StructuredGrid2D.h"
#include "petscdmda.h"

registerObject(PoissonLinearProblem);

using namespace godzilla;

InputParameters
PoissonLinearProblem::validParams()
{
    InputParameters params = LinearProblem::validParams();
    params.addParam<PetscReal>("m", 1., "Coefficient in the forcing term");
    params.addParam<PetscReal>("n", 1., "Coefficient in the forcing term");
    return params;
}

PoissonLinearProblem::PoissonLinearProblem(const InputParameters & parameters) :
    LinearProblem(parameters),
    m(getParam<PetscReal>("m")),
    n(getParam<PetscReal>("n")),
    dofs(1),
    stencil_width(1)
{
}

void
PoissonLinearProblem::init()
{
    _F_;
    LinearProblem::init();

    StructuredGrid2D * grid_2d = dynamic_cast<StructuredGrid2D *>(&this->grid);
    if (grid_2d != nullptr) {
        this->nx = grid_2d->getNx();
        this->ny = grid_2d->getNy();
    }
    else
        godzillaError("'PoissonLinearProblem' object works only on structured 2D grids.");
}

PetscErrorCode
PoissonLinearProblem::computeRhsCallback(Vec b)
{
    const DM & dm = this->grid.getDM();

    PetscScalar pi = 4 * std::atan(1.0);
    PetscScalar Hx = 1.0 / this->nx;
    PetscScalar Hy = 1.0 / this->ny;

    // Fine grid
    PetscInt xm, ym, xs, ys;
    DMDAGetCorners(dm, &xs, &ys, 0, &xm, &ym, 0);

    PetscScalar ** array;
    DMDAVecGetArray(dm, b, &array);
    for (PetscInt j = ys; j < ys + ym; j++) {
        for (PetscInt i = xs; i < xs + xm; i++) {
            array[j][i] = -PetscCosScalar(this->m * pi * (i + 0.5) * Hx) *
                          +PetscCosScalar(this->n * pi * (j + 0.5) * Hy) * Hx * Hy;
        }
    }
    DMDAVecRestoreArray(dm, b, &array);
    VecAssemblyBegin(b);
    VecAssemblyEnd(b);

    // force right hand side to be consistent for singular matrix
    // note this is really a hack, normally the model would provide you with a consistent right
    // handside
    MatNullSpace nullspace;
    MatNullSpaceCreate(comm(), PETSC_TRUE, 0, 0, &nullspace);
    MatNullSpaceRemove(nullspace, b);
    MatNullSpaceDestroy(&nullspace);

    return 0;
}

PetscErrorCode
PoissonLinearProblem::computeOperatorsCallback(Mat A, Mat B)
{
    const DM & dm = this->grid.getDM();

    PetscScalar Hx = 1.0 / this->nx;
    PetscScalar Hy = 1.0 / this->ny;
    PetscScalar HxdHy = Hx / Hy;
    PetscScalar HydHx = Hy / Hx;

    PetscInt xm, ym, xs, ys;
    DMDAGetCorners(dm, &xs, &ys, 0, &xm, &ym, 0);

    const int STENCIL_SIZE = 5;
    MatStencil row, col[STENCIL_SIZE];
    PetscScalar v[STENCIL_SIZE];
    for (PetscInt j = ys; j < ys + ym; j++) {
        for (PetscInt i = xs; i < xs + xm; i++) {
            row.i = i;
            row.j = j;

            if (i == 0 || j == 0 || i == this->nx - 1 || j == this->ny - 1) {
                PetscInt num = 0;
                PetscInt numi = 0;
                PetscInt numj = 0;

                if (j != 0) {
                    v[num] = -HxdHy;
                    col[num].i = i;
                    col[num].j = j - 1;
                    num++;
                    numj++;
                }
                if (i != 0) {
                    v[num] = -HydHx;
                    col[num].i = i - 1;
                    col[num].j = j;
                    num++;
                    numi++;
                }
                if (i != this->nx - 1) {
                    v[num] = -HydHx;
                    col[num].i = i + 1;
                    col[num].j = j;
                    num++;
                    numi++;
                }
                if (j != this->ny - 1) {
                    v[num] = -HxdHy;
                    col[num].i = i;
                    col[num].j = j + 1;
                    num++;
                    numj++;
                }
                v[num] = ((PetscReal) (numj) *HxdHy + (PetscReal) (numi) *HydHx);
                col[num].i = i;
                col[num].j = j;
                num++;
                MatSetValuesStencil(B, 1, &row, num, col, v, INSERT_VALUES);
            }
            else {
                // clang-format off
                v[0] = -HxdHy;                col[0].i = i;     col[0].j = j - 1;
                v[1] = -HydHx;                col[1].i = i - 1; col[1].j = j;
                v[2] = 2.0 * (HxdHy + HydHx); col[2].i = i;     col[2].j = j;
                v[3] = -HydHx;                col[3].i = i + 1; col[3].j = j;
                v[4] = -HxdHy;                col[4].i = i;     col[4].j = j + 1;
                // clang-format on
                MatSetValuesStencil(B, 1, &row, STENCIL_SIZE, col, v, INSERT_VALUES);
            }
        }
    }

    MatAssemblyBegin(B, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(B, MAT_FINAL_ASSEMBLY);

    MatNullSpace nullspace;
    MatNullSpaceCreate(PETSC_COMM_WORLD, PETSC_TRUE, 0, 0, &nullspace);
    MatSetNullSpace(A, nullspace);
    MatNullSpaceDestroy(&nullspace);

    return 0;
}
