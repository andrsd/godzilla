// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/L2Diff.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/Types.h"
#include "godzilla/Delegate.h"

namespace godzilla {

Parameters
L2Diff::parameters()
{
    auto params = Postprocessor::parameters();
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
    CALL_STACK_MSG();
    FunctionInterface::create();
}

void
L2Diff::compute()
{
    CALL_STACK_MSG();
    auto problem = get_problem();
    FunctionDelegate delegate(this, &L2Diff::evaluate);
    std::vector<PetscFunc *> funcs(1, internal::invoke_function_delegate);
    std::vector<void *> ctxs(1, &delegate);
    PETSC_CHECK(DMComputeL2Diff(problem->get_dm(),
                                problem->get_time(),
                                funcs.data(),
                                ctxs.data(),
                                problem->get_solution_vector(),
                                &this->l2_diff));
}

Real
L2Diff::get_value()
{
    CALL_STACK_MSG();
    return this->l2_diff;
}

void
L2Diff::evaluate(Real time, const Real x[], Scalar u[])
{
    CALL_STACK_MSG();
    evaluate_func(time, x, FunctionInterface::get_num_components(), u);
}

} // namespace godzilla
