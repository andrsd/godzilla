#pragma once

#include "FENonlinearProblem.h"
#include "TransientProblemInterface.h"

namespace godzilla {

class ImplicitFENonlinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    ImplicitFENonlinearProblem(const Parameters & params);
    virtual ~ImplicitFENonlinearProblem();

    void create() override;
    void check() override;
    bool converged() override;
    void solve() override;

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;

    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
