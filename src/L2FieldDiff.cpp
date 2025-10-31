// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/L2FieldDiff.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/Types.h"

namespace godzilla {

Parameters
L2FieldDiff::parameters()
{
    auto params = Postprocessor::parameters();
    return params;
}

L2FieldDiff::L2FieldDiff(const Parameters & pars) :
    Postprocessor(pars),
    fepi(dynamic_cast<const FEProblemInterface *>(get_problem())),
    n_fields(0)
{
    CALL_STACK_MSG();
    assert_true(this->fepi != nullptr, "FEProblemInterface is null");
}

void
L2FieldDiff::create()
{
    CALL_STACK_MSG();
    this->n_fields = this->fepi->get_num_fields();
    this->l2_diff.resize(this->n_fields, 0.);
    set_up_callbacks();
}

void
L2FieldDiff::compute()
{
    CALL_STACK_MSG();
    assert_true(this->n_fields > 0, "No fields to evaluate");
    assert_true(this->delegates.size() > 0, "No evaluation function(s) set");

    std::vector<PetscFunc *> funcs(this->n_fields, nullptr);
    std::vector<void *> contexts(this->n_fields, nullptr);
    for (auto & [fid, d] : this->delegates) {
        if (d) {
            assert_true(fid >= 0 && fid < this->n_fields,
                        fmt::format("Field ID ({}) is out of range [0, {})", fid, this->n_fields));
            contexts[fid] = &d;
            funcs[fid] = internal::invoke_function_delegate;
        }
    }
    auto * problem = get_problem();
    PETSC_CHECK(DMComputeL2FieldDiff(problem->get_dm(),
                                     problem->get_time(),
                                     funcs.data(),
                                     contexts.data(),
                                     problem->get_solution_vector(),
                                     this->l2_diff.data()));
}

std::vector<Real>
L2FieldDiff::get_value()
{
    CALL_STACK_MSG();
    return this->l2_diff;
}

} // namespace godzilla
