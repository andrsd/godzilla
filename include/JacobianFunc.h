#pragma once

#include "GodzillaConfig.h"
#include "Functional.h"
#include "FieldValue.h"
#include <string>
#include "petsc.h"

namespace godzilla {

class FEProblemInterface;

class JacobianFunc : public Functional {
public:
    explicit JacobianFunc(const FEProblemInterface * fepi);

protected:
    /// Get spatial dimension
    ///
    /// @return Spatial dimension
    NO_DISCARD const PetscInt & get_spatial_dimension() const;

    /// Get values of a field
    ///
    /// @param field_name The name of the field
    /// @return Pointer to array that contains the field values
    NO_DISCARD const FieldValue & get_field_value(const std::string & field_name) const;

    /// Get values of a gradient of a field
    ///
    /// @param field_name The name of the field
    /// @return Pointer to array that contains the field gradient values
    NO_DISCARD const FieldGradient & get_field_gradient(const std::string & field_name) const;

    /// Get the multiplier a for dF/dU_t
    ///
    /// @return The multiplier a for dF/dU_t
    NO_DISCARD const PetscReal & get_time_shift() const;

    /// Get time at which the function is evaluated
    ///
    /// @return Time at which is the function evaluated
    NO_DISCARD const PetscReal & get_time() const;
};

} // namespace godzilla
