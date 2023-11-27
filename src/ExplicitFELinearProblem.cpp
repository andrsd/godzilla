#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/ExplicitFELinearProblem.h"
#include "godzilla/WeakForm.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/Output.h"
#include "godzilla/Validation.h"
#include "godzilla/Utils.h"
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
    set_default_output_on(ExecuteOn::INITIAL | ExecuteOn::TIMESTEP);
}

ExplicitFELinearProblem::~ExplicitFELinearProblem()
{
    _F_;
    this->M.destroy();
    this->M_lumped_inv.destroy();
    set_snes(nullptr);
}

Real
ExplicitFELinearProblem::get_time() const
{
    _F_;
    return TransientProblemInterface::get_time();
}

Int
ExplicitFELinearProblem::get_step_num() const
{
    _F_;
    return TransientProblemInterface::get_step_number();
}

const Matrix &
ExplicitFELinearProblem::get_mass_matrix() const
{
    return this->M;
}

Matrix &
ExplicitFELinearProblem::get_mass_matrix()
{
    return this->M;
}

const Vector &
ExplicitFELinearProblem::get_lumped_mass_matrix() const
{
    return this->M_lumped_inv;
}

const std::string &
ExplicitFELinearProblem::get_scheme() const
{
    _F_;
    return this->scheme;
}

void
ExplicitFELinearProblem::init()
{
    _F_;
    TransientProblemInterface::init();
    auto snes = TransientProblemInterface::get_snes();
    NonlinearProblem::set_snes(snes);
    FEProblemInterface::init();
    // so that the call to DMTSCreateRHSMassMatrix would form the mass matrix
    for (Int i = 0; i < get_num_fields(); i++)
        add_jacobian_block(i, i, new G0Identity(this), nullptr, nullptr, nullptr);

    auto ds = get_ds();
    for (auto & f : get_fields()) {
        Int fid = f.second.id;
        PETSC_CHECK(PetscDSSetImplicit(ds, fid, PETSC_FALSE));
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
    lprint(9, "Solving");
    TransientProblemInterface::solve(get_solution_vector());
}

void
ExplicitFELinearProblem::build_local_solution_vector(Vector & loc_sln)
{
    _F_;
    PETSC_CHECK(DMGlobalToLocal(get_dm(), get_solution_vector(), INSERT_VALUES, loc_sln));
    compute_boundary_local(get_time(), loc_sln);
}

void
ExplicitFELinearProblem::set_up_callbacks()
{
    _F_;
    PETSC_CHECK(DMTSSetRHSFunction(get_dm(), __efelp_compute_rhs, this));
}

void
ExplicitFELinearProblem::set_up_time_scheme()
{
    _F_;
    std::string name = utils::to_lower(this->scheme);
    std::map<std::string, TimeScheme> scheme_map = { { "euler", TimeScheme::EULER },
                                                     { "ssp-rk-2", TimeScheme::SSP_RK_2 },
                                                     { "ssp-rk-3", TimeScheme::SSP_RK_3 },
                                                     { "rk-2", TimeScheme::RK_2 },
                                                     { "heun", TimeScheme::HEUN } };
    set_scheme(scheme_map[name]);
}

void
ExplicitFELinearProblem::set_up_monitors()
{
    _F_;
    FENonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

void
ExplicitFELinearProblem::allocate_mass_matrix()
{
    _F_;
    this->M = create_matrix();
    set_ksp_operators(this->M, this->M);
}

void
ExplicitFELinearProblem::allocate_lumped_mass_matrix()
{
    _F_;
    this->M_lumped_inv = create_global_vector();
}

void
ExplicitFELinearProblem::create_mass_matrix()
{
    _F_;
    Mat m;
    PETSC_CHECK(DMCreateMassMatrix(get_dm(), get_dm(), &m));
    this->M = Matrix(m);
    set_ksp_operators(this->M, this->M);
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
    Vector loc_X = get_local_vector();
    Vector loc_F = get_local_vector();
    loc_X.zero();
    compute_boundary_local(time, loc_X);
    PETSC_CHECK(DMGlobalToLocal(get_dm(), X, INSERT_VALUES, loc_X));
    loc_F.zero();
    compute_rhs_local(time, loc_X, loc_F);
    F.zero();
    PETSC_CHECK(DMLocalToGlobal(get_dm(), loc_F, ADD_VALUES, F));
    if ((Vec) this->M_lumped_inv == nullptr) {
        auto ksp = get_ksp();
        PETSC_CHECK(KSPSolve(ksp, F, F));
    }
    else
        Vector::pointwise_mult(F, this->M_lumped_inv, F);
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
ExplicitFELinearProblem::post_step()
{
    _F_;
    TransientProblemInterface::post_step();
    update_aux_vector();
    compute_postprocessors();
    output(ExecuteOn::TIMESTEP);
}

void
ExplicitFELinearProblem::add_residual_block(Int field_id,
                                            ResidualFunc * f0,
                                            ResidualFunc * f1,
                                            const std::string & region)
{
    _F_;
    // see PetscDSSetRHSResidual for explanation
    Int part = 100;

    if (region.empty()) {
        add_weak_form_residual_block(PETSC_WF_F0, field_id, f0, Label(), 0, part);
        add_weak_form_residual_block(PETSC_WF_F1, field_id, f1, Label(), 0, part);
    }
    else {
        auto label = get_mesh()->get_label(region);
        auto ids = label.get_values();
        for (auto & val : ids) {
            add_weak_form_residual_block(PETSC_WF_F0, field_id, f0, label, val, part);
            add_weak_form_residual_block(PETSC_WF_F1, field_id, f1, label, val, part);
        }
    }
}

} // namespace godzilla
