#include "FENonlinearProblem.h"
#include "Mesh.h"
#include "H1Space.h"
#include "H1LobattoShapesetEdge.h"
#include "Utils.h"
#include "CallStack.h"
#include "PetscVector.h"
#include "PetscMatrix.h"
#include "petscdm.h"

namespace godzilla {

PetscErrorCode
__compute_residual(SNES snes, Vec x, Vec f, void * ctx)
{
    _F_;
    FENonlinearProblem * problem = static_cast<FENonlinearProblem *>(ctx);
    return problem->compute_residual_callback(x, f);
}

PetscErrorCode
__compute_jacobian(SNES snes, Vec x, Mat J, Mat Jp, void * ctx)
{
    _F_;
    FENonlinearProblem * problem = static_cast<FENonlinearProblem *>(ctx);
    return problem->compute_jacobian_callback(x, J, Jp);
}

PetscErrorCode
__ksp_monitor(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx)
{
    _F_;
    FENonlinearProblem * problem = static_cast<FENonlinearProblem *>(ctx);
    return problem->ksp_monitor_callback(it, rnorm);
}

PetscErrorCode
__snes_monitor(SNES snes, PetscInt it, PetscReal norm, void * ctx)
{
    _F_;
    FENonlinearProblem * problem = static_cast<FENonlinearProblem *>(ctx);
    return problem->snes_monitor_callback(it, norm);
}

InputParameters
FENonlinearProblem::validParams()
{
    InputParameters params = Problem::validParams();

    params.add_private_param<Mesh *>("_mesh");
    params.add_param<std::string>("line_search", "bt", "The type of line search to be used");
    params.add_param<PetscReal>("nl_rel_tol",
                                1e-8,
                                "Relative convergence tolerance for the non-linear solver");
    params.add_param<PetscReal>("nl_abs_tol",
                                1e-15,
                                "Absolute convergence tolerance for the non-linear solver");
    params.add_param<PetscReal>(
        "nl_step_tol",
        1e-15,
        "Convergence tolerance in terms of the norm of the change in the solution between steps");
    params.add_param<PetscInt>("nl_max_iter",
                               40,
                               "Maximum number of iterations for the non-linear solver");
    params.add_param<PetscReal>("lin_rel_tol",
                                1e-5,
                                "Relative convergence tolerance for the linear solver");
    params.add_param<PetscReal>("lin_abs_tol",
                                1e-50,
                                "Absolute convergence tolerance for the linear solver");
    params.add_param<PetscInt>("lin_max_iter",
                               10000,
                               "Maximum number of iterations for the linear solver");
    return params;
}

FENonlinearProblem::FENonlinearProblem(const InputParameters & parameters) :
    Problem(parameters),
    mesh(get_param<Mesh *>("_mesh")),
    shapeset(nullptr),
    snes(NULL),
    x(NULL),
    r(NULL),
    J(NULL),
    Jp(NULL),
    line_search_type(get_param<std::string>("line_search")),
    nl_rel_tol(get_param<PetscReal>("nl_rel_tol")),
    nl_abs_tol(get_param<PetscReal>("nl_abs_tol")),
    nl_step_tol(get_param<PetscReal>("nl_step_tol")),
    nl_max_iter(get_param<PetscInt>("nl_max_iter")),
    lin_rel_tol(get_param<PetscReal>("lin_rel_tol")),
    lin_abs_tol(get_param<PetscReal>("lin_abs_tol")),
    lin_max_iter(get_param<PetscInt>("lin_max_iter"))
{
    _F_;
    line_search_type = utils::to_lower(line_search_type);
}

FENonlinearProblem::~FENonlinearProblem()
{
    _F_;
    if (this->snes)
        SNESDestroy(&this->snes);
    if (this->r)
        VecDestroy(&this->r);
    if (this->x)
        VecDestroy(&this->x);
    if ((this->Jp != this->J) && (this->Jp))
        MatDestroy(&this->Jp);
    if (this->J)
        MatDestroy(&this->J);
}

DM
FENonlinearProblem::get_dm() const
{
    _F_;
    return this->mesh->get_dm();
}

void
FENonlinearProblem::create()
{
    _F_;
    init();
    allocate_objects();
    on_set_matrix_properties();

    setup_solver_parameters();
    setup_line_search();
    setup_monitors();
    setup_callbacks();

    setup_initial_guess();
    Problem::create();

    on_set_fields();
    assign_dofs();
    // TOOD: create ICs
    // TOOD: create BCs
}

void
FENonlinearProblem::init()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = get_dm();

    ierr = SNESCreate(comm(), &this->snes);
    checkPetscError(ierr);
    ierr = SNESSetDM(this->snes, dm);
    checkPetscError(ierr);
    ierr = DMSetApplicationContext(dm, this);
    checkPetscError(ierr);
}

void
FENonlinearProblem::setup_initial_guess()
{
    _F_;
    PetscErrorCode ierr;
    ierr = VecSet(this->x, 0.);
    checkPetscError(ierr);
}

void
FENonlinearProblem::allocate_objects()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = get_dm();

    ierr = DMCreateGlobalVector(dm, &this->x);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->x, "sln");
    checkPetscError(ierr);

    ierr = VecDuplicate(this->x, &this->r);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->r, "res");
    checkPetscError(ierr);

    ierr = DMCreateMatrix(dm, &this->J);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->J, "Jac");
    checkPetscError(ierr);

    // full newton
    this->Jp = this->J;
}

void
FENonlinearProblem::setup_line_search()
{
    _F_;
    SNESLineSearch line_search;
    SNESGetLineSearch(this->snes, &line_search);
    if (this->line_search_type.compare("basic") == 0)
        SNESLineSearchSetType(line_search, SNESLINESEARCHBASIC);
    else if (this->line_search_type.compare("l2") == 0)
        SNESLineSearchSetType(line_search, SNESLINESEARCHL2);
    else if (this->line_search_type.compare("cp") == 0)
        SNESLineSearchSetType(line_search, SNESLINESEARCHCP);
    else if (this->line_search_type.compare("nleqerr") == 0)
        SNESLineSearchSetType(line_search, SNESLINESEARCHNLEQERR);
    else if (this->line_search_type.compare("shell") == 0)
        SNESLineSearchSetType(line_search, SNESLINESEARCHSHELL);
    else
        SNESLineSearchSetType(line_search, SNESLINESEARCHBT);
    SNESSetLineSearch(this->snes, line_search);
    SNESLineSearchSetFromOptions(line_search);
}

void
FENonlinearProblem::setup_callbacks()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSetFunction(this->snes, this->r, __compute_residual, this);
    checkPetscError(ierr);
    ierr = SNESSetJacobian(this->snes, this->J, this->Jp, __compute_jacobian, this);
    checkPetscError(ierr);
}

void
FENonlinearProblem::setup_monitors()
{
    _F_;
    SNESMonitorSet(this->snes, __snes_monitor, this, 0);

    KSP ksp;
    SNESGetKSP(this->snes, &ksp);
    KSPMonitorSet(ksp, __ksp_monitor, this, 0);
}

void
FENonlinearProblem::setup_solver_parameters()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSetTolerances(this->snes,
                             this->nl_abs_tol,
                             this->nl_rel_tol,
                             this->nl_step_tol,
                             this->nl_max_iter,
                             -1);
    checkPetscError(ierr);
    ierr = SNESSetFromOptions(this->snes);
    checkPetscError(ierr);

    KSP ksp;
    ierr = SNESGetKSP(this->snes, &ksp);
    checkPetscError(ierr);
    ierr = KSPSetTolerances(ksp,
                            this->lin_rel_tol,
                            this->lin_abs_tol,
                            PETSC_DEFAULT,
                            this->lin_max_iter);
    checkPetscError(ierr);
    ierr = KSPSetFromOptions(ksp);
    checkPetscError(ierr);
}

PetscErrorCode
FENonlinearProblem::snes_monitor_callback(PetscInt it, PetscReal norm)
{
    godzilla_print(7, it, " Non-linear residual: ", std::scientific, norm);
    return 0;
}

PetscErrorCode
FENonlinearProblem::ksp_monitor_callback(PetscInt it, PetscReal rnorm)
{
    godzilla_print(8, "    ", it, " Linear residual: ", std::scientific, rnorm);
    return 0;
}

void
FENonlinearProblem::assemble_residual(const PetscVector * x, PetscVector * rhs)
{
    _F_;
}

void
FENonlinearProblem::assemble_jacobian(const PetscVector * x, PetscMatrix * jac)
{
    _F_;
}

void
FENonlinearProblem::solve()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSolve(this->snes, NULL, this->x);
    checkPetscError(ierr);
    ierr = SNESGetConvergedReason(this->snes, &this->converged_reason);
    checkPetscError(ierr);
}

bool
FENonlinearProblem::converged()
{
    _F_;
    bool conv = (this->converged_reason == SNES_CONVERGED_FNORM_ABS) ||
                (this->converged_reason == SNES_CONVERGED_FNORM_RELATIVE) ||
                (this->converged_reason == SNES_CONVERGED_SNORM_RELATIVE) ||
                (this->converged_reason == SNES_CONVERGED_ITS);
    return conv;
}

Vec
FENonlinearProblem::get_solution_vector() const
{
    _F_;
    return this->x;
}

void
FENonlinearProblem::run()
{
    _F_;
    solve();
}

PetscErrorCode
FENonlinearProblem::compute_residual_callback(Vec x, Vec f)
{
    _F_;
    PetscVector xx(x);
    PetscVector ff(f);
    assemble_residual(&xx, &ff);
    return 0;
}

PetscErrorCode
FENonlinearProblem::compute_jacobian_callback(Vec x, Mat J, Mat /*Jp*/)
{
    _F_;
    PetscVector xx(x);
    PetscMatrix JJ(J);
    assemble_jacobian(&xx, &JJ);
    return 0;
}

void
FENonlinearProblem::add_variable(const std::string & name, uint nc, uint p)
{
    _F_;

    if (this->shapeset == nullptr) {
        this->shapeset = new H1LobattoShapesetEdge();
    }

    H1Space * spc = new H1Space(this->mesh, this->shapeset);
    spc->set_uniform_order(p);

    auto idx = this->spaces.size();
    this->spaces.push_back(spc);
    this->space_names[name] = idx;
}

void
FENonlinearProblem::assign_dofs()
{
    _F_;
    uint ndofs = 0;
    for (auto & sp : this->spaces) {
        sp->assign_dofs(ndofs);
        ndofs += sp->get_dof_count();
    }
    godzilla_print(9, "Number of DoFs: ", ndofs);
}

void
FENonlinearProblem::on_set_matrix_properties()
{
}

} // namespace godzilla
