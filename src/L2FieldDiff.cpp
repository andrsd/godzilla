#include "Godzilla.h"
#include "L2FieldDiff.h"
#include "CallStack.h"
#include "Problem.h"
#include "FEProblemInterface.h"
#include "ParsedFunction.h"
#include "Types.h"

namespace godzilla {

REGISTER_OBJECT(L2FieldDiff);

Parameters
L2FieldDiff::parameters()
{
    Parameters params = Postprocessor::parameters();
    params.add_param<std::map<std::string, std::vector<std::string>>>("functions", "");
    return params;
}

L2FieldDiff::L2FieldDiff(const Parameters & params) :
    Postprocessor(params),
    fepi(dynamic_cast<const FEProblemInterface *>(this->problem))
{
    _F_;
    const auto & fn_map = get_param<std::map<std::string, std::vector<std::string>>>("functions");

    if (this->fepi != nullptr) {
        for (const auto & it : fn_map) {
            const auto & field_name = it.first;
            std::string nm = get_name() + "_" + field_name;

            const std::string class_name = "ParsedFunction";
            Parameters * fn_pars = Factory::get_parameters(class_name);
            fn_pars->set<const App *>("_app") = this->app;
            fn_pars->set<Problem *>("_problem") = this->problem;
            fn_pars->set<std::vector<std::string>>("function") = it.second;
            auto * pfn = Factory::create<ParsedFunction>(class_name, nm, fn_pars);

            const_cast<Problem *>(this->problem)->add_function(pfn);
            this->funcs[field_name] = pfn;
        }
    }
}

void
L2FieldDiff::create()
{
    _F_;
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
    _F_;
    auto n_fields = this->funcs.size();
    std::vector<PetscFunc *> pfns(n_fields, nullptr);
    std::vector<void *> ctxs(n_fields, nullptr);
    std::vector<Real> diff(n_fields, 0.);

    for (const auto & it : this->funcs) {
        Int fid = this->fepi->get_field_id(it.first);
        ParsedFunction * pfn = it.second;

        pfns[fid] = pfn->get_function();
        ctxs[fid] = pfn->get_context();
    }

    PETSC_CHECK(DMComputeL2FieldDiff(this->problem->dm(),
                                     this->problem->get_time(),
                                     pfns.data(),
                                     ctxs.data(),
                                     this->problem->get_solution_vector(),
                                     diff.data()));

    for (Int i = 0; i < n_fields; i++) {
        this->l2_diff[i] = diff[i];
    }
}

Real
L2FieldDiff::get_value()
{
    _F_;
    return 0;
}

} // namespace godzilla
