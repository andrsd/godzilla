#pragma once

#include "GodzillaConfig.h"
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

class ResidualFunc;
class JacobianFunc;
class BndResidualFunc;
class BndJacobianFunc;

/// Weak formulation
///
class WeakForm {
public:
    WeakForm();

    /// Get residual forms
    const std::vector<ResidualFunc *> &
    get(PetscWeakFormKind kind, DMLabel label, PetscInt val, PetscInt f, PetscInt part) const;

    const std::vector<BndResidualFunc *> &
    get_bnd(PetscWeakFormKind kind, DMLabel label, PetscInt val, PetscInt f, PetscInt part) const;

    /// Get Jacobian forms
    const std::vector<JacobianFunc *> & get(PetscWeakFormKind kind,
                                            DMLabel label,
                                            PetscInt val,
                                            PetscInt f,
                                            PetscInt g,
                                            PetscInt part) const;

    const std::vector<BndJacobianFunc *> & get_bnd(PetscWeakFormKind kind,
                                                   DMLabel label,
                                                   PetscInt val,
                                                   PetscInt f,
                                                   PetscInt g,
                                                   PetscInt part) const;

    /// Add a residual form
    void add(PetscWeakFormKind kind,
             DMLabel label,
             PetscInt val,
             PetscInt f,
             PetscInt part,
             ResidualFunc * func);

    /// Add a boundary residual form
    void add(PetscWeakFormKind kind,
             DMLabel label,
             PetscInt val,
             PetscInt f,
             PetscInt part,
             BndResidualFunc * func);

    /// Add a Jacobian form
    void add(PetscWeakFormKind kind,
             DMLabel label,
             PetscInt val,
             PetscInt f,
             PetscInt g,
             PetscInt part,
             JacobianFunc * func);

    /// Add a boundary Jacobian form
    void add(PetscWeakFormKind kind,
             DMLabel label,
             PetscInt val,
             PetscInt f,
             PetscInt g,
             PetscInt part,
             BndJacobianFunc * func);

protected:
    NO_DISCARD PetscInt get_jac_key(PetscInt f, PetscInt g) const;

    /// Number of fields
    PetscInt n_fields;

    /// All residual forms
    std::array<std::map<PetscFormKey, std::vector<ResidualFunc *>>, PETSC_NUM_WF> res_forms;

    /// Empty array for residual forms
    std::vector<ResidualFunc *> empty_res_forms;

    /// All boundary residual forms
    std::array<std::map<PetscFormKey, std::vector<BndResidualFunc *>>, PETSC_NUM_WF> bnd_res_forms;

    /// Empty array for boundary residual forms
    std::vector<BndResidualFunc *> empty_bnd_res_forms;

    /// All Jacobian forms
    std::array<std::map<PetscFormKey, std::vector<JacobianFunc *>>, PETSC_NUM_WF> jac_forms;

    /// Empty array for Jacobian forms
    std::vector<JacobianFunc *> empty_jac_forms;

    /// All boundary Jacobian forms
    std::array<std::map<PetscFormKey, std::vector<BndJacobianFunc *>>, PETSC_NUM_WF> bnd_jac_forms;

    /// Empty array for boundary Jacobian forms
    std::vector<BndJacobianFunc *> empty_bnd_jac_forms;
};

} // namespace godzilla
