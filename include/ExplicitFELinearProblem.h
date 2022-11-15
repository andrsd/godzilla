#pragma once

#include "FENonlinearProblem.h"
#include "TransientProblemInterface.h"

namespace godzilla {

class ResidualFunc;

class ExplicitFELinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    ExplicitFELinearProblem(const Parameters & params);
    virtual ~ExplicitFELinearProblem();

    virtual void create() override;
    virtual void check() override;
    virtual bool converged() override;
    virtual void solve() override;

protected:
    virtual void init() override;
    virtual void set_up_callbacks() override;
    virtual void set_up_time_scheme() override;
    virtual void set_up_monitors() override;
    virtual void
    set_residual_block(PetscInt field_id, ResidualFunc * f0, ResidualFunc * f1) override;

    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
