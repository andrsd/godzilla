#include "FENonlinearProblem.h"
#include "CallStack.h"
#include "Mesh.h"
#include "petscdm.h"

namespace godzilla {

PetscErrorCode
__fep_compute_residual(DM dm, Vec x, Vec F, void * user)
{
    _F_;
    FENonlinearProblem * fep = static_cast<FENonlinearProblem *>(user);
    fep->compute_residual_callback(x, F);
    return 0;
}

PetscErrorCode
__fep_compute_jacobian(DM dm, Vec x, Mat J, Mat Jp, void * user)
{
    _F_;
    FENonlinearProblem * fep = static_cast<FENonlinearProblem *>(user);
    fep->compute_jacobian_callback(x, J, Jp);
    return 0;
}

InputParameters
FENonlinearProblem::valid_params()
{
    InputParameters params = NonlinearProblem::valid_params();
    return params;
}

FENonlinearProblem::FENonlinearProblem(const InputParameters & parameters) :
    NonlinearProblem(parameters),
    FEProblemInterface(this, parameters)
{
    _F_;
}

FENonlinearProblem::~FENonlinearProblem() {}

void
FENonlinearProblem::create()
{
    _F_;
    FEProblemInterface::create();
    NonlinearProblem::create();
}

void
FENonlinearProblem::init()
{
    _F_;
    NonlinearProblem::init();
    FEProblemInterface::init();
}

void
FENonlinearProblem::set_up_callbacks()
{
    _F_;
    DM dm = this->get_dm();
    PETSC_CHECK(DMPlexSetSNESLocalFEM(dm, this, this, this));
    PETSC_CHECK(DMSNESSetFunctionLocal(dm, __fep_compute_residual, this));
    PETSC_CHECK(DMSNESSetJacobianLocal(dm, __fep_compute_jacobian, this));
    PETSC_CHECK(SNESSetJacobian(this->snes, this->J, this->J, nullptr, nullptr));
}

void
FENonlinearProblem::set_up_initial_guess()
{
    _F_;
    lprintf(9, "Setting initial guess");
    FEProblemInterface::set_up_initial_guess();
}

PetscErrorCode
FENonlinearProblem::compute_residual_callback(Vec x, Vec f)
{
    _F_;
    DM dm = this->get_dm();
    return DMPlexSNESComputeResidualFEM(dm, x, f, this);
}

PetscErrorCode
FENonlinearProblem::compute_jacobian_callback(Vec x, Mat J, Mat Jp)
{
    _F_;
    DM dm = this->get_dm();
    return DMPlexSNESComputeJacobianFEM(dm, x, J, Jp, this);
}

void
FENonlinearProblem::set_residual_block(PetscInt field_id,
                                       PetscFEResidualFunc * f0,
                                       PetscFEResidualFunc * f1)
{
    _F_;
    PETSC_CHECK(PetscDSSetResidual(this->ds, field_id, f0, f1));
}

void
FENonlinearProblem::set_jacobian_block(PetscInt fid,
                                       PetscInt gid,
                                       PetscFEJacobianFunc * g0,
                                       PetscFEJacobianFunc * g1,
                                       PetscFEJacobianFunc * g2,
                                       PetscFEJacobianFunc * g3)
{
    _F_;
    PETSC_CHECK(PetscDSSetJacobian(this->ds, fid, gid, g0, g1, g2, g3));
}

void
FENonlinearProblem::on_initial()
{
    _F_;
    NonlinearProblem::on_initial();
    compute_aux_fields();
}

} // namespace godzilla
