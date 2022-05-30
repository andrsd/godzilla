#pragma once

#include "FENonlinearProblem.h"
#include "TransientProblemInterface.h"

namespace godzilla {

class ImplicitFENonlinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    ImplicitFENonlinearProblem(const InputParameters & params);
    virtual ~ImplicitFENonlinearProblem();

    virtual void create() override;
    virtual bool converged() override;
    virtual void solve() override;

protected:
    virtual void init() override;
    virtual void set_up_callbacks() override;
    /// Setup monitors
    virtual void set_up_monitors() override;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
