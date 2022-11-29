#pragma once

#include "GodzillaConfig.h"
#include <string>
#include "petsc.h"

namespace godzilla {

class FEProblemInterface;

class JacobianFunc {
public:
    explicit JacobianFunc(const FEProblemInterface * fepi);

    /// Evaluate this Jacobian function
    ///
    /// @param val Array to store the evaluated Jacobian into
    virtual void evaluate(PetscScalar val[]) = 0;

protected:
    /// Get pointer to FEProblemInterface
    ///
    /// @return Pointer to FEProblemInterface
    NO_DISCARD const FEProblemInterface * get_fe_problem() const;

    /// Get spatial dimension
    ///
    /// @return Spatial dimension
    NO_DISCARD const PetscInt & get_spatial_dimension() const;

    /// Get values of a field
    ///
    /// @param field_name The name of the field
    /// @return Pointer to array that contains the field values
    NO_DISCARD const PetscScalar * get_field_value(const std::string & field_name) const;

    /// Get values of a gradient of a field
    ///
    /// @param field_name The name of the field
    /// @return Pointer to array that contains the field gradient values
    NO_DISCARD const PetscScalar * get_field_gradient(const std::string & field_name) const;

    /// Get the multiplier a for dF/dU_t
    ///
    /// @return The multiplier a for dF/dU_t
    NO_DISCARD const PetscReal & get_time_shift() const;

    /// Get time at which the function is evaluated
    ///
    /// @return Time at which is the function evaluated
    NO_DISCARD const PetscReal & get_time() const;

private:
    const FEProblemInterface * fepi;
};

} // namespace godzilla
