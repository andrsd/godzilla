#include "FELinearProblem.h"
#include "CallStack.h"
#include "Mesh.h"
#include "Utils.h"
#include "AssemblyList.h"
#include "H1Space.h"
#include "H1LobattoShapesetEdge.h"
#include "BoundaryCondition.h"
#include "ShapeFunction1D.h"
#include "Solution1D.h"
#include "RefMap1D.h"
#include "QuadratureGauss1D.h"
#include "Forms.h"
#include "PetscVector.h"
#include "PetscMatrix.h"
#include "DenseMatrix.h"
#include "Set.h"

namespace godzilla {

PetscErrorCode
__compute_rhs(KSP ksp, Vec b, void * ctx)
{
    _F_;
    FELinearProblem * problem = static_cast<FELinearProblem *>(ctx);
    return problem->compute_rhs_callback(b);
}

PetscErrorCode
__compute_operators(KSP ksp, Mat A, Mat B, void * ctx)
{
    _F_;
    FELinearProblem * problem = static_cast<FELinearProblem *>(ctx);
    return problem->compute_operators_callback(A, B);
}

PetscErrorCode
__ksp_monitor_linear(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx)
{
    _F_;
    FELinearProblem * problem = static_cast<FELinearProblem *>(ctx);
    return problem->ksp_monitor_callback(it, rnorm);
}

InputParameters
FELinearProblem::valid_params()
{
    InputParameters params = Problem::valid_params();
    params.add_param<Real>("lin_rel_tol",
                           1e-5,
                           "Relative convergence tolerance for the linear solver");
    params.add_param<Real>("lin_abs_tol",
                           1e-50,
                           "Absolute convergence tolerance for the linear solver");
    params.add_param<PetscInt>("lin_max_iter",
                               10000,
                               "Maximum number of iterations for the linear solver");
    return params;
}

FELinearProblem::FELinearProblem(const InputParameters & parameters) :
    Problem(parameters),
    mesh(get_param<Mesh *>("_mesh")),
    shapeset(nullptr),
    n_dofs(0),
    dof_stride(1),
    refmap(nullptr),
    al(nullptr),
    base_fn(nullptr),
    test_fn(nullptr),
    jxw(nullptr),
    np(0),
    u(nullptr),
    grad_u(nullptr),
    v(nullptr),
    grad_v(nullptr),
    ksp(NULL),
    x(NULL),
    b(NULL),
    A(NULL),
    B(NULL),
    lin_rel_tol(get_param<PetscReal>("lin_rel_tol")),
    lin_abs_tol(get_param<PetscReal>("lin_abs_tol")),
    lin_max_iter(get_param<PetscInt>("lin_max_iter"))
{
    _F_;
}

FELinearProblem::~FELinearProblem()
{
    _F_;
    if (this->ksp)
        KSPDestroy(&this->ksp);
    if (this->b)
        VecDestroy(&this->b);
    if (this->x)
        VecDestroy(&this->x);
    if ((this->A != this->B) && (this->B))
        MatDestroy(&this->B);
    if (this->A)
        MatDestroy(&this->A);
    delete this->refmap;
    delete[] this->al;

    for (uint i = 0; i < this->neq; i++) {
        delete this->base_fn[i];
        delete this->test_fn[i];
    }
    delete[] this->base_fn;
    delete[] this->test_fn;
}

DM
FELinearProblem::get_dm() const
{
    _F_;
    return this->mesh->get_dm();
}

const Mesh *
FELinearProblem::get_mesh() const
{
    _F_;
    return this->mesh;
}

Vec
FELinearProblem::get_solution_vector() const
{
    _F_;
    return this->x;
}

Solution1D *
FELinearProblem::get_solution(const std::string & name) const
{
    _F_;
    const auto & it = this->space_names.find(name);
    if (it != this->space_names.end())
        return this->solutions[it->second];
    else
        error("Variable with name '", name, "' does not exist in the problem.");
}

void
FELinearProblem::create()
{
    _F_;
    init();
    setup_solver_parameters();
    setup_monitors();
    // setup_callbacks();

    on_set_fields();

    // FIXME: pull this from a weak form
    this->neq = 1;
    this->al = new AssemblyList[this->neq];
    MEM_CHECK(this->al);
    this->base_fn = new ShapeFunction1D *[this->neq];
    MEM_CHECK(this->base_fn);
    this->test_fn = new ShapeFunction1D *[this->neq];
    MEM_CHECK(this->test_fn);
    for (uint i = 0; i < this->neq; i++) {
        const Shapeset1D * ss = dynamic_cast<const Shapeset1D *>(this->spaces[i]->get_shapeset());
        assert(ss != nullptr);
        this->base_fn[i] = new ShapeFunction1D(ss);
        MEM_CHECK(this->base_fn[i]);
        this->test_fn[i] = new ShapeFunction1D(ss);
        MEM_CHECK(this->test_fn[i]);
    }
    this->refmap = new RefMap1D(this->mesh);
    MEM_CHECK(refmap);

    for (auto & bc : this->bcs) {
        bc->create();
        // FIXME: get the index of space the BC is applied to
        uint space_idx = 0;
        this->spaces[space_idx]->add_boundary_condition(bc);
    }

    assign_dofs();
    update_constraints();

    allocate_objects();
    on_set_matrix_properties();

    for (uint i = 0; i < this->spaces.size(); i++) {
        Space * spc = this->spaces[i];
        Solution1D * sln = this->solutions[i];
        sln->set_fe_solution(spc, get_solution_vector());
    }
}

void
FELinearProblem::init()
{
    _F_;
    PetscErrorCode ierr;
    ierr = KSPCreate(comm(), &this->ksp);
    checkPetscError(ierr);
}

void
FELinearProblem::allocate_objects()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = get_dm();

    ierr = VecCreateMPI(comm(), PETSC_DECIDE, this->n_dofs, &this->x);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->x, "sln");
    checkPetscError(ierr);

    ierr = VecDuplicate(this->x, &this->b);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->b, "rhs");
    checkPetscError(ierr);

    PetscInt * nnz = pre_alloc();
    ierr = MatCreateAIJ(comm(),
                        PETSC_DECIDE,
                        PETSC_DECIDE,
                        this->n_dofs,
                        this->n_dofs,
                        0,
                        nnz,
                        0,
                        nullptr,
                        &this->A);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->A, "A");
    checkPetscError(ierr);
    delete[] nnz;

    // TODO: Add API for setting up preconditioners
    this->B = this->A;

    // FIXME: this would conflict with the case where users would want to use callbacks
    ierr = KSPSetOperators(ksp, this->A, this->B);
    checkPetscError(ierr);
}

void
FELinearProblem::setup_callbacks()
{
    _F_;
    PetscErrorCode ierr;

    DM dm = get_dm();
    ierr = KSPSetDM(this->ksp, dm);
    checkPetscError(ierr);
    ierr = DMSetApplicationContext(dm, this);
    checkPetscError(ierr);
    ierr = KSPSetComputeRHS(this->ksp, __compute_rhs, this);
    checkPetscError(ierr);
    ierr = KSPSetComputeOperators(this->ksp, __compute_operators, this);
    checkPetscError(ierr);
}

void
FELinearProblem::setup_monitors()
{
    _F_;
    PetscErrorCode ierr;
    ierr = KSPMonitorSet(this->ksp, __ksp_monitor_linear, this, 0);
    checkPetscError(ierr);
}

void
FELinearProblem::setup_solver_parameters()
{
    _F_;
    PetscErrorCode ierr;
    ierr = KSPSetTolerances(this->ksp,
                            this->lin_rel_tol,
                            this->lin_abs_tol,
                            PETSC_DEFAULT,
                            this->lin_max_iter);
    checkPetscError(ierr);
    ierr = KSPSetFromOptions(ksp);
    checkPetscError(ierr);
}

PetscErrorCode
FELinearProblem::compute_rhs_callback(Vec b)
{
    _F_;
    return 0;
}

PetscErrorCode
FELinearProblem::compute_operators_callback(Mat A, Mat B)
{
    _F_;
    return 0;
}

PetscErrorCode
FELinearProblem::ksp_monitor_callback(PetscInt it, PetscReal rnorm)
{
    _F_;
    godzilla_print(8, it, " Linear residual: ", std::scientific, rnorm);
    return 0;
}

void
FELinearProblem::solve()
{
    _F_;
    PetscMatrix A_mat(this->A);
    PetscVector b_vec(this->b);
    assemble(&A_mat, &b_vec);
    A_mat.finish();
    b_vec.finish();

    PetscErrorCode ierr;
    ierr = KSPSolve(this->ksp, this->b, this->x);
    checkPetscError(ierr);
    ierr = KSPGetConvergedReason(this->ksp, &this->converged_reason);
    checkPetscError(ierr);
}

bool
FELinearProblem::converged()
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
FELinearProblem::on_set_matrix_properties()
{
    _F_;
}

void
FELinearProblem::run()
{
    _F_;
    solve();
    output();
}

void
FELinearProblem::add_variable(const std::string & name, uint nc, uint p)
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

    Solution1D * sln = new Solution1D(this->mesh);
    this->solutions.push_back(sln);
}

void
FELinearProblem::add_boundary_condition(BoundaryCondition * bc)
{
    _F_;
    this->bcs.push_back(bc);
}

void
FELinearProblem::assign_dofs()
{
    _F_;

    this->n_dofs = 0;
    for (auto & sp : this->spaces) {
        sp->assign_dofs(this->n_dofs, this->dof_stride);
        this->n_dofs += sp->get_dof_count();
    }
    godzilla_print(9, "Number of DoFs: ", this->n_dofs);
}

void
FELinearProblem::update_constraints()
{
    _F_;
}

PetscInt *
FELinearProblem::pre_alloc()
{
    _F_;

    Array<Set> ms;
    for (Index i = 0; i < this->n_dofs; i++)
        ms[i] = Set();

    for (auto & e : this->mesh->get_elements()) {
        const Element1D * elem = dynamic_cast<const Element1D *>(e);
        assert(elem != nullptr);

        for (uint i = 0; i < this->neq; i++)
            this->spaces[i]->get_element_assembly_list(elem, &(this->al[i]));

        uint m = 0;
        uint n = 0;

        AssemblyList * am = this->al + m;
        AssemblyList * an = this->al + n;

        for (uint i = 0; i < am->cnt; i++)
            if (am->dof[i] != Space::DIRICHLET_DOF)
                for (uint j = 0; j < an->cnt; j++)
                    if (an->dof[j] != Space::DIRICHLET_DOF)
                        ms[am->dof[i]].set(an->dof[j]);
    }

    PetscInt * nnz = new PetscInt[this->n_dofs];
    MEM_CHECK(nnz);
    for (Index i = 0; i < this->n_dofs; i++)
        nnz[i] = ms[i].count();
    return nnz;
}

void
FELinearProblem::assemble(PetscMatrix * matrix, PetscVector * rhs)
{
    _F_;
    // these things will be sitting the WeakForm class
    BilinearForm laplace(this, 0, 0);
    LinearForm ffn(this, 0);

    for (auto & e : this->mesh->get_elements()) {
        const Element1D * elem = dynamic_cast<const Element1D *>(e);
        assert(elem != nullptr);

        for (uint j = 0; j < this->neq; j++) {
            this->spaces[j]->get_element_assembly_list(elem, &(this->al[j]));
            this->base_fn[j]->set_active_element(elem);
            this->test_fn[j]->set_active_element(elem);
        }
        this->refmap->set_active_element(elem);

        // FIXME: Determine quadrature order
        uint qorder = this->refmap->get_inv_ref_order() + 2;

        const Quadrature1D & quad = QuadratureGauss1D::get();
        this->np = quad.get_num_points(qorder);
        QPoint1D * pts = quad.get_points(qorder);
        this->jxw = this->refmap->get_jacobian(np, pts);
        Real1x1 * irm = this->refmap->get_inv_ref_map(np, pts);

        // pre-compute everything that will be needed by the weak form
        uint m = 0;
        uint n = 0;

        AssemblyList * am = this->al + m;
        AssemblyList * an = this->al + n;

        this->u = new Fn1D[an->cnt];
        this->grad_u = new Gradient1D[an->cnt];
        this->v = new Fn1D[am->cnt];
        this->grad_v = new Gradient1D[am->cnt];

        ShapeFunction1D * fv = this->test_fn[m];
        for (int i = 0; i < am->cnt; i++) {
            fv->set_active_shape(am->idx[i]);
            fv->precalculate(np, pts, ShapeFunction1D::FN_DEFAULT);
            this->v[i].set(np, fv);
            this->grad_v[i].set(np, fv, irm);
        }

        ShapeFunction1D * fu = this->base_fn[n];
        for (int j = 0; j < an->cnt; j++) {
            if (this->spaces[m]->get_shapeset() == this->spaces[n]->get_shapeset()) {
                this->u[j].set(np, this->v + j);
                this->grad_u[j].set(np, this->grad_v + j);
            }
            else {
                fu->set_active_shape(an->idx[j]);
                fu->precalculate(np, pts, ShapeFunction1D::FN_DEFAULT);
                this->u[j].set(np, fu);
                this->grad_u[j].set(np, fu, irm);
            }
        }

        // assemble bilinear form (volumetric)
        laplace.assemble(matrix, rhs);
        // assemble linear form (volumetric)
        ffn.assemble(rhs);

        delete[] this->u;
        delete[] this->grad_u;
        delete[] this->v;
        delete[] this->grad_v;

        delete[] this->jxw;
        delete[] irm;
    }
}

} // namespace godzilla
