#include "godzilla/LinearProblem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Mesh.h"
#include "godzilla/Output.h"

namespace godzilla {

PetscErrorCode
__compute_rhs(KSP, Vec b, void * ctx)
{
    _F_;
    auto * problem = static_cast<LinearProblem *>(ctx);
    Vector vec_b(b);
    return problem->compute_rhs(vec_b);
}

PetscErrorCode
__compute_operators(KSP, Mat A, Mat B, void * ctx)
{
    _F_;
    auto * problem = static_cast<LinearProblem *>(ctx);
    Matrix mat_A(A);
    Matrix mat_B(B);
    return problem->compute_operators(mat_A, mat_B);
}

PetscErrorCode
__ksp_monitor_linear(KSP, Int it, Real rnorm, void * ctx)
{
    _F_;
    auto * problem = static_cast<LinearProblem *>(ctx);
    problem->ksp_monitor_callback(it, rnorm);
    return 0;
}

Parameters
LinearProblem::parameters()
{
    Parameters params = Problem::parameters();
    params.add_param<Real>("lin_rel_tol",
                           1e-5,
                           "Relative convergence tolerance for the linear solver");
    params.add_param<Real>("lin_abs_tol",
                           1e-50,
                           "Absolute convergence tolerance for the linear solver");
    params.add_param<Int>("lin_max_iter",
                          10000,
                          "Maximum number of iterations for the linear solver");
    return params;
}

LinearProblem::LinearProblem(const Parameters & parameters) :
    Problem(parameters),
    ksp(nullptr),
    converged_reason(KSP_CONVERGED_ITERATING),
    lin_rel_tol(get_param<Real>("lin_rel_tol")),
    lin_abs_tol(get_param<Real>("lin_abs_tol")),
    lin_max_iter(get_param<Int>("lin_max_iter"))
{
    _F_;
    set_default_output_on(ExecuteOn::FINAL);
}

LinearProblem::~LinearProblem()
{
    _F_;
    if (this->ksp)
        KSPDestroy(&this->ksp);
    this->b.destroy();
}

void
LinearProblem::create()
{
    _F_;
    get_mesh()->distribute();
    init();
    allocate_objects();
    set_up_matrix_properties();
    set_up_preconditioning();

    set_up_solver_parameters();
    set_up_monitors();
    set_up_callbacks();

    Problem::create();

    PETSC_CHECK(KSPSetFromOptions(this->ksp));
}

void
LinearProblem::init()
{
    _F_;
    PETSC_CHECK(KSPCreate(get_comm(), &this->ksp));
    PETSC_CHECK(KSPSetDM(this->ksp, get_dm()));
    PETSC_CHECK(DMSetApplicationContext(get_dm(), this));
}

void
LinearProblem::allocate_objects()
{
    _F_;
    Problem::allocate_objects();

    this->b = get_solution_vector().duplicate();
    this->b.set_name("rhs");
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
}

void
LinearProblem::ksp_monitor_callback(Int it, Real rnorm)
{
    _F_;
    lprint(8, "{} Linear residual: {:e}", it, rnorm);
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
#if PETSC_VERSION_GE(3, 19, 0)
                this->converged_reason == KSP_CONVERGED_NEG_CURVE ||
#else
                this->converged_reason == KSP_CONVERGED_CG_NEG_CURVE ||
                this->converged_reason == KSP_CONVERGED_CG_CONSTRAINED ||
#endif
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
    _F_;
}

void
LinearProblem::set_up_preconditioning()
{
    _F_;
}

void
LinearProblem::solve()
{
    _F_;
    PETSC_CHECK(KSPSolve(this->ksp, this->b, get_solution_vector()));
    PETSC_CHECK(KSPGetConvergedReason(this->ksp, &this->converged_reason));
}

PetscErrorCode
LinearProblem::compute_rhs(Vector &)
{
    _F_;
    return 0;
}

PetscErrorCode
LinearProblem::compute_operators(Matrix &, Matrix &)
{
    _F_;
    return 0;
}

} // namespace godzilla
