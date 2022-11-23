#pragma once

#include <string>
#include "petsc.h"

namespace godzilla {

class FEProblemInterface;
class NaturalBC;

class BndJacobianFunc {
public:
    explicit BndJacobianFunc(const NaturalBC * nbc);

    /// Evaluate this Jacobian function
    ///
    /// @param val Array to store the evaluated Jacobian into
    virtual void evaluate(PetscScalar val[]) = 0;

protected:
    /// Get pointer to FEProblemInterface
    ///
    /// @return Pointer to FEProblemInterface
    const FEProblemInterface * get_fe_problem();

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

    /// Get the multiplier a for dF/dU_t
    ///
    /// @return The multiplier a for dF/dU_t
    const PetscReal & get_time_shift() const;

    /// Get time at which the function is evaluated
    ///
    /// @return Time at which is the function evaluated
    const PetscReal & get_time() const;

    /// Get normal
    ///
    /// @return Outward normal
    PetscReal * const & get_normal() const;

    /// Get physical coordinates
    ///
    /// @return Physical coordinates
    PetscReal * const & get_xyz() const;

private:
    const FEProblemInterface * fepi;
};

} // namespace godzilla
