#include "Godzilla.h"
#include "L2Diff.h"
#include "CallStack.h"
#include "Problem.h"
#include "Types.h"

namespace godzilla {

REGISTER_OBJECT(L2Diff);

static PetscErrorCode
l2_diff_eval(PetscInt dim,
             PetscReal time,
             const PetscReal x[],
             PetscInt nc,
             PetscScalar u[],
             void * ctx)
{
    L2Diff * l2_diff = static_cast<L2Diff *>(ctx);
    l2_diff->evaluate(dim, time, x, nc, u);
    return 0;
}

Parameters
L2Diff::parameters()
{
    Parameters params = Postprocessor::parameters();
    params += FunctionInterface::parameters();
    return params;
}

L2Diff::L2Diff(const Parameters & params) :
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
    PetscFunc * funcs[1] = { l2_diff_eval };
    void * ctxs[1] = { this };
    PETSC_CHECK(DMComputeL2Diff(this->problem->get_dm(),
                                this->problem->get_time(),
                                funcs,
                                ctxs,
                                this->problem->get_solution_vector(),
                                &this->l2_diff));
}

PetscReal
L2Diff::get_value()
{
    _F_;
    return this->l2_diff;
}

void
L2Diff::evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[])
{
    _F_;
    FunctionInterface::evaluate(dim, time, x, nc, u);
}

} // namespace godzilla
