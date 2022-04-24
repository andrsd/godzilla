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

FELinearProblem::FELinearProblem(const InputParameters & parameters) :
    Problem(parameters),
    mesh(get_param<Mesh *>("_mesh")),
    shapeset(nullptr),
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
    uint neq = 1;
    AssemblyList al[neq];
    AssemblyList *am, *an;
    ShapeFunction1D * base_fn[neq];
    ShapeFunction1D * test_fn[neq];
    ShapeFunction1D *fu, *fv;
    // FIXME: there should be just one refmap (everything is on a single element)
    RefMap1D * refmap[neq];
    for (int i = 0; i < neq; i++) {
        const Shapeset1D * ss = dynamic_cast<const Shapeset1D *>(spaces[i]->get_shapeset());
        assert(ss != nullptr);
        base_fn[i] = new ShapeFunction1D(ss);
        MEM_CHECK(base_fn[i]);
        test_fn[i] = new ShapeFunction1D(ss);
        MEM_CHECK(test_fn[i]);
        refmap[i] = new RefMap1D(this->mesh);
        MEM_CHECK(refmap[i]);
    }

    for (auto & e : this->mesh->get_elements()) {
        const Element1D * elem = dynamic_cast<const Element1D *>(e);
        assert(elem != nullptr);

        uint j = 0;
        spaces[j]->get_element_assembly_list(elem, &(al[j]));
        base_fn[j]->set_active_element(elem);
        test_fn[j]->set_active_element(elem);
        refmap[j]->set_active_element(elem);

        // assemble bilinear form (volumetric)

        // FIXME: block row
        int m = 0;
        fv = test_fn[m];
        am = al + m;

        // FIXME: block column
        int n = 0;
        fu = base_fn[n];
        an = al + n;

        DenseMatrix<Scalar> mat(am->cnt, an->cnt);
        mat.zero();

        for (int i = 0; i < am->cnt; i++) {
            int k = am->dof[i];
            fv->set_active_shape(am->idx[i]);

            for (int j = 0; j < an->cnt; j++) {
                fu->set_active_shape(an->idx[j]);
                Scalar v = eval_bilin_form(fu, fv, refmap[0]);
                Scalar bi = v * an->coef[j] * am->coef[i];
                if (an->dof[j] == Space::DIRICHLET_DOF)
                    rhs->add(k, -bi);
                else
                    mat[i][j] = bi;
            }
        }
        // insert the local matrix into the global one
        matrix->add(mat, am->dof, an->dof);

        // assemble linear forms (volumetric)
        for (int i = 0; i < am->cnt; i++) {
            if (am->dof[i] == Space::DIRICHLET_DOF)
                continue;
            fv->set_active_shape(am->idx[i]);

            Scalar val = eval_lin_form(fv, refmap[0]);
            rhs->add(am->dof[i], val * am->coef[i]);
        }
    }

    for (uint i = 0; i < neq; i++) {
        delete base_fn[i];
        delete test_fn[i];
        delete refmap[i];
    }

    matrix->finish();
    rhs->finish();
}

SFn1D *
FELinearProblem::get_fn(ShapeFunction1D * shfn, RefMap1D * rm, const uint np, const QPoint1D * pts)
{
    _F_;
    SFn1D * u = new SFn1D;
    MEM_CHECK(u);
    u->nc = shfn->get_num_components();
    shfn->precalculate(np, pts, ShapeFunction1D::FN_DEFAULT);
    if (u->nc == 1) {
        u->fn = new PetscReal[np];
        MEM_CHECK(u->fn);
        u->dx = new PetscReal[np];
        MEM_CHECK(u->dx);

        Real * fn = shfn->get_fn_values();
        Real * dx = shfn->get_dx_values();
        Real1x1 * m = rm->get_inv_ref_map(np, pts);
        for (uint i = 0; i < np; i++) {
            u->fn[i] = fn[i];
            u->dx[i] = dx[i] * m[i][0][0];
        }
        delete[] m;
    }
    return u;
}

PetscScalar
FELinearProblem::eval_bilin_form(ShapeFunction1D * fu, ShapeFunction1D * fv, RefMap1D * rm)
{
    _F_;
    // TODO: pre-computing the stuff needed for bilin/lin form evaluation needs go outside of this
    // method

    uint order = rm->get_inv_ref_order() + 2;
    const Quadrature1D & quad = QuadratureGauss1D::get();
    uint np = quad.get_num_points(order);
    QPoint1D * pts = quad.get_points(order);
    Real * jxw = rm->get_jacobian(np, pts);

    SFn1D * u = get_fn(fu, rm, np, pts);
    SFn1D * v = get_fn(fv, rm, np, pts);

    PetscScalar res = 0.0;
    for (uint i = 0; i < np; i++)
        res += jxw[i] * (u->dx[i] * v->dx[i]);

    delete u;
    delete v;

    return res;
}

PetscScalar
FELinearProblem::eval_lin_form(ShapeFunction1D * fv, RefMap1D * rm)
{
    _F_;
    uint order = rm->get_inv_ref_order() + 2;
    const Quadrature1D & quad = QuadratureGauss1D::get();
    uint np = quad.get_num_points(order);
    QPoint1D * pts = quad.get_points(order);
    Real * jxw = rm->get_jacobian(np, pts);

    SFn1D * v = get_fn(fv, rm, np, pts);

    PetscScalar res = 0.0;
    for (uint i = 0; i < np; i++)
        res += jxw[i] * (-2. * v->fn[i]);

    delete v;

    return res;
}

} // namespace godzilla
