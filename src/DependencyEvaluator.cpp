// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/DependencyEvaluator.h"
#include "godzilla/ValueFunctional.h"

namespace godzilla {

DependencyEvaluator::DependencyEvaluator() {}

DependencyEvaluator::~DependencyEvaluator()
{
    for (auto & kv : this->values)
        delete kv.second;
    for (auto & kv : this->functionals)
        delete kv.second;
}

const std::map<std::string, const ValueFunctional *> &
DependencyEvaluator::get_functionals() const
{
    CALL_STACK_MSG();
    return this->functionals;
}

const ValueFunctional &
DependencyEvaluator::get_functional(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->functionals.find(name);
    if (it != this->functionals.end())
        return *it->second;
    else
        throw Exception("No functional with name '{}' found. Typo?", name);
}

std::map<std::string, const ValueFunctional *>
DependencyEvaluator::get_suppliers() const
{
    CALL_STACK_MSG();
    std::map<std::string, const ValueFunctional *> suppliers;
    for (auto & it : get_functionals()) {
        auto fnl = it.second;
        auto provides = fnl->get_provided_values();
        for (auto & s : provides) {
            if (suppliers.find(s) == suppliers.end())
                suppliers[s] = fnl;
            else
                throw Exception("Value '{}' is being supplied multiple times.", s);
        }
    }
    return suppliers;
}

DependencyGraph<const Functional *>
DependencyEvaluator::build_dependecy_graph(
    const std::map<std::string, const ValueFunctional *> & suppliers)
{
    CALL_STACK_MSG();
    DependencyGraph<const Functional *> graph;
    for (auto & it : get_functionals()) {
        auto fnl = it.second;
        auto depends_on = fnl->get_dependent_values();
        for (auto & dep : depends_on) {
            auto jt = suppliers.find(dep);
            if (jt != suppliers.end())
                graph.add_edge(fnl, jt->second);
            else
                throw Exception("Did not find any functional which would supply '{}'.", dep);
        }
    }
    return graph;
}

} // namespace godzilla
