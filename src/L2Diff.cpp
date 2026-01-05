// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/L2Diff.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "petscdmplex.h"

namespace godzilla {

static PetscErrorCode
L2Diff__invoke_delegate(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
{
    CALL_STACK_MSG();
    auto * bc = static_cast<L2Diff *>(ctx);
    GODZILLA_ASSERT_TRUE(bc != nullptr, "Pointer to L2Diff is null");
    bc->evaluate(time, x, u);
    return 0;
}

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
    auto * dpi = dynamic_cast<DiscreteProblemInterface *>(problem);
    GODZILLA_ASSERT_TRUE(dpi != nullptr, "Discrete problem is null");
    std::vector<PetscFunc *> funcs(1, L2Diff__invoke_delegate);
    std::vector<void *> ctxs(1, this);
    dpi->compute_solution_vector_local();
    PETSC_CHECK(DMPlexComputeL2DiffLocal(problem->get_dm(),
                                         problem->get_time(),
                                         funcs.data(),
                                         ctxs.data(),
                                         dpi->get_solution_vector_local(),
                                         &this->l2_diff));
}

std::vector<Real>
L2Diff::get_value()
{
    CALL_STACK_MSG();
    return { this->l2_diff };
}

} // namespace godzilla
