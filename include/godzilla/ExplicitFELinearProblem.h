// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FENonlinearProblem.h"
#include "godzilla/ExplicitProblemInterface.h"
#include "godzilla/String.h"

namespace godzilla {

class ResidualFunc;

class ExplicitFELinearProblem : public FENonlinearProblem, public ExplicitProblemInterface {
public:
    explicit ExplicitFELinearProblem(const Parameters & pars);

    Real get_time() const override;
    Int get_step_num() const override;
    void create() override;
    void run() override;
    void solve();
    bool converged();
    void compute_solution_vector_local() override;

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_monitors() override;
    void add_residual_block(FieldID field_id,
                            ResidualFunc * f0,
                            ResidualFunc * f1,
                            const String & region = "") override;
    void post_step() override;

private:
    SNESolver create_sne_solver() override;
    void compute_rhs_local(Real time, const Vector & x, Vector & F) override;
    void compute_rhs_function_fem(Real time, const Vector & loc_x, Vector & loc_g);

public:
    static Parameters parameters();
};

} // namespace godzilla
