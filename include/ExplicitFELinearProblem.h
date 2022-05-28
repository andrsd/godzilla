#pragma once

#include "FENonlinearProblem.h"
#include "TransientProblemInterface.h"

namespace godzilla {

class ExplicitFELinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    ExplicitFELinearProblem(const InputParameters & params);
    virtual ~ExplicitFELinearProblem();

    virtual void create() override;
    virtual void solve() override;

protected:
    virtual void init() override;
    virtual void set_up_callbacks() override;
    virtual void set_up_time_scheme() override;
    /// Setup monitors
    virtual void set_up_monitors() override;
    virtual void set_residual_block(PetscInt field_id,
                                    PetscFEResidualFunc * f0,
                                    PetscFEResidualFunc * f1) override;
    virtual void output_initial() override;
    virtual void output_final() override;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
