// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/ResidualFunc.h"
#include "godzilla/FieldValue.h"
#include <string>

namespace godzilla {

class NaturalBC;
class FEProblemInterface;

class AbstractBndResidualFunc : public AbstractResidualFunctional {
public:
    explicit AbstractBndResidualFunc(const BoundaryCondition * bc);

protected:
    /// Get normal
    ///
    /// @return Outward normal
    [[nodiscard]] const Normal & get_normal() const;
};

template <WeakForm::ResidualKind>
class BndResidualFunc : public AbstractBndResidualFunc {
public:
    explicit BndResidualFunc(const BoundaryCondition * bc);
};

template <>
class BndResidualFunc<WeakForm::F0> : public AbstractBndResidualFunc {
public:
    BndResidualFunc(const BoundaryCondition * bc) : AbstractBndResidualFunc(bc) {}

    void
    evaluate2(Scalar val[]) const override
    {
        auto f0 = evaluate();
    }

private:
    virtual DynDenseVector<Scalar> evaluate() const = 0;
};

/// Functional for evaluating F1 residuals
template <>
class BndResidualFunc<WeakForm::F1> : public AbstractBndResidualFunc {
public:
    BndResidualFunc(const BoundaryCondition * bc) : AbstractBndResidualFunc(bc) {}

    void
    evaluate2(Scalar val[]) const override
    {
        auto f1 = evaluate();
    }

private:
    virtual DynDenseMatrix<Scalar> evaluate() const = 0;
};

} // namespace godzilla
