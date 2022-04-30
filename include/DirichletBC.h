#pragma once

#include "EssentialBC.h"

namespace godzilla {

/// Dirichlet boundary condition
///
/// Can be used only on single-field problems
class DirichletBC : public EssentialBC {
public:
    DirichletBC(const InputParameters & params);

    virtual void create() override;
    virtual Scalar evaluate(Real x, Real y, Real z) const override;

public:
    static InputParameters validParams();
};

} // namespace godzilla
