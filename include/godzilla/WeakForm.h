// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Label.h"
#include "petscdstypes.h"
#include <array>
#include <map>
#include <vector>

namespace godzilla {

class ResidualFunc;
class JacobianFunc;

/// Weak formulation
///
class WeakForm {
public:
    /// Region of the mesh where parts of a weak form can be specified
    struct Region {
        Label label;
        Int value;
        Int part;

        Region() : label(nullptr), value(0), part(0) {}
        Region(const Label & label, Int value, Int part) : label(label), value(value), part(0) {}
        Region(DMLabel label, Int value, Int part) : label(label), value(value), part(0) {}

        bool
        operator<(const Region & other) const
        {
            if ((DMLabel) this->label == (DMLabel) other.label)
                if (this->value == other.value)
                    return this->part < other.part;
                else
                    return this->value < other.value;
            else
                return (DMLabel) this->label < (DMLabel) other.label;
        }
    };

    struct JacobianFieldID {
#if defined(PETSC_USE_64BIT_INDICES)
        unsigned field_i : 32;
        unsigned field_j : 32;
#else
        unsigned field_i : 16;
        unsigned field_j : 16;
#endif
    };

    struct Key {
        Label label;
        Int value;
        union {
            Int field;
            JacobianFieldID jac;
        };
        Int part;

        Key(const PetscFormKey & pkey) :
            label(pkey.label),
            value(pkey.value),
            field(pkey.field),
            part(pkey.part)
        {
        }

        Key(const Region & region, Int field) :
            label(region.label),
            value(region.value),
            field(field),
            part(region.part)
        {
        }

        Key(const Region & region, Int field_i, Int field_j) :
            label(region.label),
            value(region.value),
            part(region.part)
        {
            this->jac.field_i = field_i;
            this->jac.field_j = field_j;
        }

        Key(DMLabel label, Int value, Int field, Int part) :
            label(label),
            value(value),
            field(field),
            part(part)
        {
        }

        Key(DMLabel label, Int value, Int field_i, Int field_j, Int part) :
            label(label),
            value(value),
            part(part)
        {
            this->jac.field_i = field_i;
            this->jac.field_j = field_j;
        }

        operator PetscFormKey() const
        {
            PetscFormKey key;
            key.label = this->label;
            key.value = this->value;
            key.field = this->field;
            key.part = this->part;
            return key;
        }

        bool
        operator<(const Key & other) const
        {
            if ((DMLabel) this->label == (DMLabel) other.label) {
                if (this->value == other.value) {
                    if (this->field == other.field)
                        return this->part < other.part;
                    else
                        return this->field < other.field;
                }
                else
                    return this->value < other.value;
            }
            else
                return (DMLabel) this->label < (DMLabel) other.label;
        }
    };

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

    /// Get mesh regions where residual parts are defined
    ///
    /// @return Regions where residual parts are defined
    [[nodiscard]] std::vector<Region> get_residual_regions() const;

    /// Get mesh regions where Jacobian parts are defined
    ///
    /// @return Regions where Jacobian parts are defined
    [[nodiscard]] std::vector<Region> get_jacobian_regions() const;

    /// Get residual forms
    ///
    /// @param kind The kind of the residual weak form
    /// @param label Label associated with the mesh region
    /// @param val Value associated with the mesh region
    /// @param f Field ID
    /// @param part Weak form part
    [[nodiscard]] const std::vector<ResidualFunc *> &
    get(ResidualKind kind, const Label & label, Int val, Int f, Int part) const;

    /// Get Jacobian forms
    ///
    /// @param kind The kind of the Jacobian weak form
    /// @param label Label associated with the mesh region
    /// @param val Value associated with the mesh region
    /// @param f Field ID for test function
    /// @param g Field ID for base function
    /// @param part Weak form part
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

private:
    /// All residual forms
    std::array<std::map<Key, std::vector<ResidualFunc *>>, PETSC_NUM_WF> res_forms;

    /// Empty array for residual forms
    std::vector<ResidualFunc *> empty_res_forms;

    /// All Jacobian forms
    std::array<std::map<Key, std::vector<JacobianFunc *>>, PETSC_NUM_WF> jac_forms;

    /// Empty array for Jacobian forms
    std::vector<JacobianFunc *> empty_jac_forms;
};

} // namespace godzilla
