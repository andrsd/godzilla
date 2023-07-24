#include "Godzilla.h"
#include "CallStack.h"
#include "UnstructuredMesh.h"
#include "ImplicitFENonlinearProblem.h"
#include "WeakForm.h"
#include "Output.h"
#include "Validation.h"
#include "Utils.h"
#include "IndexSet.h"
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

// Taken from PETSc: dmplexts.c
static PetscErrorCode
DMTSConvertPlex(DM dm, DM * plex, PetscBool copy)
{
    PetscBool isPlex;

    PetscFunctionBegin;
    PetscCall(PetscObjectTypeCompare((PetscObject) dm, DMPLEX, &isPlex));
    if (isPlex) {
        *plex = dm;
        PetscCall(PetscObjectReference((PetscObject) dm));
    }
    else {
        PetscCall(PetscObjectQuery((PetscObject) dm, "dm_plex", (PetscObject *) plex));
        if (!*plex) {
            PetscCall(DMConvert(dm, DMPLEX, plex));
            PetscCall(PetscObjectCompose((PetscObject) dm, "dm_plex", (PetscObject) *plex));
            if (copy) {
                PetscCall(DMCopyDMTS(dm, *plex));
                PetscCall(DMCopyDMSNES(dm, *plex));
                PetscCall(DMCopyAuxiliaryVec(dm, *plex));
            }
        }
        else {
            PetscCall(PetscObjectReference((PetscObject) *plex));
        }
    }
    PetscFunctionReturn(0);
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
    this->default_output_on = Output::ON_INITIAL | Output::ON_TIMESTEP;
}

ImplicitFENonlinearProblem::~ImplicitFENonlinearProblem()
{
    _F_;
    this->snes = nullptr;
    this->ksp = nullptr;
}

Real
ImplicitFENonlinearProblem::get_time() const
{
    _F_;
    return this->time;
}

Int
ImplicitFENonlinearProblem::get_step_num() const
{
    _F_;
    return this->step_num;
}

void
ImplicitFENonlinearProblem::init()
{
    _F_;
    TransientProblemInterface::init();
    PETSC_CHECK(TSGetSNES(this->ts, &this->snes));
    PETSC_CHECK(SNESGetKSP(this->snes, &this->ksp));

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
    lprintf(9, "Solving");
    TransientProblemInterface::solve(this->x);
}

void
ImplicitFENonlinearProblem::set_up_callbacks()
{
    _F_;
    DM dm = get_dm();
    PETSC_CHECK(DMTSSetBoundaryLocal(dm, _tsfep_compute_boundary, this));
    PETSC_CHECK(DMTSSetIFunctionLocal(dm, __tsfep_compute_ifunction, this));
    PETSC_CHECK(DMTSSetIJacobianLocal(dm, __tsfep_compute_ijacobian, this));
}

void
ImplicitFENonlinearProblem::set_up_time_scheme()
{
    _F_;
    std::string sch = utils::to_lower(this->scheme);
    if (sch == "beuler")
        PETSC_CHECK(TSSetType(this->ts, TSBEULER));
    else if (sch == "cn")
        PETSC_CHECK(TSSetType(this->ts, TSCN));
}

void
ImplicitFENonlinearProblem::set_up_monitors()
{
    _F_;
    FENonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

const Vector &
ImplicitFENonlinearProblem::get_solution_vector_local()
{
    _F_;
    auto & loc_sln = this->sln;
    PETSC_CHECK(DMGlobalToLocal(get_dm(), get_solution_vector(), INSERT_VALUES, loc_sln));
    compute_boundary(get_time(), loc_sln, Vector());
    return loc_sln;
}

PetscErrorCode
ImplicitFENonlinearProblem::compute_ifunction(Real time,
                                              const Vector & X,
                                              const Vector & X_t,
                                              Vector & F)
{
    // this is based on DMSNESComputeResidual() and DMPlexTSComputeIFunctionFEM()
    _F_;
    DM plex;
    PetscCall(DMTSConvertPlex(get_dm(), &plex, PETSC_TRUE));

    IndexSet all_cells = this->unstr_mesh->get_all_cells();

    Int n_ds;
    PETSC_CHECK(DMGetNumDS(plex, &n_ds));
    for (Int s = 0; s < n_ds; ++s) {
        PetscDS ds;
        DMLabel label;
        PETSC_CHECK(DMGetRegionNumDS(plex, s, &label, nullptr, &ds));

        for (auto & res_key : this->wf->get_residual_keys()) {
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
            compute_residual_internal(plex, res_key, cells, time, X, X_t, time, F);
            cells.destroy();
        }
    }
    all_cells.destroy();
    PetscCall(DMDestroy(&plex));
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
    DM plex;
    PetscCall(DMTSConvertPlex(get_dm(), &plex, PETSC_TRUE));

    IndexSet all_cells = this->unstr_mesh->get_all_cells();

    Int n_ds;
    PetscCall(DMGetNumDS(plex, &n_ds));
    for (Int s = 0; s < n_ds; ++s) {
        PetscDS ds;
        DMLabel label;
        PetscCall(DMGetRegionNumDS(plex, s, &label, nullptr, &ds));

        if (s == 0)
            Jp.zero();

        for (auto & jac_key : this->wf->get_jacobian_keys()) {
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
            compute_jacobian_internal(plex, jac_key, cells, time, x_t_shift, X, X_t, J, Jp);
            cells.destroy();
        }
    }
    all_cells.destroy();
    PetscCall(DMDestroy(&plex));
    return 0;
}

PetscErrorCode
ImplicitFENonlinearProblem::compute_boundary(Real time, const Vector & X, const Vector & X_t)
{
    _F_;
    return DMPlexTSComputeBoundary(get_dm(), time, X, X_t, this);
}

} // namespace godzilla
