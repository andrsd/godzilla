#include "JacobianFunc.h"
#include "FEProblemInterface.h"
#include "CallStack.h"

namespace godzilla {

JacobianFunc::JacobianFunc(const FEProblemInterface * fepi, const std::string & region) :
    Functional(const_cast<FEProblemInterface *>(fepi), region)
{
}

const Real &
JacobianFunc::get_time_shift() const
{
    _F_;
    return get_fe_problem()->get_time_shift();
}

} // namespace godzilla
