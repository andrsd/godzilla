#include "FENonlinearProblem.h"
#include "CallStack.h"
#include "Mesh.h"
#include "WeakForm.h"
#include "ResidualFunc.h"
#include "JacobianFunc.h"
#include "petscdm.h"

namespace godzilla {

namespace internal {

void
__dummy_jacobian_func(PetscInt,
                      PetscInt,
                      PetscInt,
                      const PetscInt[],
                      const PetscInt[],
                      const PetscScalar[],
                      const PetscScalar[],
                      const PetscScalar[],
                      const PetscInt[],
                      const PetscInt[],
                      const PetscScalar[],
                      const PetscScalar[],
                      const PetscScalar[],
                      PetscReal,
                      PetscReal,
                      const PetscReal[],
                      PetscInt,
                      const PetscScalar[],
                      PetscScalar[])
{
}

} // namespace internal

static PetscErrorCode
__fep_compute_residual(DM, Vec x, Vec F, void * user)
{
    _F_;
    auto * fep = static_cast<FENonlinearProblem *>(user);
    fep->compute_residual(x, F);
    return 0;
}

static PetscErrorCode
__fep_compute_jacobian(DM, Vec x, Mat J, Mat Jp, void * user)
{
    _F_;
    auto * fep = static_cast<FENonlinearProblem *>(user);
    fep->compute_jacobian(x, J, Jp);
    return 0;
}

Parameters
FENonlinearProblem::parameters()
{
    Parameters params = NonlinearProblem::parameters();
    return params;
}

FENonlinearProblem::FENonlinearProblem(const Parameters & parameters) :
    NonlinearProblem(parameters),
    FEProblemInterface(this, parameters)
{
    _F_;
}

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

void
FENonlinearProblem::allocate_objects()
{
    NonlinearProblem::allocate_objects();
    FEProblemInterface::allocate_objects();
}

PetscErrorCode
FENonlinearProblem::compute_residual(Vec x, Vec f)
{
    _F_;
    return DMPlexSNESComputeResidualFEM(get_dm(), x, f, this);
}

PetscErrorCode
FENonlinearProblem::compute_jacobian(Vec x, Mat J, Mat Jp)
{
    _F_;
    return DMPlexSNESComputeJacobianFEM(get_dm(), x, J, Jp, this);
}

void
FENonlinearProblem::set_residual_block(PetscInt field_id, ResidualFunc * f0, ResidualFunc * f1)
{
    _F_;
    this->wf->add(PETSC_WF_F0, nullptr, 0, field_id, 0, f0);
    this->wf->add(PETSC_WF_F1, nullptr, 0, field_id, 0, f1);
}

void
FENonlinearProblem::set_jacobian_block(PetscInt fid,
                                       PetscInt gid,
                                       JacobianFunc * g0,
                                       JacobianFunc * g1,
                                       JacobianFunc * g2,
                                       JacobianFunc * g3)
{
    _F_;
    this->wf->add(PETSC_WF_G0, nullptr, 0, fid, gid, 0, g0);
    this->wf->add(PETSC_WF_G1, nullptr, 0, fid, gid, 0, g1);
    this->wf->add(PETSC_WF_G2, nullptr, 0, fid, gid, 0, g2);
    this->wf->add(PETSC_WF_G3, nullptr, 0, fid, gid, 0, g3);

    // So that PETSc thinks we have a Jacobian
    PetscDSSetJacobian(this->ds,
                       fid,
                       gid,
                       g0 ? internal::__dummy_jacobian_func : nullptr,
                       g1 ? internal::__dummy_jacobian_func : nullptr,
                       g2 ? internal::__dummy_jacobian_func : nullptr,
                       g3 ? internal::__dummy_jacobian_func : nullptr);
}

void
FENonlinearProblem::on_initial()
{
    _F_;
    NonlinearProblem::on_initial();
    compute_aux_fields();
}

} // namespace godzilla
