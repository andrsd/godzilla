#pragma once

#include "godzilla/ExplicitDGLinearProblem.h"

/// Test problem for DG solver
class GTestDGLinearProblem : public godzilla::ExplicitDGLinearProblem {
public:
    explicit GTestDGLinearProblem(const godzilla::Parameters & pars);

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;
    void set_up_time_scheme() override;

    /// ID for the "u" field
    godzilla::FieldID iu;
};
