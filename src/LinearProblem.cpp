#include "LinearProblem.h"
#include "CallStack.h"
#include "Mesh.h"
#include "Output.h"

namespace godzilla {

PetscErrorCode
__compute_rhs(KSP, Vec b, void * ctx)
{
    _F_;
    auto * problem = static_cast<LinearProblem *>(ctx);
    return problem->compute_rhs_callback(b);
}

PetscErrorCode
__compute_operators(KSP, Mat A, Mat B, void * ctx)
{
    _F_;
    auto * problem = static_cast<LinearProblem *>(ctx);
    return problem->compute_operators_callback(A, B);
}

PetscErrorCode
__ksp_monitor_linear(KSP, PetscInt it, PetscReal rnorm, void * ctx)
{
    _F_;
    auto * problem = static_cast<LinearProblem *>(ctx);
    return problem->ksp_monitor_callback(it, rnorm);
}

Parameters
LinearProblem::parameters()
{
    Parameters params = Problem::parameters();
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

LinearProblem::LinearProblem(const Parameters & parameters) :
    Problem(parameters),
    ksp(nullptr),
    x(nullptr),
    b(nullptr),
    A(nullptr),
    converged_reason(KSP_CONVERGED_ITERATING),
    lin_rel_tol(get_param<PetscReal>("lin_rel_tol")),
    lin_abs_tol(get_param<PetscReal>("lin_abs_tol")),
    lin_max_iter(get_param<PetscInt>("lin_max_iter"))
{
    _F_;
    this->default_output_on = Output::ON_FINAL;
}

LinearProblem::~LinearProblem()
{
    _F_;
    if (this->ksp)
        KSPDestroy(&this->ksp);
    if (this->b)
        VecDestroy(&this->b);
    if (this->x)
        VecDestroy(&this->x);
    if (this->A)
        MatDestroy(&this->A);
}

DM
LinearProblem::get_dm() const
{
    _F_;
    return this->mesh->get_dm();
}

Vec
LinearProblem::get_solution_vector() const
{
    _F_;
    return this->x;
}

void
LinearProblem::create()
{
    _F_;
    const_cast<Mesh *>(this->mesh)->distribute();
    init();
    allocate_objects();
    set_up_matrix_properties();
    set_up_preconditioning();

    set_up_solver_parameters();
    set_up_monitors();
    set_up_callbacks();

    Problem::create();
}

void
LinearProblem::init()
{
    _F_;
    DM dm = get_dm();
    PETSC_CHECK(KSPCreate(get_comm(), &this->ksp));
    PETSC_CHECK(KSPSetDM(this->ksp, dm));
    PETSC_CHECK(DMSetApplicationContext(dm, this));
}

void
LinearProblem::allocate_objects()
{
    _F_;
    DM dm = get_dm();

    PETSC_CHECK(DMCreateGlobalVector(dm, &this->x));
    PETSC_CHECK(PetscObjectSetName((PetscObject) this->x, "sln"));

    PETSC_CHECK(VecDuplicate(this->x, &this->b));
    PETSC_CHECK(PetscObjectSetName((PetscObject) this->b, "rhs"));

    PETSC_CHECK(DMCreateMatrix(dm, &this->A));
    PETSC_CHECK(PetscObjectSetName((PetscObject) this->A, "A"));
}

void
LinearProblem::set_up_callbacks()
{
    _F_;
    PETSC_CHECK(KSPSetComputeRHS(this->ksp, __compute_rhs, this));
    PETSC_CHECK(KSPSetComputeOperators(this->ksp, __compute_operators, this));
}

void
LinearProblem::set_up_monitors()
{
    _F_;
    PETSC_CHECK(KSPMonitorSet(this->ksp, __ksp_monitor_linear, this, nullptr));
}

void
LinearProblem::set_up_solver_parameters()
{
    _F_;
    PETSC_CHECK(KSPSetTolerances(this->ksp,
                                 this->lin_rel_tol,
                                 this->lin_abs_tol,
                                 PETSC_DEFAULT,
                                 this->lin_max_iter));
    PETSC_CHECK(KSPSetFromOptions(ksp));
}

PetscErrorCode
LinearProblem::ksp_monitor_callback(PetscInt it, PetscReal rnorm)
{
    _F_;
    lprintf(8, "%d Linear residual: %e", it, rnorm);
    return 0;
}

void
LinearProblem::solve()
{
    _F_;
    PETSC_CHECK(KSPSolve(this->ksp, this->b, this->x));
    PETSC_CHECK(KSPGetConvergedReason(this->ksp, &this->converged_reason));
}

bool
LinearProblem::converged()
{
    _F_;
    bool conv = this->converged_reason == KSP_CONVERGED_RTOL_NORMAL ||
                this->converged_reason == KSP_CONVERGED_ATOL_NORMAL ||
                this->converged_reason == KSP_CONVERGED_RTOL ||
                this->converged_reason == KSP_CONVERGED_ATOL ||
                this->converged_reason == KSP_CONVERGED_ITS ||
                this->converged_reason == KSP_CONVERGED_CG_NEG_CURVE ||
                this->converged_reason == KSP_CONVERGED_CG_CONSTRAINED ||
                this->converged_reason == KSP_CONVERGED_STEP_LENGTH ||
                this->converged_reason == KSP_CONVERGED_HAPPY_BREAKDOWN;
    return conv;
}

void
LinearProblem::run()
{
    _F_;
    solve();
    if (converged())
        on_final();
}

void
LinearProblem::set_up_matrix_properties()
{
}

void
LinearProblem::set_up_preconditioning()
{
}

} // namespace godzilla
