#pragma once

#include "godzilla/FENonlinearProblem.h"

/// Test problem for simple FE solver
class GTestFENonlinearProblem : public godzilla::FENonlinearProblem {
public:
    explicit GTestFENonlinearProblem(const godzilla::Parameters & pars);

    PetscDS get_ds();
    void set_up_initial_guess() override;

    std::vector<godzilla::Ref<godzilla::BoundaryCondition>>
    get_boundary_conditions() const
    {
        return DiscreteProblemInterface::get_boundary_conditions();
    }

    std::vector<godzilla::Ref<godzilla::EssentialBC>>
    get_essential_bcs() const
    {
        return DiscreteProblemInterface::get_essential_bcs();
    }

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    /// ID for the "u" field
    const godzilla::FieldID iu;
};
