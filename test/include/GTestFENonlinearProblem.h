#pragma once

#include "godzilla/FENonlinearProblem.h"

using namespace godzilla;

/// Test problem for simple FE solver
class GTestFENonlinearProblem : public FENonlinearProblem {
public:
    explicit GTestFENonlinearProblem(const Parameters & params);

    PetscDS get_ds();
    void compute_postprocessors() override;
    void set_up_initial_guess() override;
    void solve() override;
    void
    add_boundary_natural_riemann(const std::string & name,
                                 const std::string & boundary,
                                 Int field,
                                 const std::vector<Int> & components,
                                 PetscNaturalRiemannBCFunc * fn,
                                 PetscNaturalRiemannBCFunc * fn_t,
                                 void * context) override
    {
        FENonlinearProblem::add_boundary_natural_riemann(name,
                                                         boundary,
                                                         field,
                                                         components,
                                                         fn,
                                                         fn_t,
                                                         context);
    }

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    /// ID for the "u" field
    const Int iu;
};
