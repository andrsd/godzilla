#include "WeakForm.h"
#include "Functional.h"
#include <set>

namespace godzilla {

struct Key {
    DMLabel label;
    PetscInt value;
    PetscInt part;
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
    std::set<Key> unique;
    std::array<PetscWeakFormKind, 4> jacmap = { PETSC_WF_G0,
                                                PETSC_WF_G1,
                                                PETSC_WF_G2,
                                                PETSC_WF_G3 };
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

const std::vector<Functional *> &
WeakForm::get(PetscWeakFormKind kind, DMLabel label, PetscInt val, PetscInt f, PetscInt part) const
{
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

const std::vector<Functional *> &
WeakForm::get(PetscWeakFormKind kind,
              DMLabel label,
              PetscInt val,
              PetscInt f,
              PetscInt g,
              PetscInt part) const
{
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
              DMLabel label,
              PetscInt value,
              PetscInt f,
              PetscInt part,
              Functional * func)
{
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
              DMLabel label,
              PetscInt val,
              PetscInt f,
              PetscInt g,
              PetscInt part,
              Functional * func)
{
    if (func != nullptr) {
        PetscFormKey key;
        key.label = label;
        key.value = val;
        key.field = get_jac_key(f, g);
        key.part = part;
        this->jac_forms[kind][key].push_back(func);
    }
}

PetscInt
WeakForm::get_jac_key(PetscInt f, PetscInt g) const
{
    return f * this->n_fields + g;
}

} // namespace godzilla
