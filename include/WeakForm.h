#pragma once

#include "GodzillaConfig.h"
#include "Types.h"
#include <array>
#include <map>
#include <vector>
#include "petsc.h"

namespace std {

template <>
struct less<PetscFormKey> {
    bool
    operator()(const PetscFormKey & lhs, const PetscFormKey & rhs) const
    {
        if (lhs.label == rhs.label) {
            if (lhs.value == rhs.value) {
                if (lhs.field == rhs.field) {
                    return lhs.part < rhs.part;
                }
                else
                    return lhs.field < rhs.field;
            }
            else
                return lhs.value < rhs.value;
        }
        else
            return lhs.label < rhs.label;
    }
};

} // namespace std

namespace godzilla {

class Functional;

/// Weak formulation
///
class WeakForm {
public:
    WeakForm();

    /// Get residual keys
    ///
    /// FIXME: needs a better name
    std::vector<PetscFormKey> get_residual_keys() const;

    /// Get Jacobian keys
    ///
    /// FIXME: needs a better name
    std::vector<PetscFormKey> get_jacobian_keys() const;

    /// Get residual forms
    const std::vector<Functional *> &
    get(PetscWeakFormKind kind, DMLabel label, Int val, Int f, Int part) const;

    /// Get Jacobian forms
    const std::vector<Functional *> &
    get(PetscWeakFormKind kind, DMLabel label, Int val, Int f, Int g, Int part) const;

    /// Add a residual form
    ///
    /// @param kind Kind of the form
    /// @param label DMLabel where the form is defined
    /// @param val Value
    /// @param f Field ID
    /// @param part Part
    /// @param func Functional representing a boundary or a volumetric residual form
    void add(PetscWeakFormKind kind, DMLabel label, Int val, Int f, Int part, Functional * func);

    /// Add a Jacobian form
    ///
    /// @param kind Kind of the form
    /// @param label DMLabel where the form is defined
    /// @param val Value
    /// @param f Field ID (test)
    /// @param g Field ID (base)
    /// @param part Part
    /// @param func Functional representing a boundary or a volumetric Jacobian form
    void
    add(PetscWeakFormKind kind, DMLabel label, Int val, Int f, Int g, Int part, Functional * func);

protected:
    NO_DISCARD Int get_jac_key(Int f, Int g) const;

    /// Number of fields
    Int n_fields;

    /// All residual forms
    std::array<std::map<PetscFormKey, std::vector<Functional *>>, PETSC_NUM_WF> res_forms;

    /// Empty array for residual forms
    std::vector<Functional *> empty_res_forms;

    /// All Jacobian forms
    std::array<std::map<PetscFormKey, std::vector<Functional *>>, PETSC_NUM_WF> jac_forms;

    /// Empty array for Jacobian forms
    std::vector<Functional *> empty_jac_forms;
};

} // namespace godzilla
