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
    const std::vector<PetscInt> & get_components() const override;
    PetscFunc * get_function_t() override;
    void evaluate(PetscInt dim,
                  PetscReal time,
                  const PetscReal x[],
                  PetscInt nc,
                  PetscScalar u[]) override;
    void evaluate_t(PetscInt dim,
                    PetscReal time,
                    const PetscReal x[],
                    PetscInt nc,
                    PetscScalar u[]) override;

protected:
    std::vector<PetscInt> components;

public:
    static Parameters parameters();
};

} // namespace godzilla
