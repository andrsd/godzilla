#pragma once

#include "FENonlinearProblem.h"
#include "TransientProblemInterface.h"

namespace godzilla {

class ResidualFunc;

class ExplicitFELinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    explicit ExplicitFELinearProblem(const Parameters & params);
    ~ExplicitFELinearProblem() override;

    void create() override;
    void check() override;
    bool converged() override;
    void solve() override;

    virtual PetscErrorCode compute_rhs(Real time, const Vector & x, Vector & F);

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;
    void set_residual_block(Int field_id,
                            ResidualFunc * f0,
                            ResidualFunc * f1,
                            DMLabel label = nullptr,
                            Int val = 0) override;

    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
