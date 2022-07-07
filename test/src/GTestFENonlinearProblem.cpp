#include "GTestFENonlinearProblem.h"
#include "Godzilla.h"

REGISTER_OBJECT(GTestFENonlinearProblem);

static void
f0_u(PetscInt dim,
     PetscInt Nf,
     PetscInt NfAux,
     const PetscInt uOff[],
     const PetscInt uOff_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt aOff[],
     const PetscInt aOff_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt numConstants,
     const PetscScalar constants[],
     PetscScalar f0[])
{
    f0[0] = 2.0;
}

/* gradU[comp*dim+d] = {u_x, u_y} or {u_x, u_y, u_z} */
static void
f1_u(PetscInt dim,
     PetscInt Nf,
     PetscInt NfAux,
     const PetscInt uOff[],
     const PetscInt uOff_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt aOff[],
     const PetscInt aOff_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt numConstants,
     const PetscScalar constants[],
     PetscScalar f1[])
{
    PetscInt d;
    for (d = 0; d < dim; ++d)
        f1[d] = u_x[d];
}

/* < \nabla v, \nabla u + {\nabla u}^T >
   This just gives \nabla u, give the perdiagonal for the transpose */
static void
g3_uu(PetscInt dim,
      PetscInt Nf,
      PetscInt NfAux,
      const PetscInt uOff[],
      const PetscInt uOff_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt aOff[],
      const PetscInt aOff_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_tShift,
      const PetscReal x[],
      PetscInt numConstants,
      const PetscScalar constants[],
      PetscScalar g3[])
{
    PetscInt d;
    for (d = 0; d < dim; ++d)
        g3[d * dim + d] = 1.0;
}

GTestFENonlinearProblem::GTestFENonlinearProblem(const Parameters & params) :
    FENonlinearProblem(params),
    iu(0)
{
}

GTestFENonlinearProblem::~GTestFENonlinearProblem() {}

const std::vector<PetscReal> &
GTestFENonlinearProblem::getConstants()
{
    return this->consts;
}

void
GTestFENonlinearProblem::set_up_initial_guess()
{
    FENonlinearProblem::set_up_initial_guess();
}

void
GTestFENonlinearProblem::set_up_constants()
{
    FENonlinearProblem::set_up_constants();
}

PetscDS
GTestFENonlinearProblem::getDS()
{
    return this->ds;
}

void
GTestFENonlinearProblem::compute_postprocessors()
{
    FENonlinearProblem::compute_postprocessors();
}

void
GTestFENonlinearProblem::set_up_fields()
{
    PetscInt order = 1;
    add_fe(this->iu, "u", 1, order);
}

void
GTestFENonlinearProblem::set_up_weak_form()
{
    set_residual_block(this->iu, f0_u, f1_u);
    set_jacobian_block(this->iu, this->iu, NULL, NULL, NULL, g3_uu);
}
