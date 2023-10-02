#include "Godzilla.h"
#include "L2Diff.h"
#include "CallStack.h"
#include "Problem.h"
#include "Types.h"

namespace godzilla {

REGISTER_OBJECT(L2Diff);

static PetscErrorCode
l2_diff_eval(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
{
    auto * l2_diff = static_cast<L2Diff *>(ctx);
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
    std::vector<PetscFunc *> funcs(1, l2_diff_eval);
    std::vector<void *> ctxs(1, this);
    PETSC_CHECK(DMComputeL2Diff(this->problem->dm(),
                                this->problem->get_time(),
                                funcs.data(),
                                ctxs.data(),
                                this->problem->get_solution_vector(),
                                &this->l2_diff));
}

Real
L2Diff::get_value()
{
    _F_;
    return this->l2_diff;
}

void
L2Diff::evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[])
{
    _F_;
    evaluate_func(dim, time, x, nc, u);
}

} // namespace godzilla
