#include "godzilla/ValueFunctional.h"
#include "godzilla/CallStack.h"

namespace godzilla {

ValueFunctional::ValueFunctional(FEProblemInterface * fepi, const std::string & region) :
    Functional(fepi, region),
    evalr(fepi)
{
}

const std::set<std::string> &
ValueFunctional::get_provided_values() const
{
    _F_;
    return this->provides;
}

} // namespace godzilla
