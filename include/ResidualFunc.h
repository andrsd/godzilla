#pragma once

#include <string>
#include "petsc.h"

namespace godzilla {

class FEProblemInterface;

class ResidualFunc {
public:
    ResidualFunc(const FEProblemInterface * fepi);

    /// Evaluate this residual function
    ///
    /// @param val Array to store the evaluated residual into
    virtual void evaluate(PetscScalar val[]) = 0;

protected:
    /// Get pointer to FEProblemInterface
    ///
    /// @return Pointer to FEProblemInterface
    const FEProblemInterface * get_fe_problem() const;

    /// Get spatial dimension
    ///
    /// @return Spatial dimension
    const PetscInt & get_spatial_dimension() const;

    /// Get values of a field
    ///
    /// @param field_name The name of the field
    /// @return Pointer to array that contains the field values
    const PetscScalar * get_field_value(const std::string & field_name) const;

    /// Get values of a gradient of a field
    ///
    /// @param field_name The name of the field
    /// @return Pointer to array that contains the field gradient values
    const PetscScalar * get_field_gradient(const std::string & field_name) const;

    /// Get values of a time derivative of a field
    ///
    /// @param field_name The name of the field
    /// @return Pointer to array that contains the field time derivative values
    const PetscScalar * get_field_dot(const std::string & field_name) const;

    /// Get time at which the function is evaluated
    ///
    /// @return Time at which is the function evaluated
    const PetscReal & get_time() const;

private:
    /// FEProblemInterface this function is part of
    const FEProblemInterface * fepi;
};

} // namespace godzilla
