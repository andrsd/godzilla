// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/WeakForm.h"
#include "godzilla/CallStack.h"
#include <set>

namespace godzilla {

struct Key {
    DMLabel label;
    Int value;
    Int part;
};

} // namespace godzilla

namespace std {

template <>
struct less<godzilla::Key> {
    bool
    operator()(const godzilla::Key & lhs, const godzilla::Key & rhs) const
    {
        if (lhs.label == rhs.label) {
            if (lhs.value == rhs.value)
                return lhs.part < rhs.part;
            else
                return lhs.value < rhs.value;
        }
        else
            return lhs.label < rhs.label;
    }
};

} // namespace std

namespace godzilla {

WeakForm::WeakForm() :
    // FIXME: this should be either called something else or set to a correct value
    n_fields(100)
{
}

std::vector<PetscFormKey>
WeakForm::get_residual_keys() const
{
    _F_;
    std::set<Key> unique;
    std::array<PetscWeakFormKind, 2> resmap = { PETSC_WF_F0, PETSC_WF_F1 };
    for (const auto & r : resmap) {
        const auto & forms = res_forms[r];
        for (const auto & it : forms) {
            const auto & form_key = it.first;
            Key k = { form_key.label, form_key.value, form_key.part };
            unique.insert(k);
        }
    }

    std::vector<PetscFormKey> v;
    for (auto & k : unique) {
        PetscFormKey fk = { k.label, k.value, 0, k.part };
        v.push_back(fk);
    }

    return v;
}

std::vector<PetscFormKey>
WeakForm::get_jacobian_keys() const
{
    _F_;
    std::set<Key> unique;
    std::array<PetscWeakFormKind, 8> jacmap = { PETSC_WF_G0,  PETSC_WF_G1,  PETSC_WF_G2,
                                                PETSC_WF_G3,  PETSC_WF_GP0, PETSC_WF_GP1,
                                                PETSC_WF_GP2, PETSC_WF_GP3 };
    for (const auto & r : jacmap) {
        const auto & forms = this->jac_forms[r];
        for (const auto & it : forms) {
            const auto & form_key = it.first;
            Key k = { form_key.label, form_key.value, form_key.part };
            unique.insert(k);
        }
    }

    std::vector<PetscFormKey> v;
    for (auto & k : unique) {
        PetscFormKey fk = { k.label, k.value, 0, k.part };
        v.push_back(fk);
    }

    return v;
}

const std::vector<ResidualFunc *> &
WeakForm::get(PetscWeakFormKind kind, const Label & label, Int val, Int f, Int part) const
{
    _F_;
    PetscFormKey key;
    key.label = label;
    key.value = val;
    key.field = f;
    key.part = part;
    const auto & it = this->res_forms[kind].find(key);
    if (it != this->res_forms[kind].end())
        return it->second;
    else
        return this->empty_res_forms;
}

const std::vector<JacobianFunc *> &
WeakForm::get(PetscWeakFormKind kind, const Label & label, Int val, Int f, Int g, Int part) const
{
    _F_;
    PetscFormKey key;
    key.label = label;
    key.value = val;
    key.field = get_jac_key(f, g);
    key.part = part;
    const auto & it = this->jac_forms[kind].find(key);
    if (it != this->jac_forms[kind].end())
        return it->second;
    else
        return this->empty_jac_forms;
}

void
WeakForm::add(PetscWeakFormKind kind,
              const Label & label,
              Int value,
              Int f,
              Int part,
              ResidualFunc * func)
{
    _F_;
    if (func != nullptr) {
        PetscFormKey key;
        key.label = label;
        key.value = value;
        key.field = f;
        key.part = part;
        this->res_forms[kind][key].push_back(func);
    }
}

void
WeakForm::add(PetscWeakFormKind kind,
              const Label & label,
              Int val,
              Int f,
              Int g,
              Int part,
              JacobianFunc * func)
{
    _F_;
    if (func != nullptr) {
        PetscFormKey key;
        key.label = label;
        key.value = val;
        key.field = get_jac_key(f, g);
        key.part = part;
        this->jac_forms[kind][key].push_back(func);
    }
}

Int
WeakForm::get_jac_key(Int f, Int g) const
{
    _F_;
    return f * this->n_fields + g;
}

bool
WeakForm::has_jacobian() const
{
    _F_;
    auto n0 = this->jac_forms[PETSC_WF_G0].size();
    auto n1 = this->jac_forms[PETSC_WF_G1].size();
    auto n2 = this->jac_forms[PETSC_WF_G2].size();
    auto n3 = this->jac_forms[PETSC_WF_G3].size();
    return (n0 + n1 + n2 + n3) > 0;
}

bool
WeakForm::has_jacobian_preconditioner() const
{
    _F_;
    auto n0 = this->jac_forms[PETSC_WF_GP0].size();
    auto n1 = this->jac_forms[PETSC_WF_GP1].size();
    auto n2 = this->jac_forms[PETSC_WF_GP2].size();
    auto n3 = this->jac_forms[PETSC_WF_GP3].size();
    return (n0 + n1 + n2 + n3) > 0;
}

} // namespace godzilla
