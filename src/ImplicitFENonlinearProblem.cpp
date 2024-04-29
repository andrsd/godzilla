// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/ImplicitFENonlinearProblem.h"
#include "godzilla/WeakForm.h"
#include "godzilla/Output.h"
#include "godzilla/Validation.h"
#include "godzilla/Utils.h"
#include "godzilla/IndexSet.h"
#include "petscts.h"
#include <petsc/private/tsimpl.h>

namespace godzilla {

static PetscErrorCode
__tsfep_compute_ifunction(DM, Real time, Vec x, Vec x_t, Vec F, void * user)
{
    CALL_STACK_MSG();
    auto * fep = static_cast<ImplicitFENonlinearProblem *>(user);
    Vector vec_x(x);
    Vector vec_x_t(x_t);
    Vector vec_F(F);
    fep->compute_ifunction(time, vec_x, vec_x_t, vec_F);
    return 0;
}

static PetscErrorCode
__tsfep_compute_ijacobian(DM, Real time, Vec x, Vec x_t, Real x_t_shift, Mat J, Mat Jp, void * user)
{
    CALL_STACK_MSG();
    auto * fep = static_cast<ImplicitFENonlinearProblem *>(user);
    Vector vec_x(x);
    Vector vec_x_t(x_t);
    Matrix mat_J(J);
    Matrix mat_Jp(Jp);
    fep->compute_ijacobian(time, vec_x, vec_x_t, x_t_shift, mat_J, mat_Jp);
    return 0;
}

static PetscErrorCode
_tsfep_compute_boundary(DM, Real time, Vec x, Vec x_t, void * user)
{
    CALL_STACK_MSG();
    auto * fep = static_cast<ImplicitFENonlinearProblem *>(user);
    Vector vec_x(x);
    Vector vec_x_t(x_t);
    fep->compute_boundary(time, vec_x, vec_x_t);
    return 0;
}

///

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

void
ImplicitFENonlinearProblem::init()
{
    CALL_STACK_MSG();
    TransientProblemInterface::init();
    auto snes = TransientProblemInterface::get_snes();
    NonlinearProblem::set_snes(snes);
    FEProblemInterface::init();
}

void
ImplicitFENonlinearProblem::create()
{
    CALL_STACK_MSG();
    FENonlinearProblem::create();
    TransientProblemInterface::create();
}

bool
ImplicitFENonlinearProblem::converged()
{
    CALL_STACK_MSG();
    return TransientProblemInterface::converged();
}

void
ImplicitFENonlinearProblem::solve()
{
    CALL_STACK_MSG();
    lprint(9, "Solving");
    TransientProblemInterface::solve(get_solution_vector());
}

void
ImplicitFENonlinearProblem::set_up_callbacks()
{
    CALL_STACK_MSG();
    auto dm = get_dm();
    PETSC_CHECK(DMTSSetBoundaryLocal(dm, _tsfep_compute_boundary, this));
    PETSC_CHECK(DMTSSetIFunctionLocal(dm, __tsfep_compute_ifunction, this));
    PETSC_CHECK(DMTSSetIJacobianLocal(dm, __tsfep_compute_ijacobian, this));
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
    PETSC_CHECK(DMGlobalToLocal(get_dm(), get_solution_vector(), INSERT_VALUES, loc_sln));
    compute_boundary(get_time(), loc_sln, Vector());
}

PetscErrorCode
ImplicitFENonlinearProblem::compute_ifunction(Real time,
                                              const Vector & X,
                                              const Vector & X_t,
                                              Vector & F)
{
    // this is based on DMSNESComputeResidual() and DMPlexTSComputeIFunctionFEM()
    CALL_STACK_MSG();
    IndexSet all_cells = get_unstr_mesh()->get_all_cells();

    for (auto & res_key : get_weak_form()->get_residual_keys()) {
        IndexSet cells;
        if (res_key.label == nullptr) {
            all_cells.inc_ref();
            cells = all_cells;
        }
        else {
            Label l(res_key.label);
            auto points = l.get_stratum(res_key.value);
            cells = IndexSet::intersect_caching(all_cells, points);
            points.destroy();
        }
        compute_residual_internal(get_dm(), res_key, cells, time, X, X_t, time, F);
        cells.destroy();
    }

    return 0;
}

PetscErrorCode
ImplicitFENonlinearProblem::compute_ijacobian(Real time,
                                              const Vector & X,
                                              const Vector & X_t,
                                              Real x_t_shift,
                                              Matrix & J,
                                              Matrix & Jp)
{
    // this is based on DMPlexSNESComputeJacobianFEM(), DMSNESComputeJacobianAction() and
    // DMPlexTSComputeIJacobianFEM()
    CALL_STACK_MSG();
    IndexSet all_cells = get_unstr_mesh()->get_all_cells();

    Jp.zero();

    for (auto & jac_key : get_weak_form()->get_jacobian_keys()) {
        IndexSet cells;
        if (!jac_key.label) {
            all_cells.inc_ref();
            cells = all_cells;
        }
        else {
            Label l(jac_key.label);
            auto points = l.get_stratum(jac_key.value);
            cells = IndexSet::intersect_caching(all_cells, points);
            points.destroy();
        }
        compute_jacobian_internal(get_dm(), jac_key, cells, time, x_t_shift, X, X_t, J, Jp);
        cells.destroy();
    }

    return 0;
}

PetscErrorCode
ImplicitFENonlinearProblem::compute_boundary(Real time, const Vector & X, const Vector & X_t)
{
    CALL_STACK_MSG();
    return DMPlexTSComputeBoundary(get_dm(), time, X, X_t, this);
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
