#pragma once

#include "GodzillaConfig.h"
#include "ResidualFunc.h"
#include "FieldValue.h"
#include <string>

namespace godzilla {

class NaturalBC;
class FEProblemInterface;

class BndResidualFunc : public ResidualFunc {
public:
    explicit BndResidualFunc(const NaturalBC * nbc);

protected:
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

    /// Get normal
    ///
    /// @return Outward normal
    NO_DISCARD const Normal & get_normal() const;
};

} // namespace godzilla
