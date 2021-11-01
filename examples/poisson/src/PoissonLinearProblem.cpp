#include "PoissonLinearProblem.h"

registerMooseObject("GodzillaApp", PoissonLinearProblem);

InputParameters
PoissonLinearProblem::validParams()
{
    InputParameters params = GPetscLinearProblem::validParams();
    params.addParam<PetscReal>("uu", 1., "");
    params.addParam<PetscReal>("tt", 1., "");
    params.addParam<PetscInt>("M", 11., "Number of grid points in x-direction");
    params.addParam<PetscInt>("N", 11., "Number of grid points in y-direction");
    return params;
}

PoissonLinearProblem::PoissonLinearProblem(const InputParameters & parameters) :
    GPetscLinearProblem(parameters),
    uu(getParam<PetscReal>("uu")),
    tt(getParam<PetscReal>("tt")),
    M(getParam<PetscInt>("M")),
    N(getParam<PetscInt>("N")),
    dofs(1),
    stencil_width(1)
{
    DMDACreate2d(comm().get(),
        DM_BOUNDARY_NONE, DM_BOUNDARY_NONE,
        DMDA_STENCIL_STAR,
        this->M, this->N, PETSC_DECIDE, PETSC_DECIDE,
        this->dofs, this->stencil_width,
        NULL, NULL,
        &this->da);
    DMSetFromOptions(this->da);
    DMSetUp(da);
}

PoissonLinearProblem::~PoissonLinearProblem()
{
    DMDestroy(&this->da);
}

const DM &
PoissonLinearProblem::getDM()
{
    return this->da;
}

void
PoissonLinearProblem::setupProblem()
{
}

PetscErrorCode
PoissonLinearProblem::computeRhsCallback(Vec b)
{
    PetscScalar pi = 4 * std::atan(1.0);
    PetscScalar Hx = 1.0 / this->M;
    PetscScalar Hy = 1.0 / this->N;

    PetscInt xm, ym, xs, ys;
    DMDAGetCorners(this->da, &xs, &ys, 0, &xm, &ym, 0); /* Fine grid */

    PetscScalar **array;
    DMDAVecGetArray(this->da, b, &array);
    for (PetscInt j = ys; j < ys + ym; j++) {
        for (PetscInt i = xs; i < xs + xm; i++) {
            array[j][i] =
                -PetscCosScalar(this->uu * pi * (i + 0.5) * Hx) *
                +PetscCosScalar(this->tt * pi * (j + 0.5) * Hy) * Hx * Hy;
        }
    }
    DMDAVecRestoreArray(this->da, b, &array);
    VecAssemblyBegin(b);
    VecAssemblyEnd(b);

    // force right hand side to be consistent for singular matrix
    // note this is really a hack, normally the model would provide you with a consistent right handside
    MatNullSpace nullspace;
    MatNullSpaceCreate(comm().get(), PETSC_TRUE, 0, 0, &nullspace);
    MatNullSpaceRemove(nullspace, b);
    MatNullSpaceDestroy(&nullspace);

    return 0;
}

PetscErrorCode
PoissonLinearProblem::computeOperatorsCallback(Mat A, Mat B)
{
    PetscScalar Hx = 1.0 / this->M;
    PetscScalar Hy = 1.0 / this->N;
    PetscScalar HxdHy = Hx / Hy;
    PetscScalar HydHx = Hy / Hx;

    PetscInt xm, ym, xs, ys;
    DMDAGetCorners(this->da, &xs, &ys, 0, &xm, &ym, 0);

    const int STENCIL_SIZE = 5;
    MatStencil row, col[STENCIL_SIZE];
    PetscScalar v[STENCIL_SIZE];
    for (PetscInt j = ys; j < ys + ym; j++) {
        for (PetscInt i = xs; i < xs + xm; i++) {
            row.i = i;
            row.j = j;

            if (i == 0 || j == 0 || i == M - 1 || j == N - 1) {
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
                if (i != M - 1) {
                    v[num] = -HydHx;
                    col[num].i = i + 1;
                    col[num].j = j;
                    num++;
                    numi++;
                }
                if (j != N - 1) {
                    v[num] = -HxdHy;
                    col[num].i = i;
                    col[num].j = j + 1;
                    num++;
                    numj++;
                }
                v[num] = ((PetscReal) (numj) * HxdHy + (PetscReal) (numi) * HydHx);
                col[num].i = i;
                col[num].j = j;
                num++;
                MatSetValuesStencil(B, 1, &row, num, col, v, INSERT_VALUES);
            }
            else {
                v[0] = -HxdHy;                col[0].i = i;     col[0].j = j - 1;
                v[1] = -HydHx;                col[1].i = i - 1; col[1].j = j;
                v[2] = 2.0 * (HxdHy + HydHx); col[2].i = i;     col[2].j = j;
                v[3] = -HydHx;                col[3].i = i + 1; col[3].j = j;
                v[4] = -HxdHy;                col[4].i = i;     col[4].j = j + 1;
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
