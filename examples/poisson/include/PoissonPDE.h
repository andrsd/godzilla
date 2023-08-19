#pragma once

#include "ResidualFunc.h"
#include "JacobianFunc.h"
#include "petsc.h"

using namespace godzilla;

class Residual0 : public ResidualFunc {
public:
    Residual0(FEProblemInterface * fepi) : ResidualFunc(fepi), ffn(get_field_value("forcing_fn")) {}

    void
    evaluate(PetscScalar f[]) const override
    {
        _F_;
        f[0] = -this->ffn(0);
    }

protected:
    const FieldValue & ffn;
};

class Residual1 : public ResidualFunc {
public:
    Residual1(FEProblemInterface * fepi) :
        ResidualFunc(fepi),
        dim(get_spatial_dimension()),
        u_x(get_field_gradient("u"))
    {
    }

    void
    evaluate(PetscScalar f[]) const override
    {
        _F_;
        for (PetscInt d = 0; d < this->dim; ++d)
            f[d] = this->u_x(d);
    }

protected:
    const PetscInt & dim;
    const FieldGradient & u_x;
};

class Jacobian3 : public JacobianFunc {
public:
    Jacobian3(FEProblemInterface * fepi) : JacobianFunc(fepi), dim(get_spatial_dimension()) {}

    void
    evaluate(PetscScalar g[]) const override
    {
        _F_;
        for (PetscInt d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = 1.0;
    }

protected:
    const PetscInt & dim;
};
