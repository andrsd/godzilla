#include "godzilla/BndResidualFunc.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/NaturalBC.h"
#include "godzilla/CallStack.h"

namespace godzilla {

BndResidualFunc::BndResidualFunc(const BoundaryCondition * bc) :
    ResidualFunc(dynamic_cast<FEProblemInterface *>(bc->get_discrete_problem_interface()))
{
}

const Normal &
BndResidualFunc::get_normal() const
{
    _F_;
    return get_fe_problem()->get_normal();
}

} // namespace godzilla
