#pragma once

#include "GTestFENonlinearProblem.h"

using namespace godzilla;

/// Test problem for simple FE solver with 2 fields
class GTest2FieldsFENonlinearProblem : public GTestFENonlinearProblem {
public:
    GTest2FieldsFENonlinearProblem(const InputParameters & params);

protected:
    virtual void set_up_fields() override;

    /// ID for the "v" field
    const PetscInt iv;
};
