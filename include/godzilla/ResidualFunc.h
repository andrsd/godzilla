// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/DenseVector.h"
#include "godzilla/DenseMatrix.h"
#include "godzilla/Functional.h"
#include "godzilla/Types.h"
#include "godzilla/WeakForm.h"

namespace godzilla {

class FEProblemInterface;

class AbstractResidualFunctional : public Functional {
public:
    AbstractResidualFunctional(FEProblemInterface * fepi, const std::string & region = "");

    /// Evaluate this functional
    ///
    /// @param val Array to store the values into
    virtual void evaluate2(Scalar val[]) const = 0;
};

/// Functional for evaluating residuals
template <WeakForm::ResidualKind>
class ResidualFunc : public AbstractResidualFunctional {
public:
};

/// Functional for evaluating F0 residuals
template <>
class ResidualFunc<WeakForm::F0> : public AbstractResidualFunctional {
public:
    ResidualFunc(FEProblemInterface * fepi, const std::string & region = "") :
        AbstractResidualFunctional(fepi, region)
    {
    }

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
class ResidualFunc<WeakForm::F1> : public AbstractResidualFunctional {
public:
    ResidualFunc(FEProblemInterface * fepi, const std::string & region = "") :
        AbstractResidualFunctional(fepi, region)
    {
    }

    void
    evaluate2(Scalar val[]) const override
    {
        auto f1 = evaluate();
    }

private:
    virtual DynDenseMatrix<Scalar> evaluate() const = 0;
};

} // namespace godzilla
