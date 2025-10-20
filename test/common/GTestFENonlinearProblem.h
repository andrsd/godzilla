#pragma once

#include "godzilla/FENonlinearProblem.h"

using namespace godzilla;

/// Test problem for simple FE solver
class GTestFENonlinearProblem : public FENonlinearProblem {
public:
    explicit GTestFENonlinearProblem(const Parameters & pars);

    PetscDS get_ds();
    void set_up_initial_guess() override;

    std::vector<BoundaryCondition *>
    get_boundary_conditions() const
    {
        return DiscreteProblemInterface::get_boundary_conditions();
    }

    std::vector<EssentialBC *>
    get_essential_bcs() const
    {
        return DiscreteProblemInterface::get_essential_bcs();
    }

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    /// ID for the "u" field
    const FieldID iu;
};
