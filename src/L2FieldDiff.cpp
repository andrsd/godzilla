// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/L2FieldDiff.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/ParsedFunction.h"
#include "godzilla/Types.h"
#include "godzilla/App.h"
#include "godzilla/FunctionDelegate.h"

namespace godzilla {

Parameters
L2FieldDiff::parameters()
{
    Parameters params = Postprocessor::parameters();
    params.add_param<std::map<std::string, std::vector<std::string>>>("functions", "");
    return params;
}

L2FieldDiff::L2FieldDiff(const Parameters & params) :
    Postprocessor(params),
    fepi(dynamic_cast<const FEProblemInterface *>(get_problem()))
{
    CALL_STACK_MSG();
    const auto & fn_map = get_param<std::map<std::string, std::vector<std::string>>>("functions");

    if (this->fepi != nullptr) {
        for (const auto & it : fn_map) {
            const auto & field_name = it.first;
            std::string nm = get_name() + "_" + field_name;

            const std::string class_name = "ParsedFunction";
            Parameters * fn_pars = get_app()->get_parameters(class_name);
            fn_pars->set<App *>("_app") = get_app();
            fn_pars->set<Problem *>("_problem") = get_problem();
            fn_pars->set<std::vector<std::string>>("function") = it.second;
            auto * pfn = get_app()->build_object<ParsedFunction>(class_name, nm, fn_pars);

            get_problem()->add_function(pfn);
            this->funcs[field_name] = pfn;
        }
    }
}

void
L2FieldDiff::create()
{
    CALL_STACK_MSG();
    auto field_names = this->fepi->get_field_names();
    auto n_fields = field_names.size();

    if (this->funcs.size() == n_fields) {
        this->l2_diff.resize(n_fields, 0.);

        for (const auto & it : this->funcs) {
            const auto & fld_name = it.first;
            if (!this->fepi->has_field_by_name(fld_name))
                log_error("Field '{}' does not exists. Typo?", fld_name);
        }
    }
    else
        log_error("Provided {} functions for {} fields. You must supply the same number of "
                  "functions as you have fields.",
                  this->funcs.size(),
                  n_fields);
}

void
L2FieldDiff::compute()
{
    CALL_STACK_MSG();
    auto n_fields = this->funcs.size();
    std::vector<PetscFunc *> pfns(n_fields, nullptr);
    std::vector<internal::FunctionMethodAbstract *> delegates(n_fields, nullptr);
    std::vector<Real> diff(n_fields, 0.);

    for (const auto & it : this->funcs) {
        Int fid = this->fepi->get_field_id(it.first);
        ParsedFunction * pfn = it.second;
        pfns[fid] = internal::invoke_function_method;
        delegates[fid] = new internal::FunctionMethod(pfn, &ParsedFunction::evaluate);
    }

    auto * problem = get_problem();
    PETSC_CHECK(DMComputeL2FieldDiff(problem->get_dm(),
                                     problem->get_time(),
                                     pfns.data(),
                                     reinterpret_cast<void **>(delegates.data()),
                                     problem->get_solution_vector(),
                                     diff.data()));

    this->l2_diff = diff;

    for (auto & d : delegates)
        delete d;
}

Real
L2FieldDiff::get_value()
{
    CALL_STACK_MSG();
    return 0;
}

} // namespace godzilla
