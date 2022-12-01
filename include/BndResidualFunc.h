#pragma once

#include "GodzillaConfig.h"
#include "Functional.h"
#include "FieldValue.h"
#include <string>
#include "petsc.h"

namespace godzilla {

class NaturalBC;
class FEProblemInterface;

class BndResidualFunc : public Functional {
public:
    explicit BndResidualFunc(const NaturalBC * nbc);

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

    /// Get values of a time derivative of a field
    ///
    /// @param field_name The name of the field
    /// @return Pointer to array that contains the field time derivative values
    NO_DISCARD const FieldValue & get_field_dot(const std::string & field_name) const;

    /// Get time at which the function is evaluated
    ///
    /// @return Time at which is the function evaluated
    NO_DISCARD const PetscReal & get_time() const;

    /// Get normal
    ///
    /// @return Outward normal
    NO_DISCARD const Vector & get_normal() const;

    /// Get physical coordinates
    ///
    /// @return Physical coordinates
    NO_DISCARD const Point & get_xyz() const;
};

} // namespace godzilla
