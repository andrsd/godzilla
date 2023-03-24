#include "DependencyEvaluator.h"
#include "ValueFunctional.h"

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
    _F_;
    return this->functionals;
}

const ValueFunctional &
DependencyEvaluator::get_functional(const std::string & name) const
{
    _F_;
    const auto & it = this->functionals.find(name);
    if (it != this->functionals.end())
        return *it->second;
    else
        error("No functional with name '{}' found. Typo?", name);
}

} // namespace godzilla
