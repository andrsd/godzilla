#pragma once

#include "EssentialBC.h"
#include "FunctionInterface.h"

namespace godzilla {

/// Dirichlet boundary condition
///
/// Can be used only on single-field problems
class DirichletBC : public EssentialBC, public FunctionInterface {
public:
    DirichletBC(const InputParameters & params);

    virtual void create();
    virtual PetscInt get_field_id() const;
    virtual PetscInt get_num_components() const;
    virtual std::vector<PetscInt> get_components() const;
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]);

public:
    static InputParameters valid_params();
};

} // namespace godzilla
