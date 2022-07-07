#pragma once

#include "EssentialBC.h"
#include "FunctionInterface.h"

namespace godzilla {

/// Dirichlet boundary condition
///
/// Can be used only on single-field problems
class DirichletBC : public EssentialBC, public FunctionInterface {
public:
    DirichletBC(const Parameters & params);

    virtual void create();
    virtual PetscInt get_num_components() const;
    virtual std::vector<PetscInt> get_components() const;
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]);
    virtual void
    evaluate_t(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]);

protected:
    virtual void set_up_callback();

public:
    static Parameters parameters();
};

} // namespace godzilla
