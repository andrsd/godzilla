// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ExplicitProblemInterface.h"
#include "godzilla/Problem.h"
#include "godzilla/Validation.h"
#include "godzilla/Utils.h"

namespace godzilla {

namespace {
PetscErrorCode
compute_rhs(TS, Real time, Vec x, Vec F, void * ctx)
{
    CALL_STACK_MSG();
    auto * epi = static_cast<ExplicitProblemInterface *>(ctx);
    Vector vec_x(x);
    Vector vec_F(F);
    return epi->compute_rhs(time, vec_x, vec_F);
}

} // namespace

Parameters
ExplicitProblemInterface::parameters()
{
    auto params = TransientProblemInterface::parameters();
    params.add_param<std::string>("scheme",
                                  "euler",
                                  "Time stepping scheme: [euler, ssp-rk-2, ssp-rk-3, rk-2, heun]");
    return params;
}

ExplicitProblemInterface::ExplicitProblemInterface(NonlinearProblem * problem,
                                                   const Parameters & params) :
    TransientProblemInterface(problem, params),
    nl_problem(problem)
{
}

ExplicitProblemInterface::~ExplicitProblemInterface()
{
    this->M.destroy();
    this->M_lumped_inv.destroy();
}

const Matrix &
ExplicitProblemInterface::get_mass_matrix() const
{
    CALL_STACK_MSG();
    return this->M;
}

Matrix &
ExplicitProblemInterface::get_mass_matrix()
{
    CALL_STACK_MSG();
    return this->M;
}

const Vector &
ExplicitProblemInterface::get_lumped_mass_matrix() const
{
    CALL_STACK_MSG();
    return this->M_lumped_inv;
}

void
ExplicitProblemInterface::check()
{
    CALL_STACK_MSG();
    TransientProblemInterface::check();
    auto prob = this->nl_problem;
    if (!validation::in(get_scheme(), { "euler", "ssp-rk-2", "ssp-rk-3", "rk-2", "heun" }))
        prob->log_error("The 'scheme' parameter can be either 'euler', 'ssp-rk-2', 'ssp-rk-3', "
                        "'rk-2' or 'heun'.");
}

void
ExplicitProblemInterface::set_up_callbacks()
{
    CALL_STACK_MSG();
    auto dm = this->nl_problem->get_dm();
    PETSC_CHECK(DMTSSetRHSFunction(dm, godzilla::compute_rhs, this));
}

void
ExplicitProblemInterface::set_up_time_scheme()
{
    CALL_STACK_MSG();
    std::string name = utils::to_lower(get_scheme());
    std::map<std::string, TimeScheme> scheme_map = { { "euler", TimeScheme::EULER },
                                                     { "ssp-rk-2", TimeScheme::SSP_RK_2 },
                                                     { "ssp-rk-3", TimeScheme::SSP_RK_3 },
                                                     { "rk-2", TimeScheme::RK_2 },
                                                     { "heun", TimeScheme::HEUN } };
    set_scheme(scheme_map[name]);
}

void
ExplicitProblemInterface::allocate_mass_matrix()
{
    CALL_STACK_MSG();
    this->M = this->nl_problem->create_matrix();
}

void
ExplicitProblemInterface::allocate_lumped_mass_matrix()
{
    CALL_STACK_MSG();
    this->M_lumped_inv = this->nl_problem->create_global_vector();
}

void
ExplicitProblemInterface::create_mass_matrix()
{
    CALL_STACK_MSG();
    auto dm = this->nl_problem->get_dm();
    Mat m;
    PETSC_CHECK(DMCreateMassMatrix(dm, dm, &m));
    this->M = Matrix(m);
    this->nl_problem->set_ksp_operators(this->M, this->M);
}

void
ExplicitProblemInterface::create_mass_matrix_lumped()
{
    CALL_STACK_MSG();
    auto dm = this->nl_problem->get_dm();
    Vec v;
    PETSC_CHECK(DMCreateMassMatrixLumped(dm, &v));
    this->M_lumped_inv = Vector(v);
    this->M_lumped_inv.reciprocal();
}

PetscErrorCode
ExplicitProblemInterface::compute_rhs(Real time, const Vector & x, Vector & F)
{
    CALL_STACK_MSG();
    auto dm = this->nl_problem->get_dm();
    Vector loc_x = this->nl_problem->get_local_vector();
    Vector loc_F = this->nl_problem->get_local_vector();
    loc_x.zero();
    compute_boundary_local(time, loc_x);
    PETSC_CHECK(DMGlobalToLocal(dm, x, INSERT_VALUES, loc_x));
    loc_F.zero();
    compute_rhs_local(time, loc_x, loc_F);
    F.zero();
    PETSC_CHECK(DMLocalToGlobal(dm, loc_F, ADD_VALUES, F));
    if ((Vec) this->M_lumped_inv == nullptr) {
        auto ksp = this->nl_problem->get_ksp();
        PETSC_CHECK(KSPSolve(ksp, F, F));
    }
    else
        Vector::pointwise_mult(F, this->M_lumped_inv, F);
    this->nl_problem->restore_local_vector(loc_x);
    this->nl_problem->restore_local_vector(loc_F);
    return 0;
}

PetscErrorCode
ExplicitProblemInterface::compute_boundary_local(Real time, Vector & x)
{
    CALL_STACK_MSG();
    auto dm = this->nl_problem->get_dm();
    return DMPlexTSComputeBoundary(dm, time, x, nullptr, this);
}

} // namespace godzilla
