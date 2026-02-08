// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/NonlinearProblem.h"
#include "godzilla/DGProblemInterface.h"
#include "godzilla/ExplicitProblemInterface.h"

namespace godzilla {

class ExplicitDGLinearProblem :
    public NonlinearProblem,
    public DGProblemInterface,
    public ExplicitProblemInterface {
public:
    explicit ExplicitDGLinearProblem(const Parameters & pars);

    void create() override;
    void run() override;
    void solve();
    bool converged();
    Real get_time() const override;
    Int get_step_num() const override;
    void compute_solution_vector_local() override;

protected:
    void init() override;
    void allocate_objects() override;
    void set_up_callbacks() override;
    void set_up_initial_guess() override;
    void set_up_monitors() override;
    void post_step() override;

    ExecuteOnFlags
    default_execute_on(PostprocessorTag) const override
    {
        return ExecuteOn::INITIAL | ExecuteOn::TIMESTEP;
    }

private:
    SNESolver create_sne_solver() override;

public:
    static Parameters parameters();
};

} // namespace godzilla
