#pragma once

#include "ImplicitFENonlinearProblem.h"

using namespace godzilla;

/// Test problem for simple FE solver
class GTestImplicitFENonlinearProblem : public ImplicitFENonlinearProblem {
public:
    explicit GTestImplicitFENonlinearProblem(const Parameters & params);
    void set_up_initial_guess() override;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    /// ID for the "u" field
    const Int iu;
};
