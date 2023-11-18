#pragma once

#include "godzilla/EssentialBC.h"
#include "godzilla/FunctionInterface.h"

namespace godzilla {

/// Dirichlet boundary condition
///
/// Can be used only on single-field problems
class DirichletBC : public EssentialBC, protected FunctionInterface {
public:
    explicit DirichletBC(const Parameters & params);

    void create() override;
    const std::vector<Int> & get_components() const override;
    PetscFunc * get_function_t() override;
    void evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override;
    void evaluate_t(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override;

private:
    std::vector<Int> components;

public:
    static Parameters parameters();
};

} // namespace godzilla
