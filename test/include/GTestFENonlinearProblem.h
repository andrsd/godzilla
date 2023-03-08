#pragma once

#include "FENonlinearProblem.h"

using namespace godzilla;

/// Test problem for simple FE solver
class GTestFENonlinearProblem : public FENonlinearProblem {
public:
    explicit GTestFENonlinearProblem(const Parameters & params);

    PetscDS getDS();
    void compute_postprocessors() override;
    void set_up_initial_guess() override;
    void solve() override;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    /// ID for the "u" field
    const Int iu;
};
