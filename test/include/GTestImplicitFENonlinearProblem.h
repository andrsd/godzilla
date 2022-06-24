#pragma once

#include "ImplicitFENonlinearProblem.h"

using namespace godzilla;

/// Test problem for simple FE solver
class GTestImplicitFENonlinearProblem : public ImplicitFENonlinearProblem {
public:
    GTestImplicitFENonlinearProblem(const InputParameters & params);
    virtual ~GTestImplicitFENonlinearProblem();

protected:
    virtual void set_up_fields() override;
    virtual void set_up_weak_form() override;

    /// ID for the "u" field
    const PetscInt iu;
};
