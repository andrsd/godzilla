#include "WeakForm.h"
#include "Error.h"

namespace godzilla {

WeakForm::WeakForm() :
    // FIXME: this should be either called something else or set to a correct value
    n_fields(100)
{
}

const std::vector<ResidualFunc *> &
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

const std::vector<BndResidualFunc *> &
WeakForm::get_bnd(PetscWeakFormKind kind, DMLabel label, PetscInt val, PetscInt f, PetscInt part)
    const
{
    PetscFormKey key;
    key.label = label;
    key.value = val;
    key.field = f;
    key.part = part;
    const auto & it = this->bnd_res_forms[kind].find(key);
    if (it != this->bnd_res_forms[kind].end())
        return it->second;
    else
        return this->empty_bnd_res_forms;
}

const std::vector<JacobianFunc *> &
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

const std::vector<BndJacobianFunc *> &
WeakForm::get_bnd(PetscWeakFormKind kind,
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
    const auto & it = this->bnd_jac_forms[kind].find(key);
    if (it != this->bnd_jac_forms[kind].end())
        return it->second;
    else
        return this->empty_bnd_jac_forms;
}

void
WeakForm::add(PetscWeakFormKind kind,
              DMLabel label,
              PetscInt value,
              PetscInt f,
              PetscInt part,
              ResidualFunc * func)
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
              PetscInt part,
              BndResidualFunc * func)
{
    if (func != nullptr) {
        PetscFormKey key;
        key.label = label;
        key.value = val;
        key.field = f;
        key.part = part;
        this->bnd_res_forms[kind][key].push_back(func);
    }
}

void
WeakForm::add(PetscWeakFormKind kind,
              DMLabel label,
              PetscInt val,
              PetscInt f,
              PetscInt g,
              PetscInt part,
              JacobianFunc * func)
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

void
WeakForm::add(PetscWeakFormKind kind,
              DMLabel label,
              PetscInt val,
              PetscInt f,
              PetscInt g,
              PetscInt part,
              BndJacobianFunc * func)
{
    if (func != nullptr) {
        PetscFormKey key;
        key.label = label;
        key.value = val;
        key.field = get_jac_key(f, g);
        key.part = part;
        this->bnd_jac_forms[kind][key].push_back(func);
    }
}

PetscInt
WeakForm::get_jac_key(PetscInt f, PetscInt g) const
{
    return f * this->n_fields + g;
}

} // namespace godzilla
