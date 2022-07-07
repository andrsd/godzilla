#pragma once

#include "FENonlinearProblem.h"
#include "TransientProblemInterface.h"

namespace godzilla {

class ImplicitFENonlinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    ImplicitFENonlinearProblem(const Parameters & params);
    virtual ~ImplicitFENonlinearProblem();

    virtual void create() override;
    virtual void check() override;
    virtual bool converged() override;
    virtual void solve() override;

protected:
    virtual void init() override;
    virtual void set_up_callbacks() override;
    virtual void set_up_time_scheme() override;
    virtual void set_up_monitors() override;

    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
