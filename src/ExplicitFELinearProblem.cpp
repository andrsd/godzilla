#include "Godzilla.h"
#include "CallStack.h"
#include "ExplicitFELinearProblem.h"
#include "WeakForm.h"
#include "ResidualFunc.h"
#include "JacobianFunc.h"
#include "Output.h"
#include "Validation.h"
#include "Utils.h"
#include "petscts.h"

namespace godzilla {

static PetscErrorCode
__efelp_compute_rhs(TS, PetscReal time, Vec X, Vec F, void * ctx)
{
    _F_;
    auto prob = static_cast<ExplicitFELinearProblem *>(ctx);
    Vector x(X);
    Vector f(F);
    return prob->compute_rhs(time, x, f);
}

namespace {

class G0Identity : public JacobianFunc {
public:
    explicit G0Identity(ExplicitFELinearProblem * prob) :
        JacobianFunc(prob),
        n_comp(prob->get_field_num_components(0))
    {
    }

    void
    evaluate(Scalar * g) const override
    {
        for (Int c = 0; c < n_comp; ++c)
            g[c * n_comp + c] = 1.0;
    }

protected:
    Int n_comp;
};

} // namespace

Parameters
ExplicitFELinearProblem::parameters()
{
    Parameters params = FENonlinearProblem::parameters();
    params += TransientProblemInterface::parameters();
    params.add_param<std::string>("scheme",
                                  "euler",
                                  "Time stepping scheme: [euler, ssp-rk-2, ssp-rk-3, rk-2, heun]");
    return params;
}

ExplicitFELinearProblem::ExplicitFELinearProblem(const Parameters & params) :
    FENonlinearProblem(params),
    TransientProblemInterface(this, params),
    scheme(get_param<std::string>("scheme"))
{
    _F_;
    this->default_output_on = Output::ON_INITIAL | Output::ON_TIMESTEP;
}

ExplicitFELinearProblem::~ExplicitFELinearProblem()
{
    _F_;
    this->M.destroy();
    this->M_lumped_inv.destroy();
    this->snes = nullptr;
    this->ksp = nullptr;
}

Real
ExplicitFELinearProblem::get_time() const
{
    _F_;
    return this->time;
}

Int
ExplicitFELinearProblem::get_step_num() const
{
    _F_;
    return this->step_num;
}

void
ExplicitFELinearProblem::init()
{
    _F_;
    TransientProblemInterface::init();
    PETSC_CHECK(TSGetSNES(this->ts, &this->snes));
    PETSC_CHECK(SNESGetKSP(this->snes, &this->ksp));

    FEProblemInterface::init();
    // so that the call to DMTSCreateRHSMassMatrix would form the mass matrix
    for (Int i = 0; i < get_num_fields(); i++)
        set_jacobian_block(i, i, new G0Identity(this), nullptr, nullptr, nullptr);

    for (auto & f : this->fields) {
        Int fid = f.second.id;
        PETSC_CHECK(PetscDSSetImplicit(this->ds, fid, PETSC_FALSE));
    }
}

void
ExplicitFELinearProblem::create()
{
    _F_;
    FENonlinearProblem::create();
    TransientProblemInterface::create();
}

void
ExplicitFELinearProblem::check()
{
    _F_;
    FENonlinearProblem::check();
    TransientProblemInterface::check(this);

    if (!validation::in(this->scheme, { "euler", "ssp-rk-2", "ssp-rk-3", "rk-2", "heun" }))
        log_error("The 'scheme' parameter can be either 'euler', 'ssp-rk-2', 'ssp-rk-3', 'rk-2' or "
                  "'heun'.");
}

bool
ExplicitFELinearProblem::converged()
{
    _F_;
    return TransientProblemInterface::converged();
}

void
ExplicitFELinearProblem::solve()
{
    _F_;
    lprintf(9, "Solving");
    TransientProblemInterface::solve(this->x);
}

const Vector &
ExplicitFELinearProblem::get_solution_vector_local()
{
    _F_;
    auto & loc_sln = this->sln;
    PETSC_CHECK(DMGlobalToLocal(get_dm(), get_solution_vector(), INSERT_VALUES, loc_sln));
    compute_boundary_local(get_time(), loc_sln);
    return loc_sln;
}

void
ExplicitFELinearProblem::set_up_callbacks()
{
    _F_;
    DM dm = get_dm();
    PETSC_CHECK(DMTSSetRHSFunction(dm, __efelp_compute_rhs, this));
}

void
ExplicitFELinearProblem::set_up_time_scheme()
{
    _F_;
    std::string sch = utils::to_lower(this->scheme);
    if (sch == "euler")
        PETSC_CHECK(TSSetType(this->ts, TSEULER));
    else if (sch == "ssp-rk-2") {
        PETSC_CHECK(TSSetType(this->ts, TSSSP));
        PETSC_CHECK(TSSSPSetType(this->ts, TSSSPRKS2));
    }
    else if (sch == "ssp-rk-3") {
        PETSC_CHECK(TSSetType(this->ts, TSSSP));
        PETSC_CHECK(TSSSPSetType(this->ts, TSSSPRKS3));
    }
    else if (sch == "rk-2") {
        PETSC_CHECK(TSSetType(this->ts, TSRK));
        PETSC_CHECK(TSRKSetType(this->ts, TSRK2B));
    }
    else if (sch == "heun") {
        PETSC_CHECK(TSSetType(this->ts, TSRK));
        PETSC_CHECK(TSRKSetType(this->ts, TSRK2A));
    }
}

void
ExplicitFELinearProblem::set_up_monitors()
{
    _F_;
    FENonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

void
ExplicitFELinearProblem::create_mass_matrix()
{
    _F_;
    DM dm = get_dm();
    Mat m;
    PETSC_CHECK(DMCreateMassMatrix(dm, dm, &m));
    this->M = Matrix(m);
    PETSC_CHECK(KSPSetOperators(this->ksp, this->M, this->M));
}

void
ExplicitFELinearProblem::create_mass_matrix_lumped()
{
    _F_;
    Vec v;
    PETSC_CHECK(DMCreateMassMatrixLumped(get_dm(), &v));
    this->M_lumped_inv = Vector(v);
    this->M_lumped_inv.reciprocal();
}

PetscErrorCode
ExplicitFELinearProblem::compute_rhs(Real time, const Vector & X, Vector & F)
{
    _F_;
    DM dm = get_dm();
    Vector loc_X = get_local_vector();
    Vector loc_F = get_local_vector();
    loc_X.zero();
    compute_boundary_local(time, loc_X);
    PETSC_CHECK(DMGlobalToLocal(dm, X, INSERT_VALUES, loc_X));
    loc_F.zero();
    compute_rhs_local(time, loc_X, loc_F);
    F.zero();
    PETSC_CHECK(DMLocalToGlobal(dm, loc_F, ADD_VALUES, F));
    if ((Vec) this->M_lumped_inv == nullptr)
        PETSC_CHECK(KSPSolve(this->ksp, F, F));
    else
        PETSC_CHECK(VecPointwiseMult(F, this->M_lumped_inv, F));
    restore_local_vector(loc_X);
    restore_local_vector(loc_F);
    return 0;
}

PetscErrorCode
ExplicitFELinearProblem::compute_boundary_local(Real time, Vector & x)
{
    _F_;
    return DMPlexTSComputeBoundary(get_dm(), time, x, nullptr, this);
}

PetscErrorCode
ExplicitFELinearProblem::compute_rhs_local(Real time, const Vector & x, Vector & F)
{
    _F_;
    return DMPlexTSComputeRHSFunctionFEM(get_dm(), time, x, F, this);
}

void
ExplicitFELinearProblem::set_residual_block(Int field_id,
                                            ResidualFunc * f0,
                                            ResidualFunc * f1,
                                            const Label & label,
                                            Int val)
{
    _F_;
    // see PetscDSSetRHSResidual for explanation
    Int part = 100;
    this->wf->add(PETSC_WF_F0, label, val, field_id, part, f0);
    this->wf->add(PETSC_WF_F1, label, val, field_id, part, f1);
}

} // namespace godzilla
