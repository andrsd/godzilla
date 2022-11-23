#pragma once

#include "FENonlinearProblem.h"
#include "TransientProblemInterface.h"

namespace godzilla {

class ResidualFunc;

class ExplicitFELinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    explicit ExplicitFELinearProblem(const Parameters & params);
    virtual ~ExplicitFELinearProblem();

    void create() override;
    void check() override;
    bool converged() override;
    void solve() override;

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;
    void set_residual_block(PetscInt field_id, ResidualFunc * f0, ResidualFunc * f1) override;

    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
