// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FENonlinearProblem.h"
#include "godzilla/ExplicitProblemInterface.h"

namespace godzilla {

class AbstractResidualFunctional;

class ExplicitFELinearProblem : public FENonlinearProblem, public ExplicitProblemInterface {
public:
    explicit ExplicitFELinearProblem(const Parameters & params);
    ~ExplicitFELinearProblem() override;

    [[nodiscard]] Real get_time() const override;
    [[nodiscard]] Int get_step_num() const override;
    void create() override;
    void run() override;
    bool converged();
    void compute_solution_vector_local() override;

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_monitors() override;
    void post_step() override;

    template <WeakForm::ResidualKind KIND>
    void
    add_residual_block(Int fid,
                       AbstractResidualFunctional * res_fn,
                       const std::string & region = "")
    {
        throw Exception("Unsupported residual functional kind");
    }

private:
    void compute_rhs_local(Real time, const Vector & x, Vector & F) override;

    void add_weak_form_residual_block(WeakForm::ResidualKind kind,
                                      Int fid,
                                      AbstractResidualFunctional * res_fn,
                                      const std::string & region);

public:
    static Parameters parameters();
};

/// Add residual statement for a field variable
///
/// @param fid Field ID
/// @param res_fn Integrand for the test function term
/// @param region Region name where this residual statement is active
template <>
inline void
ExplicitFELinearProblem::add_residual_block<WeakForm::F0>(Int fid,
                                                          AbstractResidualFunctional * res_fn,
                                                          const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_residual_block(WeakForm::F0, fid, res_fn, region);
}

/// Add residual statement for a field variable
///
/// @param fid Field ID
/// @param res_fn Integrand for the test function gradient term
/// @param region Region name where this residual statement is active
template <>
inline void
ExplicitFELinearProblem::add_residual_block<WeakForm::F1>(Int fid,
                                                          AbstractResidualFunctional * res_fn,
                                                          const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_residual_block(WeakForm::F1, fid, res_fn, region);
}

} // namespace godzilla
