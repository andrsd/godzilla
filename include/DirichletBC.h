#pragma once

#include "EssentialBC.h"
#include "FunctionInterface.h"

namespace godzilla {

/// Dirichlet boundary condition
///
/// Can be used only on single-field problems
class DirichletBC : public EssentialBC, public FunctionInterface {
public:
    explicit DirichletBC(const Parameters & params);

    void create() override;
    const std::vector<Int> & get_components() const override;
    PetscFunc * get_function_t() override;
    void evaluate(Int dim, PetscReal time, const PetscReal x[], Int nc, PetscScalar u[]) override;
    void evaluate_t(Int dim, PetscReal time, const PetscReal x[], Int nc, PetscScalar u[]) override;

protected:
    std::vector<Int> components;

public:
    static Parameters parameters();
};

} // namespace godzilla
