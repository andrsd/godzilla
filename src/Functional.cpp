#include "Functional.h"
#include "CallStack.h"
#include "FEProblemInterface.h"

namespace godzilla {

Functional::Functional(const FEProblemInterface * fepi) : fepi(fepi) {}

const FEProblemInterface *
Functional::get_fe_problem() const
{
    _F_;
    return this->fepi;
}

} // namespace godzilla
