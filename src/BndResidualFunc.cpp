#include "BndResidualFunc.h"
#include "FEProblemInterface.h"
#include "NaturalBC.h"
#include "CallStack.h"

namespace godzilla {

BndResidualFunc::BndResidualFunc(const BoundaryCondition * bc) :
    ResidualFunc(dynamic_cast<const FEProblemInterface *>(bc->get_discrete_problem_interface()))
{
}

const Normal &
BndResidualFunc::get_normal() const
{
    _F_;
    return get_fe_problem()->get_normal();
}

} // namespace godzilla
