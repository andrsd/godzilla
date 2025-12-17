// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

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
        n_comp(prob->get_field_num_components(FieldID(0)))
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
    auto params = FENonlinearProblem::parameters();
    params += ExplicitProblemInterface::parameters();
    return params;
}

ExplicitFELinearProblem::ExplicitFELinearProblem(const Parameters & pars) :
    FENonlinearProblem(pars),
    ExplicitProblemInterface(this, pars)
{
    CALL_STACK_MSG();
    set_default_output_on(ExecuteOn::INITIAL | ExecuteOn::TIMESTEP);
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

SNESolver
ExplicitFELinearProblem::create_sne_solver()
{
    return ExplicitProblemInterface::get_snes();
}

void
ExplicitFELinearProblem::init()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::init();
    NonlinearProblem::init();
    FEProblemInterface::init();
    // so that the call to DMTSCreateRHSMassMatrix would form the mass matrix
    for (Int i = 0; i < get_num_fields(); ++i)
        add_jacobian_block(FieldID(i), FieldID(i), new G0Identity(this), nullptr, nullptr, nullptr);

    auto ds = get_ds();
    for (auto & [_, info] : get_fields())
        PETSC_CHECK(PetscDSSetImplicit(ds, info.id.value(), PETSC_FALSE));
}

void
ExplicitFELinearProblem::create()
{
    CALL_STACK_MSG();
    FENonlinearProblem::create();
    ExplicitProblemInterface::create();
}

void
ExplicitFELinearProblem::solve()
{
    CALL_STACK_MSG();
    lprintln(9, "Solving");
    TransientProblemInterface::solve(get_solution_vector());
}

void
ExplicitFELinearProblem::run()
{
    CALL_STACK_MSG();
    pre_solve();
    solve();
    post_solve();
    if (converged())
        on_final();
}

bool
ExplicitFELinearProblem::converged()
{
    CALL_STACK_MSG();
    return TransientProblemInterface::get_converged_reason() > 0;
}

void
ExplicitFELinearProblem::compute_solution_vector_local()
{
    CALL_STACK_MSG();
    auto & loc_sln = get_solution_vector_local();
    global_to_local(get_solution_vector(), INSERT_VALUES, loc_sln);
    ExplicitProblemInterface::compute_boundary_local(get_time(), loc_sln);
}

void
ExplicitFELinearProblem::set_up_callbacks()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::set_up_callbacks();
}

void
ExplicitFELinearProblem::set_up_monitors()
{
    CALL_STACK_MSG();
    FENonlinearProblem::set_up_monitors();
    ExplicitProblemInterface::set_up_monitors();
}

void
ExplicitFELinearProblem::compute_rhs_local(Real time, const Vector & x, Vector & F)
{
    CALL_STACK_MSG();
    compute_rhs_function_fem(time, x, F);
}

void
ExplicitFELinearProblem::post_step()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::post_step();
    update_aux_vector();
    compute_postprocessors();
    output(ExecuteOn::TIMESTEP);
}

void
ExplicitFELinearProblem::add_residual_block(FieldID field_id,
                                            ResidualFunc * f0,
                                            ResidualFunc * f1,
                                            String region)
{
    CALL_STACK_MSG();
    // `compute_rhs_function_fem` which does the integration sets `Region::part = 100`. This matches
    // what PETSc does internally, so we do this as well in case we get inside PETSc.
    // See also `PetscDSSetRHSResidual` where the `part` is set to `100` as well which matches what
    // this function does
    const Int part = 100;

    if (region.length() == 0) {
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

void
ExplicitFELinearProblem::compute_rhs_function_fem(Real time, const Vector & loc_x, Vector & loc_g)
{
    // this is based on DMPlexTSComputeRHSFunctionFEM()
    CALL_STACK_MSG();
    auto all_cells = get_mesh()->get_all_cells();

    for (auto region : get_weak_form().get_residual_regions()) {
        IndexSet cells;
        region.value = 0;
        region.part = 100;
        if (region.label.is_null()) {
            all_cells.inc_reference();
            cells = all_cells;
        }
        else {
            region.value = 1;
            auto points = region.label.get_stratum(region.value);
            cells = IndexSet::intersect_caching(all_cells, points);
        }
        compute_residual_internal(get_dm(), region, cells, time, loc_x, nullptr, time, loc_g);
    }
}

} // namespace godzilla
