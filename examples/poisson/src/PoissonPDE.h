#pragma once

#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/Types.h"

using namespace godzilla;

class Residual0 : public ResidualFunc {
public:
    Residual0(Ref<FEProblemInterface> fepi) : ResidualFunc(fepi), ffn(get_field_value("forcing_fn"))
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        CALL_STACK_MSG();
        f[0] = -this->ffn(0);
    }

protected:
    const FieldValue & ffn;
};

class Residual1 : public ResidualFunc {
public:
    Residual1(Ref<FEProblemInterface> fepi) :
        ResidualFunc(fepi),
        dim(get_spatial_dimension()),
        u_x(get_field_gradient("u"))
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        CALL_STACK_MSG();
        for (Int d = 0; d < this->dim; ++d)
            f[d] = this->u_x(d);
    }

protected:
    const Dimension & dim;
    const FieldGradient & u_x;
};

class Jacobian3 : public JacobianFunc {
public:
    Jacobian3(Ref<FEProblemInterface> fepi) : JacobianFunc(fepi), dim(get_spatial_dimension()) {}

    void
    evaluate(Scalar g[]) const override
    {
        CALL_STACK_MSG();
        for (Int d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = 1.0;
    }

protected:
    const Dimension & dim;
};
