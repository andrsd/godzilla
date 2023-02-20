#include "BndResidualFunc.h"
#include "FEProblemInterface.h"
#include "NaturalBC.h"
#include "CallStack.h"

namespace godzilla {

BndResidualFunc::BndResidualFunc(const NaturalBC * nbc) :
    ResidualFunc(dynamic_cast<const FEProblemInterface *>(nbc->get_discrete_problem_interface()))
{
}

const Normal &
BndResidualFunc::get_normal() const
{
    _F_;
    return get_fe_problem()->get_normal();
}

const Point &
BndResidualFunc::get_xyz() const
{
    _F_;
    return get_fe_problem()->get_xyz();
}

} // namespace godzilla
