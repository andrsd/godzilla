#include "BndJacobianFunc.h"
#include "FEProblemInterface.h"
#include "NaturalBC.h"
#include "CallStack.h"

namespace godzilla {

BndJacobianFunc::BndJacobianFunc(const BoundaryCondition * bc) :
    JacobianFunc(dynamic_cast<FEProblemInterface *>(bc->get_discrete_problem_interface()))
{
}

const Normal &
BndJacobianFunc::get_normal() const
{
    _F_;
    return get_fe_problem()->get_normal();
}

} // namespace godzilla
