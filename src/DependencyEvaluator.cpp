// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/DependencyEvaluator.h"
#include "godzilla/ValueFunctional.h"

namespace godzilla {

DependencyEvaluator::~DependencyEvaluator()
{
    for (auto & [_, val] : this->values)
        delete val;
    for (auto & [_, fnl] : this->functionals)
        delete fnl;
}

const std::map<String, const ValueFunctional *> &
DependencyEvaluator::get_functionals() const
{
    CALL_STACK_MSG();
    return this->functionals;
}

const ValueFunctional &
DependencyEvaluator::get_functional(String name) const
{
    CALL_STACK_MSG();
    const auto & it = this->functionals.find(name);
    expect_true(it != this->functionals.end(), "No functional with name '{}' found. Typo?", name);
    return *it->second;
}

std::map<String, const ValueFunctional *>
DependencyEvaluator::get_suppliers() const
{
    CALL_STACK_MSG();
    std::map<String, const ValueFunctional *> suppliers;
    for (auto & [_, fnl] : get_functionals()) {
        auto provides = fnl->get_provided_values();
        for (auto & s : provides) {
            expect_true(suppliers.find(s) == suppliers.end(),
                        "Value '{}' is being supplied multiple times.",
                        s);
            suppliers[s] = fnl;
        }
    }
    return suppliers;
}

DependencyGraph<const Functional *>
DependencyEvaluator::build_dependecy_graph(
    const std::map<String, const ValueFunctional *> & suppliers)
{
    CALL_STACK_MSG();
    DependencyGraph<const Functional *> graph;
    for (auto & [_, fnl] : get_functionals()) {
        auto depends_on = fnl->get_dependent_values();
        for (auto & dep : depends_on) {
            auto jt = suppliers.find(dep);
            expect_true(jt != suppliers.end(),
                        "Did not find any functional which would supply '{}'.",
                        dep);
            graph.add_edge(fnl, jt->second);
        }
    }
    return graph;
}

} // namespace godzilla
