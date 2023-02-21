#include "ResidualFunc.h"

namespace godzilla {

ResidualFunc::ResidualFunc(const FEProblemInterface * fepi, const std::string & region) :
    Functional(const_cast<FEProblemInterface *>(fepi), region)
{
}

const Point &
ResidualFunc::get_xyz() const
{
    _F_;
    return get_fe_problem()->get_xyz();
}

} // namespace godzilla
