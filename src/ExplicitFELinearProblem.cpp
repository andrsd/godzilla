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

namespace {

class G0Identity : public JacobianFunc {
public:
    explicit G0Identity(const ExplicitFELinearProblem * prob) :
        JacobianFunc(prob),
        n_comp(prob->get_field_num_components(0))
    {
    }

    void
    evaluate(PetscScalar * g) override
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
    params.add_param<std::string>("scheme", "euler", "Time stepping scheme: [euler, ssp, rk]");
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
    // we should be doing this, but DM is already gone when we try to do this
    // DM dm = get_dm();
    // DMTSDestroyRHSMassMatrix(dm);

    this->snes = nullptr;
    this->ksp = nullptr;
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
    set_jacobian_block(0, 0, new G0Identity(this), nullptr, nullptr, nullptr);

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

    if (!validation::in(this->scheme, { "euler", "ssp", "rk" }))
        log_error("The 'scheme' parameter can be either 'euler', 'ssp' or 'rk'.");
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

void
ExplicitFELinearProblem::set_up_callbacks()
{
    _F_;
    DM dm = get_dm();
    PETSC_CHECK(DMTSSetBoundaryLocal(dm, DMPlexTSComputeBoundary, this));
    PETSC_CHECK(DMTSSetRHSFunctionLocal(dm, DMPlexTSComputeRHSFunctionFEM, this));
    PETSC_CHECK(DMTSCreateRHSMassMatrix(dm));
}

void
ExplicitFELinearProblem::set_up_time_scheme()
{
    _F_;
    std::string sch = utils::to_lower(this->scheme);
    if (sch == "euler")
        PETSC_CHECK(TSSetType(this->ts, TSEULER));
    else if (sch == "ssp")
        PETSC_CHECK(TSSetType(this->ts, TSSSP));
    else if (sch == "rk")
        PETSC_CHECK(TSSetType(this->ts, TSRK));
}

void
ExplicitFELinearProblem::set_up_monitors()
{
    _F_;
    FENonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

void
ExplicitFELinearProblem::set_residual_block(Int field_id,
                                            ResidualFunc * f0,
                                            ResidualFunc * f1,
                                            DMLabel label,
                                            Int val)
{
    _F_;
    // see PetscDSSetRHSResidual for explanation
    Int part = 100;
    this->wf->add(PETSC_WF_F0, label, val, field_id, part, f0);
    this->wf->add(PETSC_WF_F1, label, val, field_id, part, f1);
}

} // namespace godzilla
