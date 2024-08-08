// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Label.h"
#include "petscdstypes.h"
#include <array>
#include <map>
#include <vector>

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

class ResidualFunc;
class JacobianFunc;

/// Weak formulation
///
class WeakForm {
public:
    enum ResidualKind {
        F0 = PETSC_WF_F0,
        F1 = PETSC_WF_F1,
        BND_F0 = PETSC_WF_BDF0,
        BND_F1 = PETSC_WF_BDF1
    };

    enum JacobianKind {
        G0 = PETSC_WF_G0,
        G1 = PETSC_WF_G1,
        G2 = PETSC_WF_G2,
        G3 = PETSC_WF_G3,
        GP0 = PETSC_WF_GP0,
        GP1 = PETSC_WF_GP1,
        GP2 = PETSC_WF_GP2,
        GP3 = PETSC_WF_GP3,
        GT0 = PETSC_WF_GT0,
        GT1 = PETSC_WF_GT1,
        GT2 = PETSC_WF_GT2,
        GT3 = PETSC_WF_GT3,
        BND_G0 = PETSC_WF_BDG0,
        BND_G1 = PETSC_WF_BDG1,
        BND_G2 = PETSC_WF_BDG2,
        BND_G3 = PETSC_WF_BDG3,
    };

    WeakForm();

    /// Get residual keys
    ///
    /// FIXME: needs a better name
    [[nodiscard]] std::vector<PetscFormKey> get_residual_keys() const;

    /// Get Jacobian keys
    ///
    /// FIXME: needs a better name
    [[nodiscard]] std::vector<PetscFormKey> get_jacobian_keys() const;

    /// Get residual forms
    [[nodiscard]] const std::vector<ResidualFunc *> &
    get(ResidualKind kind, const Label & label, Int val, Int f, Int part) const;

    /// Get Jacobian forms
    [[nodiscard]] const std::vector<JacobianFunc *> &
    get(JacobianKind kind, const Label & label, Int val, Int f, Int g, Int part) const;

    /// Add a residual form
    ///
    /// @param kind Kind of the form
    /// @param label Label where the form is defined
    /// @param val Value
    /// @param f Field ID
    /// @param part Part
    /// @param func Functional representing a boundary or a volumetric residual form
    void add(ResidualKind kind, const Label & label, Int val, Int f, Int part, ResidualFunc * func);

    /// Add a Jacobian form
    ///
    /// @param kind Kind of the form
    /// @param label Label where the form is defined
    /// @param val Value
    /// @param f Field ID (test)
    /// @param g Field ID (base)
    /// @param part Part
    /// @param func Functional representing a boundary or a volumetric Jacobian form
    void add(JacobianKind kind,
             const Label & label,
             Int val,
             Int f,
             Int g,
             Int part,
             JacobianFunc * func);

    /// Query if Jacobian statement is set
    ///
    /// @return `true` if weak form for Jacobian statement is set, otherwise `false`
    [[nodiscard]] bool has_jacobian() const;

    /// Query if Jacobian preconditioner statement is set
    ///
    /// @return `true` if weak form for Jacobian preconditioner statement is set, otherwise `false`
    [[nodiscard]] bool has_jacobian_preconditioner() const;

    /// Get field ID for the combination of fields `f` and `g`
    ///
    /// @param f Field `f` ID
    /// @param g Field `g` ID
    /// @return Field ID used in `PetscFormKey`
    [[nodiscard]] Int get_jac_key(Int f, Int g) const;

private:
    /// Number of fields
    Int n_fields;

    /// All residual forms
    std::array<std::map<PetscFormKey, std::vector<ResidualFunc *>>, PETSC_NUM_WF> res_forms;

    /// Empty array for residual forms
    std::vector<ResidualFunc *> empty_res_forms;

    /// All Jacobian forms
    std::array<std::map<PetscFormKey, std::vector<JacobianFunc *>>, PETSC_NUM_WF> jac_forms;

    /// Empty array for Jacobian forms
    std::vector<JacobianFunc *> empty_jac_forms;
};

} // namespace godzilla
