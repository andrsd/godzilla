#pragma once

#include "Functional.h"
#include "FieldValue.h"
#include <string>
#include "petsc.h"

namespace godzilla {

class FEProblemInterface;

class ResidualFunc : public Functional {
public:
    explicit ResidualFunc(const FEProblemInterface * fepi);

protected:
    /// Get spatial dimension
    ///
    /// @return Spatial dimension
    const PetscInt & get_spatial_dimension() const;

    /// Get values of a field
    ///
    /// @param field_name The name of the field
    /// @return Reference to a class that contains the field values
    const FieldValue & get_field_value(const std::string & field_name) const;

    /// Get values of a gradient of a field
    ///
    /// @param field_name The name of the field
    /// @return Pointer to array that contains the field gradient values
    const FieldGradient & get_field_gradient(const std::string & field_name) const;

    /// Get values of a time derivative of a field
    ///
    /// @param field_name The name of the field
    /// @return Reference to a class that contains the field time derivative values
    const FieldValue & get_field_dot(const std::string & field_name) const;

    /// Get time at which the function is evaluated
    ///
    /// @return Time at which is the function evaluated
    const PetscReal & get_time() const;
};

} // namespace godzilla
