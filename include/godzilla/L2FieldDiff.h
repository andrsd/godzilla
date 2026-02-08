// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Postprocessor.h"

namespace godzilla {

class FEProblemInterface;

/// Computes the L_2 difference between a function `u` and an FEM interpolant solution `u_h`,
/// separated into field components.
///
class L2FieldDiff : public Postprocessor {
public:
    explicit L2FieldDiff(const Parameters & pars);

    void create() override;
    void compute() override;
    std::vector<Real> get_value() override;

protected:
    template <class T>
    void
    set_function(FieldID fid, Ref<T> instance, void (T::*method)(Real time, const Real[], Scalar[]))
    {
        this->delegates[fid.value()].bind(instance, method);
    }

private:
    virtual void set_up_callbacks() = 0;

    /// FE problem
    Optional<Ref<const FEProblemInterface>> fepi;
    /// Number of fields
    Int n_fields;
    /// Computed L_2 errors
    std::vector<Real> l2_diff;
    /// Delegates: [field id] -> function
    std::map<Int, FunctionDelegate> delegates;

public:
    static Parameters parameters();
};

} // namespace godzilla
