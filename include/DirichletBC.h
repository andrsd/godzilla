#pragma once

#include "EssentialBC.h"
#include "ParsedFunctionInterface.h"

namespace godzilla {

/// Dirichlet boundary condition
///
/// Can be used only on single-field problems
class DirichletBC : public EssentialBC, public ParsedFunctionInterface {
public:
    DirichletBC(const InputParameters & params);

    virtual void create() override;
    virtual Scalar evaluate(Real time, Real x, Real y, Real z) override;

public:
    static InputParameters validParams();
};

} // namespace godzilla
