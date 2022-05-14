#pragma once

#include "NonlinearProblem.h"
#include "FEProblemInterface.h"

namespace godzilla {

/// PETSc non-linear problem that arises from a finite element discretization
/// using the PetscFE system
///
class FENonlinearProblem : public NonlinearProblem, public FEProblemInterface {
public:
    FENonlinearProblem(const InputParameters & parameters);
    virtual ~FENonlinearProblem();

    virtual void create() override;

protected:
    virtual void init() override;
    virtual void set_up_callbacks() override;
    virtual void set_up_initial_guess() override;
    virtual PetscErrorCode compute_residual_callback(Vec x, Vec f) override;
    virtual PetscErrorCode compute_jacobian_callback(Vec x, Mat J, Mat Jp) override;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
