// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/L2Diff.h"
#include "godzilla/CallStack.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/Problem.h"

namespace godzilla {

static ErrorCode
L2Diff__invoke_delegate(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
{
    CALL_STACK_MSG();
    auto * bc = static_cast<L2Diff *>(ctx);
    assert_true(bc != nullptr, "Pointer to L2Diff is null");
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
    std::vector<PetscFunc *> funcs(1, L2Diff__invoke_delegate);
    std::vector<void *> ctxs(1, this);
    std::cerr << "L2Diff::compute()" << std::endl;

    auto sln = problem->get_solution_vector();
    for (Int i = 0; i < sln.get_local_size(); ++i) {
        fmt::println("{:.12f}", sln(i));
    }

    auto * fepi = dynamic_cast<FEProblemInterface *>(problem);
    auto loc_sln = fepi->get_solution_vector_local();
    loc_sln.zero();
    loc_sln.assemble();
    std::cerr << "loc sln" << std::endl;
    for (Int i = 0; i < loc_sln.get_local_size(); ++i) {
        fmt::println("{:.12f}", loc_sln(i));
    }

    PETSC_CHECK(DMComputeL2Diff(problem->get_dm(),
                                problem->get_time(),
                                funcs.data(),
                                ctxs.data(),
                                problem->get_solution_vector(),
                                &this->l2_diff));
    std::cerr << "ls = " << this->l2_diff << std::endl;
    std::cerr << "L2Diff::compute() - end" << std::endl;
}

std::vector<Real>
L2Diff::get_value()
{
    CALL_STACK_MSG();
    return { this->l2_diff };
}

} // namespace godzilla
