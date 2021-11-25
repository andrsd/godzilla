#include "Godzilla.h"
#include "FunctionInterface.h"
#include "HeatEquationProblem.h"
#include "CallStack.h"

using namespace godzilla;

registerObject(HeatEquationProblem);

static void
f0_temp(PetscInt dim,
        PetscInt Nf,
        PetscInt NfAux,
        const PetscInt uOff[],
        const PetscInt uOff_x[],
        const PetscScalar u[],
        const PetscScalar u_t[],
        const PetscScalar u_x[],
        const PetscInt aOff[],
        const PetscInt aOff_x[],
        const PetscScalar a[],
        const PetscScalar a_t[],
        const PetscScalar a_x[],
        PetscReal t,
        const PetscReal x[],
        PetscInt numConstants,
        const PetscScalar constants[],
        PetscScalar f0[])
{
    f0[0] = u_t[0];
}

static void
f1_temp(PetscInt dim,
        PetscInt Nf,
        PetscInt NfAux,
        const PetscInt uOff[],
        const PetscInt uOff_x[],
        const PetscScalar u[],
        const PetscScalar u_t[],
        const PetscScalar u_x[],
        const PetscInt aOff[],
        const PetscInt aOff_x[],
        const PetscScalar a[],
        const PetscScalar a_t[],
        const PetscScalar a_x[],
        PetscReal t,
        const PetscReal x[],
        PetscInt numConstants,
        const PetscScalar constants[],
        PetscScalar f1[])
{
    for (PetscInt d = 0; d < dim; ++d)
        f1[d] = u_x[d];
}

static void
g3_temp(PetscInt dim,
        PetscInt Nf,
        PetscInt NfAux,
        const PetscInt uOff[],
        const PetscInt uOff_x[],
        const PetscScalar u[],
        const PetscScalar u_t[],
        const PetscScalar u_x[],
        const PetscInt aOff[],
        const PetscInt aOff_x[],
        const PetscScalar a[],
        const PetscScalar a_t[],
        const PetscScalar a_x[],
        PetscReal t,
        PetscReal u_tShift,
        const PetscReal x[],
        PetscInt numConstants,
        const PetscScalar constants[],
        PetscScalar g3[])
{
    for (PetscInt d = 0; d < dim; ++d)
        g3[d * dim + d] = 1.0;
}

static void
g0_temp(PetscInt dim,
        PetscInt Nf,
        PetscInt NfAux,
        const PetscInt uOff[],
        const PetscInt uOff_x[],
        const PetscScalar u[],
        const PetscScalar u_t[],
        const PetscScalar u_x[],
        const PetscInt aOff[],
        const PetscInt aOff_x[],
        const PetscScalar a[],
        const PetscScalar a_t[],
        const PetscScalar a_x[],
        PetscReal t,
        PetscReal u_tShift,
        const PetscReal x[],
        PetscInt numConstants,
        const PetscScalar constants[],
        PetscScalar g0[])
{
    g0[0] = u_tShift * 1.0;
}

InputParameters
HeatEquationProblem::validParams()
{
    InputParameters params = ImplicitFENonlinearProblem::validParams();
    return params;
}

HeatEquationProblem::HeatEquationProblem(const InputParameters & parameters) :
    ImplicitFENonlinearProblem(parameters),
    itemp(0)
{
    _F_;
}

HeatEquationProblem::~HeatEquationProblem() {}

void
HeatEquationProblem::onSetFields()
{
    _F_;
    PetscInt order = 1;
    addField(this->itemp, "temp", 1, order);
}

void
HeatEquationProblem::onSetWeakForm()
{
    _F_;
    setResidualBlock(this->itemp, f0_temp, f1_temp);
    setJacobianBlock(this->itemp, this->itemp, g0_temp, NULL, NULL, g3_temp);
}
