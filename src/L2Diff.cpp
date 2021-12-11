#include "Godzilla.h"
#include "L2Diff.h"
#include "CallStack.h"
#include "Problem.h"

namespace godzilla {

registerObject(L2Diff);

PetscErrorCode
__l2_diff(PetscInt dim,
          PetscReal time,
          const PetscReal x[],
          PetscInt nc,
          PetscScalar u[],
          void * ctx)
{
    L2Diff * l2_diff = (L2Diff *) ctx;
    l2_diff->evaluate(dim, time, x, nc, u);
    return 0;
}

InputParameters
L2Diff::validParams()
{
    InputParameters params = Postprocessor::validParams();
    params += FunctionInterface::validParams();
    return params;
}

L2Diff::L2Diff(const InputParameters & params) :
    Postprocessor(params),
    FunctionInterface(params),
    l2_diff(0.)
{
}

void
L2Diff::create()
{
    _F_;
    FunctionInterface::create();
}

void
L2Diff::compute()
{
    _F_;
    PetscErrorCode ierr;
    PetscReal time = 0;
    PetscFunc * funcs[1] = { __l2_diff };
    void * ctxs[1] = { this };
    ierr = DMComputeL2Diff(this->problem.getDM(),
                           time,
                           funcs,
                           ctxs,
                           this->problem.getSolutionVector(),
                           &this->l2_diff);
    checkPetscError(ierr);
}

PetscReal
L2Diff::getValue()
{
    _F_;
    return this->l2_diff;
}

void
L2Diff::evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[])
{
    _F_;
    for (PetscInt i = 0; i < nc; i++)
        u[i] = evaluateFunction(i, dim, time, x);
}

} // namespace godzilla
