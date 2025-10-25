#pragma once

#include "GTestFENonlinearProblem.h"
#include "godzilla/Types.h"

/// Test problem for simple FE solver with 2 fields
class GTest2FieldsFENonlinearProblem : public GTestFENonlinearProblem {
public:
    GTest2FieldsFENonlinearProblem(const godzilla::Parameters & pars);

protected:
    virtual void set_up_fields() override;

    /// ID for the "v" field
    const godzilla::FieldID iv;
};
