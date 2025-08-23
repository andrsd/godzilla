// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/ImplicitFENonlinearProblem.h"
#include "godzilla/WeakForm.h"
#include "godzilla/Validation.h"
#include "godzilla/Utils.h"
#include "godzilla/IndexSet.h"
#include <petsc/private/tsimpl.h>

namespace godzilla {

ErrorCode
ImplicitFENonlinearProblem::invoke_compute_boundary_delegate(DM,
                                                             Real time,
                                                             Vec x,
                                                             Vec x_t,
                                                             void * context)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(Real time, Vector & x, Vector & x_t)> *>(context);
    Vector vec_x(x);
    vec_x.inc_reference();
    Vector vec_x_t(x_t);
    vec_x_t.inc_reference();
    method->invoke(time, vec_x, vec_x_t);
    return 0;
}

//

Parameters
ImplicitFENonlinearProblem::parameters()
{
    Parameters params = FENonlinearProblem::parameters();
    params += TransientProblemInterface::parameters();
    params.add_param<std::string>("scheme", "beuler", "Time stepping scheme: [beuler, cn]");
    return params;
}

ImplicitFENonlinearProblem::ImplicitFENonlinearProblem(const Parameters & params) :
    FENonlinearProblem(params),
    TransientProblemInterface(this, params),
    scheme(get_param<std::string>("scheme"))
{
    CALL_STACK_MSG();
    set_default_output_on(EXECUTE_ON_INITIAL | EXECUTE_ON_TIMESTEP);
    if (!validation::in(this->scheme, { "beuler", "cn" }))
        log_error("The 'scheme' parameter can be either 'beuler' or 'cn'.");
}

ImplicitFENonlinearProblem::~ImplicitFENonlinearProblem()
{
    CALL_STACK_MSG();
}

Real
ImplicitFENonlinearProblem::get_time() const
{
    CALL_STACK_MSG();
    return TransientProblemInterface::get_time();
}

Int
ImplicitFENonlinearProblem::get_step_num() const
{
    CALL_STACK_MSG();
    return TransientProblemInterface::get_step_number();
}

SNESolver
ImplicitFENonlinearProblem::create_sne_solver()
{
    return TransientProblemInterface::get_snes();
}

void
ImplicitFENonlinearProblem::init()
{
    CALL_STACK_MSG();
    TransientProblemInterface::init();
    FENonlinearProblem::init();
}

void
ImplicitFENonlinearProblem::create()
{
    CALL_STACK_MSG();
    FENonlinearProblem::create();
    TransientProblemInterface::create();
}

void
ImplicitFENonlinearProblem::solve()
{
    CALL_STACK_MSG();
    lprintln(9, "Solving");
    TransientProblemInterface::solve(get_solution_vector());
}

bool
ImplicitFENonlinearProblem::converged()
{
    CALL_STACK_MSG();
    return TransientProblemInterface::get_converged_reason() > 0;
}

void
ImplicitFENonlinearProblem::run()
{
    CALL_STACK_MSG();
    pre_solve();
    solve();
    post_solve();
    if (converged())
        on_final();
}

void
ImplicitFENonlinearProblem::set_up_callbacks()
{
    CALL_STACK_MSG();
    TransientProblemInterface::set_up_callbacks();
    set_time_boundary_local(this, &ImplicitFENonlinearProblem::compute_boundary_fem);
    set_ifunction_local(this, &ImplicitFENonlinearProblem::compute_ifunction_fem);
    set_ijacobian_local(this, &ImplicitFENonlinearProblem::compute_ijacobian_fem);
}

void
ImplicitFENonlinearProblem::set_up_time_scheme()
{
    CALL_STACK_MSG();
    std::string name = utils::to_lower(this->scheme);
    std::map<std::string, TimeScheme> scheme_map = { { "beuler", TimeScheme::BEULER },
                                                     { "cn", TimeScheme::CN } };
    set_scheme(scheme_map[name]);
}

void
ImplicitFENonlinearProblem::set_up_monitors()
{
    CALL_STACK_MSG();
    FENonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

void
ImplicitFENonlinearProblem::compute_solution_vector_local()
{
    CALL_STACK_MSG();
    auto loc_sln = get_solution_vector_local();
    global_to_local(get_solution_vector(), INSERT_VALUES, loc_sln);
    TransientProblemInterface::compute_boundary_local(get_time(), loc_sln);
}

void
ImplicitFENonlinearProblem::compute_ifunction_fem(Real time,
                                                  const Vector & x,
                                                  const Vector & x_t,
                                                  Vector & F)
{
    // this is based on DMSNESComputeResidual() and DMPlexTSComputeIFunctionFEM()
    CALL_STACK_MSG();
    auto all_cells = get_mesh()->get_all_cells();

    for (auto & region : get_weak_form()->get_residual_regions()) {
        IndexSet cells;
        if (region.label.is_null()) {
            all_cells.inc_reference();
            cells = all_cells;
        }
        else {
            auto points = region.label.get_stratum(region.value);
            cells = IndexSet::intersect_caching(all_cells, points);
            points.destroy();
        }
        compute_residual_internal(get_dm(), region, cells, time, x, x_t, time, F);
    }
}

void
ImplicitFENonlinearProblem::compute_ijacobian_fem(Real time,
                                                  const Vector & x,
                                                  const Vector & x_t,
                                                  Real x_t_shift,
                                                  Matrix & J,
                                                  Matrix & Jp)
{
    // this is based on DMPlexSNESComputeJacobianFEM(), DMSNESComputeJacobianAction() and
    // DMPlexTSComputeIJacobianFEM()
    CALL_STACK_MSG();
    auto all_cells = get_mesh()->get_all_cells();

    Jp.zero();

    for (auto & region : get_weak_form()->get_jacobian_regions()) {
        IndexSet cells;
        if (region.label.is_null()) {
            all_cells.inc_reference();
            cells = all_cells;
        }
        else {
            auto points = region.label.get_stratum(region.value);
            cells = IndexSet::intersect_caching(all_cells, points);
        }
        compute_jacobian_internal(get_dm(), region, cells, time, x_t_shift, x, x_t, J, Jp);
    }
}

void
ImplicitFENonlinearProblem::compute_boundary_fem(Real time, Vector & x, Vector & x_t)
{
    CALL_STACK_MSG();
    auto dm = get_dm();
    PETSC_CHECK(DMPlexTSComputeBoundary(dm, time, x, x_t, this));
}

void
ImplicitFENonlinearProblem::post_step()
{
    CALL_STACK_MSG();
    TransientProblemInterface::post_step();
    update_aux_vector();
    compute_postprocessors();
    output(EXECUTE_ON_TIMESTEP);
}

} // namespace godzilla
