#pragma once

#include "godzilla/GodzillaConfig.h"
#include "godzilla/Types.h"
#include "godzilla/Label.h"
#include <array>
#include <map>
#include <vector>
#include "petsc.h"

namespace godzilla {

class ResidualFunc;
class JacobianFunc;

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
    const std::vector<ResidualFunc *> &
    get(PetscWeakFormKind kind, const Label & label, Int val, Int f, Int part) const;

    /// Get Jacobian forms
    const std::vector<JacobianFunc *> &
    get(PetscWeakFormKind kind, const Label & label, Int val, Int f, Int g, Int part) const;

    /// Add a residual form
    ///
    /// @param kind Kind of the form
    /// @param label Label where the form is defined
    /// @param val Value
    /// @param f Field ID
    /// @param part Part
    /// @param func Functional representing a boundary or a volumetric residual form
    void
    add(PetscWeakFormKind kind, const Label & label, Int val, Int f, Int part, ResidualFunc * func);

    /// Add a Jacobian form
    ///
    /// @param kind Kind of the form
    /// @param label Label where the form is defined
    /// @param val Value
    /// @param f Field ID (test)
    /// @param g Field ID (base)
    /// @param part Part
    /// @param func Functional representing a boundary or a volumetric Jacobian form
    void add(PetscWeakFormKind kind,
             const Label & label,
             Int val,
             Int f,
             Int g,
             Int part,
             JacobianFunc * func);

    /// Query if Jacobian statement is set
    ///
    /// @return `true` if weak form for Jacobian statement is set, otherwise `false`
    bool has_jacobian() const;

    /// Query if Jacobian preconditioner statement is set
    ///
    /// @return `true` if weak form for Jacobian preconditioner statement is set, otherwise `false`
    bool has_jacobian_preconditioner() const;

    /// Get field ID for the combination of fields `f` and `g`
    ///
    /// @param f Field `f` ID
    /// @param g Field `g` ID
    /// @return Field ID used in `PetscFormKey`
    NO_DISCARD Int get_jac_key(Int f, Int g) const;

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
