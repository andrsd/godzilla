#include "godzilla/JacobianFunc.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/CallStack.h"

namespace godzilla {

JacobianFunc::JacobianFunc(FEProblemInterface * fepi, const std::string & region) :
    Functional(fepi, region)
{
}

const Real &
JacobianFunc::get_time_shift() const
{
    _F_;
    return get_fe_problem()->get_time_shift();
}

} // namespace godzilla
