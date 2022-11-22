#include "JacobianFunc.h"
#include "FEProblemInterface.h"
#include "CallStack.h"

namespace godzilla {

JacobianFunc::JacobianFunc(const FEProblemInterface * fepi) : fepi(fepi) {}

const FEProblemInterface *
JacobianFunc::get_fe_problem() const
{
    _F_;
    return this->fepi;
}

const PetscInt &
JacobianFunc::get_spatial_dimension() const
{
    _F_;
    return this->fepi->get_spatial_dimension();
}

const PetscScalar *
JacobianFunc::get_field_value(const std::string & field_name) const
{
    _F_;
    return this->fepi->get_field_value(field_name);
}

const PetscScalar *
JacobianFunc::get_field_gradient(const std::string & field_name) const
{
    _F_;
    return this->fepi->get_field_gradient(field_name);
}

const PetscReal &
JacobianFunc::get_time_shift() const
{
    _F_;
    return this->fepi->get_time_shift();
}

const PetscReal &
JacobianFunc::get_time() const
{
    _F_;
    return this->fepi->get_time();
}

} // namespace godzilla
