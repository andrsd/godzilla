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
    _F_;
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
    _F_;
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
    _F_;
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
    _F_;
    set_default_output_on(ExecuteOn::INITIAL | ExecuteOn::TIMESTEP);
}

ImplicitFENonlinearProblem::~ImplicitFENonlinearProblem()
{
    _F_;
    set_snes(nullptr);
}

Real
ImplicitFENonlinearProblem::get_time() const
{
    _F_;
    return TransientProblemInterface::get_time();
}

Int
ImplicitFENonlinearProblem::get_step_num() const
{
    _F_;
    return TransientProblemInterface::get_step_number();
}

void
ImplicitFENonlinearProblem::init()
{
    _F_;
    TransientProblemInterface::init();
    auto snes = TransientProblemInterface::get_snes();
    NonlinearProblem::set_snes(snes);
    FEProblemInterface::init();
}

void
ImplicitFENonlinearProblem::create()
{
    _F_;
    FENonlinearProblem::create();
    TransientProblemInterface::create();
}

void
ImplicitFENonlinearProblem::check()
{
    _F_;
    FENonlinearProblem::check();
    TransientProblemInterface::check(this);

    if (!validation::in(this->scheme, { "beuler", "cn" }))
        log_error("The 'scheme' parameter can be either 'beuler' or 'cn'.");
}

bool
ImplicitFENonlinearProblem::converged()
{
    _F_;
    return TransientProblemInterface::converged();
}

void
ImplicitFENonlinearProblem::solve()
{
    _F_;
    lprint(9, "Solving");
    TransientProblemInterface::solve(get_solution_vector());
}

void
ImplicitFENonlinearProblem::set_up_callbacks()
{
    _F_;
    auto dm = get_dm();
    PETSC_CHECK(DMTSSetBoundaryLocal(dm, _tsfep_compute_boundary, this));
    PETSC_CHECK(DMTSSetIFunctionLocal(dm, __tsfep_compute_ifunction, this));
    PETSC_CHECK(DMTSSetIJacobianLocal(dm, __tsfep_compute_ijacobian, this));
}

void
ImplicitFENonlinearProblem::set_up_time_scheme()
{
    _F_;
    std::string name = utils::to_lower(this->scheme);
    std::map<std::string, TimeScheme> scheme_map = { { "beuler", TimeScheme::BEULER },
                                                     { "cn", TimeScheme::CN } };
    set_scheme(scheme_map[name]);
}

void
ImplicitFENonlinearProblem::set_up_monitors()
{
    _F_;
    FENonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

void
ImplicitFENonlinearProblem::build_local_solution_vector(Vector & loc_sln)
{
    _F_;
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
    _F_;
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
    _F_;
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
    _F_;
    return DMPlexTSComputeBoundary(get_dm(), time, X, X_t, this);
}

void
ImplicitFENonlinearProblem::post_step()
{
    _F_;
    TransientProblemInterface::post_step();
    update_aux_vector();
    compute_postprocessors();
    output(ExecuteOn::TIMESTEP);
}

} // namespace godzilla
