// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/L2Diff.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"

namespace godzilla {

Parameters
L2Diff::parameters()
{
    auto params = Postprocessor::parameters();
    return params;
}

L2Diff::L2Diff(const Parameters & pars) : Postprocessor(pars), l2_diff(0.) {}

void
L2Diff::create()
{
    CALL_STACK_MSG();
}

void
L2Diff::compute()
{
    CALL_STACK_MSG();
    auto * problem = get_problem();
    std::vector<PetscFunc *> funcs(1, internal::invoke_function_delegate);
    std::vector<void *> ctxs(1, &this->delegate);
    PETSC_CHECK(DMComputeL2Diff(problem->get_dm(),
                                problem->get_time(),
                                funcs.data(),
                                ctxs.data(),
                                problem->get_solution_vector(),
                                &this->l2_diff));
}

std::vector<Real>
L2Diff::get_value()
{
    CALL_STACK_MSG();
    return { this->l2_diff };
}

void
L2Diff::evaluate(Real time, const Real x[], Scalar u[])
{
    CALL_STACK_MSG();
}

} // namespace godzilla
