#include "FELinearProblem.h"
#include "CallStack.h"
#include "Mesh.h"
#include "Utils.h"
#include "AssemblyList.h"
#include "H1Space.h"
#include "H1LobattoShapesetEdge.h"
#include "ShapeFunction1D.h"
#include "RefMap1D.h"
#include "QuadratureGauss1D.h"
#include "Forms.h"
#include "PetscVector.h"
#include "PetscMatrix.h"
#include "DenseMatrix.h"

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
FELinearProblem::validParams()
{
    InputParameters params = Problem::validParams();
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
    section(nullptr),
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
    if (this->section)
        PetscSectionDestroy(&this->section);
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

Vec
FELinearProblem::get_solution_vector() const
{
    _F_;
    return this->x;
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

    set_bc_information();
    assign_dofs();
    update_constraints();

    allocate_objects();
    on_set_matrix_properties();
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

    ierr = DMCreateGlobalVector(dm, &this->x);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->x, "sln");
    checkPetscError(ierr);

    ierr = VecDuplicate(this->x, &this->b);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->b, "rhs");
    checkPetscError(ierr);

    ierr = DMCreateMatrix(dm, &this->A);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->A, "A");
    checkPetscError(ierr);

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
    /// FIXME: implement preallocation and remove this
    MatSetOption(this->A, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE);

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
}

void
FELinearProblem::set_bc_information()
{
    _F_;
    for (auto & bnd : this->mesh->get_side_boundaries()) {
        const uint & marker = bnd->marker;
        for (auto & sp : this->spaces) {
            // 1D
            const Element * e = this->mesh->get_element(bnd->elem_id);
            PetscInt vtx_id = this->mesh->get_vertex_id(e, bnd->side);
            sp->set_vertex_bc_info(vtx_id);
        }
    }
}

void
FELinearProblem::assign_dofs()
{
    _F_;

    // std::cerr << "n_elems = " << this->mesh->get_elements().count() << std::endl;
    // for (auto & elem : this->mesh->get_elements()) {
    //     // std::cerr << "idx = " << idx << std:::endl;
    //     std::cerr << "elem = " << elem->get_id() << std::endl;
    // }

    PetscErrorCode ierr;
    DM dm = this->mesh->get_dm();

    PetscInt p_start, p_end;
    ierr = DMPlexGetChart(dm, &p_start, &p_end);
    checkPetscError(ierr);

    ierr = PetscSectionCreate(comm(), &this->section);
    checkPetscError(ierr);

    ierr = PetscSectionSetNumFields(this->section, this->spaces.size());
    checkPetscError(ierr);

    // TODO: should be done by the space
    ierr = PetscSectionSetFieldComponents(this->section, 0, 1);
    checkPetscError(ierr);

    ierr = PetscSectionSetChart(this->section, p_start, p_end);
    checkPetscError(ierr);

    // std::cerr << "n_verts = " << this->mesh->get_vertices().count() << std::endl;
    for (auto & vtx : this->mesh->get_vertices()) {
        // std::cerr << "vtx = " << vtx->id << std::endl;
        PetscInt ndofs = 0;
        for (auto & sp : this->spaces) {
            ndofs += sp->assign_vertex_dofs(vtx->id);
        }
        ierr = PetscSectionSetDof(this->section, vtx->id, ndofs);
        checkPetscError(ierr);
    }

    ierr = PetscSectionSetUp(this->section);
    checkPetscError(ierr);

    ierr = DMSetLocalSection(dm, this->section);
    checkPetscError(ierr);

    ierr = DMSetAdjacency(dm, 0, PETSC_FALSE, PETSC_TRUE);
    checkPetscError(ierr);

    // godzilla_print(9, "Number of DoFs: ", ndofs);
}

void
FELinearProblem::update_constraints()
{
    _F_;
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
