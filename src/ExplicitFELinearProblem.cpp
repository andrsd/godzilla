// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/ExplicitFELinearProblem.h"
#include "godzilla/WeakForm.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/Output.h"

namespace godzilla {

namespace {

class G0Identity : public JacobianFunc {
public:
    explicit G0Identity(ExplicitFELinearProblem * prob) :
        JacobianFunc(prob),
        n_comp(prob->get_field_num_components(0))
    {
    }

    void
    evaluate(Scalar * g) const override
    {
        for (Int c = 0; c < n_comp; ++c)
            g[c * n_comp + c] = 1.0;
    }

protected:
    Int n_comp;
};

} // namespace

Parameters
ExplicitFELinearProblem::parameters()
{
    Parameters params = FENonlinearProblem::parameters();
    params += ExplicitProblemInterface::parameters();
    return params;
}

ExplicitFELinearProblem::ExplicitFELinearProblem(const Parameters & params) :
    FENonlinearProblem(params),
    ExplicitProblemInterface(this, params)
{
    CALL_STACK_MSG();
    set_default_output_on(EXECUTE_ON_INITIAL | EXECUTE_ON_TIMESTEP);
}

ExplicitFELinearProblem::~ExplicitFELinearProblem()
{
    CALL_STACK_MSG();
    set_snes(nullptr);
}

Real
ExplicitFELinearProblem::get_time() const
{
    CALL_STACK_MSG();
    return ExplicitProblemInterface::get_time();
}

Int
ExplicitFELinearProblem::get_step_num() const
{
    CALL_STACK_MSG();
    return ExplicitProblemInterface::get_step_number();
}

void
ExplicitFELinearProblem::init()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::init();
    auto snes = ExplicitProblemInterface::get_snes();
    NonlinearProblem::set_snes(snes);
    FEProblemInterface::init();
    // so that the call to DMTSCreateRHSMassMatrix would form the mass matrix
    for (Int i = 0; i < get_num_fields(); i++)
        add_jacobian_block(i, i, new G0Identity(this), nullptr, nullptr, nullptr);

    auto ds = get_ds();
    for (auto & f : get_fields()) {
        Int fid = f.second.id;
        PETSC_CHECK(PetscDSSetImplicit(ds, fid, PETSC_FALSE));
    }
}

void
ExplicitFELinearProblem::create()
{
    CALL_STACK_MSG();
    FENonlinearProblem::create();
    ExplicitProblemInterface::create();
}

bool
ExplicitFELinearProblem::converged()
{
    CALL_STACK_MSG();
    return ExplicitProblemInterface::converged();
}

void
ExplicitFELinearProblem::solve()
{
    CALL_STACK_MSG();
    lprint(9, "Solving");
    ExplicitProblemInterface::solve(get_solution_vector());
}

void
ExplicitFELinearProblem::compute_solution_vector_local()
{
    CALL_STACK_MSG();
    auto & loc_sln = get_solution_vector_local();
    PETSC_CHECK(DMGlobalToLocal(get_dm(), get_solution_vector(), INSERT_VALUES, loc_sln));
    compute_boundary_local(get_time(), loc_sln);
}

void
ExplicitFELinearProblem::set_up_callbacks()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::set_up_callbacks();
}

void
ExplicitFELinearProblem::set_up_time_scheme()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::set_up_time_scheme();
}

void
ExplicitFELinearProblem::set_up_monitors()
{
    CALL_STACK_MSG();
    FENonlinearProblem::set_up_monitors();
    ExplicitProblemInterface::set_up_monitors();
}

PetscErrorCode
ExplicitFELinearProblem::compute_rhs_local(Real time, const Vector & x, Vector & F)
{
    CALL_STACK_MSG();
    return DMPlexTSComputeRHSFunctionFEM(get_dm(), time, x, F, this);
}

void
ExplicitFELinearProblem::post_step()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::post_step();
    update_aux_vector();
    compute_postprocessors();
    output(EXECUTE_ON_TIMESTEP);
}

void
ExplicitFELinearProblem::add_residual_block(Int field_id,
                                            ResidualFunc * f0,
                                            ResidualFunc * f1,
                                            const std::string & region)
{
    CALL_STACK_MSG();
    // see PetscDSSetRHSResidual for explanation
    Int part = 100;

    if (region.empty()) {
        add_weak_form_residual_block(WeakForm::F0, field_id, f0, Label(), 0, part);
        add_weak_form_residual_block(WeakForm::F1, field_id, f1, Label(), 0, part);
    }
    else {
        auto label = get_mesh()->get_label(region);
        auto ids = label.get_values();
        for (auto & val : ids) {
            add_weak_form_residual_block(WeakForm::F0, field_id, f0, label, val, part);
            add_weak_form_residual_block(WeakForm::F1, field_id, f1, label, val, part);
        }
    }
}

} // namespace godzilla
