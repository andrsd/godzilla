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
}

void
L2FieldDiff::create()
{
    CALL_STACK_MSG();
    this->l2_diff.resize(this->n_fields);
}

void
L2FieldDiff::compute()
{
    CALL_STACK_MSG();
    assert_true(this->delegates.size() > 0, "No evaluation function(s) set");

    std::vector<PetscFunc *> funcs(this->n_fields, nullptr);
    // std::vector<Real> diff(this->n_fields, 0.);
#if 0
    for (const auto & [name, fn] : this->parsed_fns) {
        auto fid = this->fepi->get_field_id(name);
        funcs[fid.value()] = internal::invoke_function_delegate;
    }
    std::vector<void *> contexts;
    contexts.reserve(n_fields);
    for (auto & d : delegates) {
        if (d)
            contexts.push_back(&d);
        else
            contexts.push_back(nullptr);
    }
    auto * problem = get_problem();
    PETSC_CHECK(DMComputeL2FieldDiff(problem->get_dm(),
                                     problem->get_time(),
                                     funcs.data(),
                                     contexts.data(),
                                     problem->get_solution_vector(),
                                     this->l2_diff.data()));
#endif
}

std::vector<Real>
L2FieldDiff::get_value()
{
    CALL_STACK_MSG();
    return this->l2_diff;
}

} // namespace godzilla
