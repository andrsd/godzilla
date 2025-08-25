// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/WeakForm.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/CallStack.h"
#include <set>

namespace godzilla {

WeakForm::WeakForm() {}

WeakForm::~WeakForm()
{
    CALL_STACK_MSG();
    for (auto & m : this->res_forms)
        for (auto & [k, forms] : m)
            for (auto & f : forms)
                delete f;
    for (auto & f : this->empty_res_forms)
        delete f;

    for (auto & m : this->jac_forms)
        for (auto & [k, forms] : m)
            for (auto & f : forms)
                delete f;
    for (auto & f : this->empty_jac_forms)
        delete f;
}

std::vector<WeakForm::Region>
WeakForm::get_residual_regions() const
{
    CALL_STACK_MSG();
    std::set<Region> unique;
    std::array<ResidualKind, 2> res_kind = { F0, F1 };
    for (const auto & r : res_kind) {
        const auto & forms = this->res_forms[r];
        for (const auto & it : forms) {
            const auto & form_key = it.first;
            Region k(form_key.label, form_key.value, form_key.part);
            unique.emplace(k);
        }
    }

    std::vector<Region> v;
    v.assign(unique.begin(), unique.end());
    return v;
}

std::vector<WeakForm::Region>
WeakForm::get_jacobian_regions() const
{
    CALL_STACK_MSG();
    std::set<Region> unique;
    std::array<JacobianKind, 8> jacmap = { G0, G1, G2, G3, GP0, GP1, GP2, GP3 };
    for (const auto & r : jacmap) {
        const auto & forms = this->jac_forms[r];
        for (const auto & it : forms) {
            const auto & form_key = it.first;
            Region k(form_key.label, form_key.value, form_key.part);
            unique.emplace(k);
        }
    }

    std::vector<Region> v;
    v.assign(unique.begin(), unique.end());
    return v;
}

const std::vector<ResidualFunc *> &
WeakForm::get(ResidualKind kind, const Label & label, Int val, FieldID f, Int part) const
{
    CALL_STACK_MSG();
    Key key(label, val, f.value(), part);
    const auto & it = this->res_forms[kind].find(key);
    if (it != this->res_forms[kind].end())
        return it->second;
    else
        return this->empty_res_forms;
}

const std::vector<JacobianFunc *> &
WeakForm::get(JacobianKind kind, const Label & label, Int val, FieldID f, FieldID g, Int part) const
{
    CALL_STACK_MSG();
    Key key(label, val, f.value(), g.value(), part);
    const auto & it = this->jac_forms[kind].find(key);
    if (it != this->jac_forms[kind].end())
        return it->second;
    else
        return this->empty_jac_forms;
}

void
WeakForm::add(ResidualKind kind,
              const Label & label,
              Int value,
              FieldID f,
              Int part,
              ResidualFunc * func)
{
    CALL_STACK_MSG();
    if (func != nullptr) {
        Key key(label, value, f.value(), part);
        this->res_forms[kind][key].push_back(func);
    }
}

void
WeakForm::add(JacobianKind kind,
              const Label & label,
              Int val,
              FieldID f,
              FieldID g,
              Int part,
              JacobianFunc * func)
{
    CALL_STACK_MSG();
    if (func != nullptr) {
        Key key(label, val, f.value(), g.value(), part);
        this->jac_forms[kind][key].push_back(func);
    }
}

bool
WeakForm::has_jacobian() const
{
    CALL_STACK_MSG();
    auto n0 = this->jac_forms[WeakForm::G0].size();
    auto n1 = this->jac_forms[WeakForm::G1].size();
    auto n2 = this->jac_forms[WeakForm::G2].size();
    auto n3 = this->jac_forms[WeakForm::G3].size();
    return (n0 + n1 + n2 + n3) > 0;
}

bool
WeakForm::has_jacobian_preconditioner() const
{
    CALL_STACK_MSG();
    auto n0 = this->jac_forms[WeakForm::GP0].size();
    auto n1 = this->jac_forms[WeakForm::GP1].size();
    auto n2 = this->jac_forms[WeakForm::GP2].size();
    auto n3 = this->jac_forms[WeakForm::GP3].size();
    return (n0 + n1 + n2 + n3) > 0;
}

} // namespace godzilla
