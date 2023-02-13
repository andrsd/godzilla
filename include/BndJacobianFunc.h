#pragma once

#include "GodzillaConfig.h"
#include "JacobianFunc.h"
#include "FieldValue.h"
#include <string>

namespace godzilla {

class FEProblemInterface;
class NaturalBC;

class BndJacobianFunc : public JacobianFunc {
public:
    explicit BndJacobianFunc(const NaturalBC * nbc);

protected:
    /// Get normal
    ///
    /// @return Outward normal
    NO_DISCARD const Normal & get_normal() const;

    /// Get physical coordinates
    ///
    /// @return Physical coordinates
    NO_DISCARD const Point & get_xyz() const;
};

} // namespace godzilla
