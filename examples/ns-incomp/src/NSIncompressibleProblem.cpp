#include "Godzilla.h"
#include "FunctionInterface.h"
#include "NSIncompressibleProblem.h"
#include "CallStack.h"
#include <assert.h>
#include "petscsys.h"

using namespace godzilla;

registerObject(NSIncompressibleProblem);

static void
f0_veloc(PetscInt dim,
         PetscInt nf,
         PetscInt nf_aux,
         const PetscInt u_off[],
         const PetscInt u_off_x[],
         const PetscScalar u[],
         const PetscScalar u_t[],
         const PetscScalar u_x[],
         const PetscInt a_off[],
         const PetscInt a_off_x[],
         const PetscScalar a[],
         const PetscScalar a_t[],
         const PetscScalar a_x[],
         PetscReal t,
         const PetscReal x[],
         PetscInt num_constants,
         const PetscScalar constants[],
         PetscScalar f0[])
{
    const PetscReal * vel = u + u_off[NSIncompressibleProblem::velocity_id];
    const PetscReal * vel_t = u_t + u_off[NSIncompressibleProblem::velocity_id];
    const PetscReal * vel_x = u_x + u_off_x[NSIncompressibleProblem::velocity_id];
    const PetscInt n_comp = dim;

    for (PetscInt c = 0; c < n_comp; ++c) {
        f0[c] = vel_t[c];

        for (PetscInt d = 0; d < dim; ++d)
            f0[c] += vel[d] * vel_x[c * dim + d];

        f0[c] -= a[c];
    }
}

static void
f1_veloc(PetscInt dim,
         PetscInt nf,
         PetscInt nf_aux,
         const PetscInt u_off[],
         const PetscInt u_off_x[],
         const PetscScalar u[],
         const PetscScalar u_t[],
         const PetscScalar u_x[],
         const PetscInt a_off[],
         const PetscInt a_off_x[],
         const PetscScalar a[],
         const PetscScalar a_t[],
         const PetscScalar a_x[],
         PetscReal t,
         const PetscReal x[],
         PetscInt num_constants,
         const PetscScalar constants[],
         PetscScalar f1[])
{
    const PetscReal * vel_x = u_x + u_off_x[NSIncompressibleProblem::velocity_id];
    const PetscReal * press = u + u_off[NSIncompressibleProblem::pressure_id];
    const PetscReal Re = constants[NSIncompressibleProblem::Re_idx];
    const PetscInt n_comp = dim;

    for (PetscInt comp = 0; comp < n_comp; ++comp) {
        for (PetscInt d = 0; d < dim; ++d) {
            f1[comp * dim + d] = 1.0 / Re * vel_x[comp * dim + d];
        }
        f1[comp * dim + comp] -= press[0];
    }
}

static void
f0_press(PetscInt dim,
         PetscInt nf,
         PetscInt nf_aux,
         const PetscInt u_off[],
         const PetscInt u_off_x[],
         const PetscScalar u[],
         const PetscScalar u_t[],
         const PetscScalar u_x[],
         const PetscInt a_off[],
         const PetscInt a_off_x[],
         const PetscScalar a[],
         const PetscScalar a_t[],
         const PetscScalar a_x[],
         PetscReal t,
         const PetscReal x[],
         PetscInt num_constants,
         const PetscScalar constants[],
         PetscScalar f0[])
{
    const PetscReal * vel_x = u_x + u_off_x[NSIncompressibleProblem::velocity_id];

    f0[0] = 0.0;
    for (PetscInt d = 0; d < dim; ++d)
        f0[0] += vel_x[d * dim + d];
}

static void
f1_press(PetscInt dim,
         PetscInt nf,
         PetscInt nf_aux,
         const PetscInt u_off[],
         const PetscInt u_off_x[],
         const PetscScalar u[],
         const PetscScalar u_t[],
         const PetscScalar u_x[],
         const PetscInt a_off[],
         const PetscInt a_off_x[],
         const PetscScalar a[],
         const PetscScalar a_t[],
         const PetscScalar a_x[],
         PetscReal t,
         const PetscReal x[],
         PetscInt num_constants,
         const PetscScalar constants[],
         PetscScalar f1[])
{
    for (PetscInt d = 0; d < dim; ++d)
        f1[d] = 0.0;
}

static void
g0_vv(PetscInt dim,
      PetscInt nf,
      PetscInt nf_aux,
      const PetscInt u_off[],
      const PetscInt u_off_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt a_off[],
      const PetscInt a_off_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_t_shift,
      const PetscReal x[],
      PetscInt num_constants,
      const PetscScalar constants[],
      PetscScalar g0[])
{
    const PetscReal * vel_x = u_x + u_off_x[NSIncompressibleProblem::velocity_id];
    PetscInt nc_i = dim;
    PetscInt nc_j = dim;

    for (PetscInt d = 0; d < dim; ++d) {
        g0[d * dim + d] = u_t_shift;
    }

    for (PetscInt fc = 0; fc < nc_i; ++fc) {
        for (PetscInt gc = 0; gc < nc_j; ++gc) {
            g0[fc * nc_j + gc] += vel_x[fc * nc_j + gc];
        }
    }
}

static void
g1_vv(PetscInt dim,
      PetscInt nf,
      PetscInt nf_aux,
      const PetscInt u_off[],
      const PetscInt u_off_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt a_off[],
      const PetscInt a_off_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_t_shift,
      const PetscReal x[],
      PetscInt num_constants,
      const PetscScalar constants[],
      PetscScalar g1[])
{
    const PetscReal * vel = u + u_off[NSIncompressibleProblem::velocity_id];
    PetscInt nc_i = dim;
    PetscInt nc_j = dim;

    for (PetscInt fc = 0; fc < nc_i; ++fc) {
        for (PetscInt gc = 0; gc < nc_j; ++gc) {
            for (PetscInt dg = 0; dg < dim; ++dg) {
                // kronecker delta
                if (fc == gc) {
                    g1[(fc * nc_j + gc) * dim + dg] += vel[dg];
                }
            }
        }
    }
}

static void
g1_pv(PetscInt dim,
      PetscInt nf,
      PetscInt nf_aux,
      const PetscInt u_off[],
      const PetscInt u_off_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt a_off[],
      const PetscInt a_off_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_t_shift,
      const PetscReal x[],
      PetscInt num_constants,
      const PetscScalar constants[],
      PetscScalar g1[])
{
    for (PetscInt d = 0; d < dim; ++d)
        g1[d * dim + d] = 1.0;
}

static void
g2_vp(PetscInt dim,
      PetscInt nf,
      PetscInt nf_aux,
      const PetscInt u_off[],
      const PetscInt u_off_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt a_off[],
      const PetscInt a_off_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_t_shift,
      const PetscReal x[],
      PetscInt num_constants,
      const PetscScalar constants[],
      PetscScalar g2[])
{
    for (PetscInt d = 0; d < dim; ++d)
        g2[d * dim + d] = -1.0;
}

static void
g3_vv(PetscInt dim,
      PetscInt nf,
      PetscInt nf_aux,
      const PetscInt u_off[],
      const PetscInt u_off_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt a_off[],
      const PetscInt a_off_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_t_shift,
      const PetscReal x[],
      PetscInt num_constants,
      const PetscScalar constants[],
      PetscScalar g3[])
{
    const PetscReal Re = constants[NSIncompressibleProblem::Re_idx];
    const PetscInt n_comp = dim;

    for (PetscInt comp_i = 0; comp_i < n_comp; ++comp_i) {
        for (PetscInt d = 0; d < dim; ++d) {
            g3[((comp_i * n_comp + comp_i) * dim + d) * dim + d] = 1.0 / Re;
        }
    }
}

InputParameters
NSIncompressibleProblem::valid_params()
{
    InputParameters params = ImplicitFENonlinearProblem::valid_params();
    params.add_required_param<PetscReal>("Re", "Reynolds number");
    return params;
}

NSIncompressibleProblem::NSIncompressibleProblem(const InputParameters & parameters) :
    ImplicitFENonlinearProblem(parameters),
    Re(get_param<PetscReal>("Re"))
{
    _F_;
}

NSIncompressibleProblem::~NSIncompressibleProblem() {}

void
NSIncompressibleProblem::set_up_fields()
{
    _F_;
    PetscInt dim = this->get_dimension();
    add_fe(velocity_id, "velocity", dim, 2);
    add_fe(pressure_id, "pressure", 1, 1);

    add_aux_fe(ffn_aid, "ffn", dim, 2);
}

void
NSIncompressibleProblem::set_up_weak_form()
{
    _F_;
    set_residual_block(velocity_id, f0_veloc, f1_veloc);
    set_residual_block(pressure_id, f0_press, f1_press);

    set_jacobian_block(velocity_id, velocity_id, g0_vv, g1_vv, nullptr, g3_vv);
    set_jacobian_block(velocity_id, pressure_id, nullptr, nullptr, g2_vp, nullptr);
    set_jacobian_block(pressure_id, velocity_id, nullptr, g1_pv, nullptr, nullptr);

    std::vector<PetscReal> consts = { this->Re };
    set_constants(consts);
}

void
NSIncompressibleProblem::set_up_matrix_properties()
{
    _F_;
}

void
NSIncompressibleProblem::set_up_field_null_space(DM dm)
{
    _F_;
    // PetscObject pressure;
    // MatNullSpace nsp;
    // PETSC_CHECK(DMGetField(dm, pressure_id, nullptr, &pressure));
    // PETSC_CHECK(MatNullSpaceCreate(PetscObjectComm(pressure), PETSC_TRUE, 0, nullptr, &nsp));
    // PETSC_CHECK(PetscObjectCompose(pressure, "nullspace", (PetscObject) nsp));
    // PETSC_CHECK(MatNullSpaceDestroy(&nsp));
}

void
NSIncompressibleProblem::set_up_preconditioning()
{
    _F_;

    PC pc;
    PETSC_CHECK(KSPGetPC(this->ksp, &pc));

    PETSC_CHECK(PCSetType(pc, PCFIELDSPLIT));
    PETSC_CHECK(PCFieldSplitSetType(pc, PC_COMPOSITE_SCHUR));
    PETSC_CHECK(PCFieldSplitSetSchurFactType(pc, PC_FIELDSPLIT_SCHUR_FACT_FULL));
    PetscInt n_fields;
    char ** field_names;
    IS * is;
    PETSC_CHECK(DMCreateFieldIS(get_dm(), &n_fields, &field_names, &is));

    // attach null space to the pressure field
    MatNullSpace nsp;
    PETSC_CHECK(MatNullSpaceCreate(get_comm(), PETSC_TRUE, 0, nullptr, &nsp));
    PETSC_CHECK(PetscObjectCompose((PetscObject) is[1], "nullspace", (PetscObject) nsp));
    PETSC_CHECK(MatNullSpaceDestroy(&nsp));

    for (PetscInt i = 0; i < n_fields; i++)
        PETSC_CHECK(PCFieldSplitSetIS(pc, field_names[i], is[i]));
    for (PetscInt i = 0; i < n_fields; i++) {
        PetscFree(field_names[i]);
        PETSC_CHECK(ISDestroy(is + i));
    }
    PetscFree(field_names);
    PetscFree(is);
    PETSC_CHECK(PCSetOperators(pc, this->J, this->J));
    PETSC_CHECK(PCSetUp(pc));

    PetscInt n;
    KSP * sub_ksp;
    PETSC_CHECK(PCFieldSplitGetSubKSP(pc, &n, &sub_ksp));
    assert(n == 2);

    PC pc_vel;
    PETSC_CHECK(KSPGetPC(sub_ksp[velocity_id], &pc_vel));
    PETSC_CHECK(PCSetType(pc_vel, PCLU));

    PETSC_CHECK(KSPSetTolerances(sub_ksp[pressure_id],
                                 1.0e-10,
                                 PETSC_DEFAULT,
                                 PETSC_DEFAULT,
                                 PETSC_DEFAULT));
    PC pc_press;
    PETSC_CHECK(KSPGetPC(sub_ksp[pressure_id], &pc_press));
    PETSC_CHECK(PCSetType(pc_press, PCJACOBI));

    PetscFree(sub_ksp);
}
