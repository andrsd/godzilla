#pragma once

#include "FENonlinearProblem.h"
#include "TransientInterface.h"

namespace godzilla {

class ImplicitFENonlinearProblem : public FENonlinearProblem, public TransientInterface {
public:
    ImplicitFENonlinearProblem(const InputParameters & params);
    virtual ~ImplicitFENonlinearProblem();

    virtual void create() override;
    virtual void solve() override;

protected:
    virtual void init() override;
    virtual void setupCallbacks() override;

public:
    static InputParameters validParams();
};

} // namespace godzilla
