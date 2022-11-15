#include "ResidualFunc.h"
#include "FEProblemInterface.h"
#include "CallStack.h"

namespace godzilla {

ResidualFunc::ResidualFunc(const FEProblemInterface * fepi) : fepi(fepi) {}

const FEProblemInterface *
ResidualFunc::get_fe_problem() const
{
    _F_;
    return this->fepi;
}

const PetscInt &
ResidualFunc::get_spatial_dimension() const
{
    _F_;
    return this->fepi->get_spatial_dimension();
}

const PetscScalar *
ResidualFunc::get_field_value(const std::string & field_name) const
{
    _F_;
    return this->fepi->get_field_value(field_name);
}

const PetscScalar *
ResidualFunc::get_field_gradient(const std::string & field_name) const
{
    _F_;
    return this->fepi->get_field_gradient(field_name);
}

const PetscScalar *
ResidualFunc::get_field_dot(const std::string & field_name) const
{
    _F_;
    return this->fepi->get_field_dot(field_name);
}

const PetscReal &
ResidualFunc::get_time() const
{
    _F_;
    return this->fepi->get_time();
}

} // namespace godzilla
