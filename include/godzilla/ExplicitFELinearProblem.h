#pragma once

#include "godzilla/FENonlinearProblem.h"
#include "godzilla/ExplicitProblemInterface.h"

namespace godzilla {

class ResidualFunc;

class ExplicitFELinearProblem : public FENonlinearProblem, public ExplicitProblemInterface {
public:
    explicit ExplicitFELinearProblem(const Parameters & params);
    ~ExplicitFELinearProblem() override;

    Real get_time() const override;
    Int get_step_num() const override;
    void create() override;
    void check() override;
    bool converged() override;
    void solve() override;

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;
    void add_residual_block(Int field_id,
                            ResidualFunc * f0,
                            ResidualFunc * f1,
                            const std::string & region = "") override;
    void build_local_solution_vector(Vector & loc_sln) override;
    PetscErrorCode compute_rhs_local(Real time, const Vector & x, Vector & F) override;
    void post_step() override;

public:
    static Parameters parameters();
};

} // namespace godzilla
