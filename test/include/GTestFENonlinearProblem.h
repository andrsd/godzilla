#pragma once

#include "FENonlinearProblem.h"

using namespace godzilla;

/// Test problem for simple FE solver
class GTestFENonlinearProblem : public FENonlinearProblem {
public:
    GTestFENonlinearProblem(const InputParameters & params);
    virtual ~GTestFENonlinearProblem();

    virtual PetscErrorCode compute_residual_callback(Vec x, Vec f) override;
    virtual PetscErrorCode compute_jacobian_callback(Vec x, Mat J, Mat Jp) override;
    const std::vector<PetscReal> & getConstants();
    virtual void set_up_constants();
    PetscDS getDS();
    void compute_postprocessors() override;
    void set_up_initial_guess() override;

protected:
    virtual void set_up_fields() override;
    virtual void set_up_weak_form() override;

    /// ID for the "u" field
    const PetscInt iu;
};
