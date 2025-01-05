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
    explicit ExplicitDGLinearProblem(const Parameters & params);
    ~ExplicitDGLinearProblem() override;

    void create() override;
    void run() override;
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

private:
    SNESolver create_sne_solver() override;
    void compute_residual(const Vector & x, Vector & f) final;
    void compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp) final;

public:
    static Parameters parameters();
};

} // namespace godzilla
