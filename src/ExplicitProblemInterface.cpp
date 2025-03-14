// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ExplicitProblemInterface.h"
#include "godzilla/Problem.h"
#include "godzilla/Validation.h"
#include "godzilla/Utils.h"
#include "petscdmplex.h"

namespace godzilla {

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
    nl_problem(problem),
    scheme(params.get<std::string>("scheme"))
{
    if (!validation::in(this->scheme, { "euler", "ssp-rk-2", "ssp-rk-3", "rk-2", "heun" }))
        this->nl_problem->log_error("The 'scheme' parameter can be either 'euler', 'ssp-rk-2', "
                                    "'ssp-rk-3', 'rk-2' or 'heun'.");
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

Vector &
ExplicitProblemInterface::get_lumped_mass_matrix()
{
    CALL_STACK_MSG();
    return this->M_lumped_inv;
}

void
ExplicitProblemInterface::set_up_callbacks()
{
    CALL_STACK_MSG();
    TransientProblemInterface::set_up_callbacks();
    set_rhs_function(this, &ExplicitProblemInterface::compute_rhs_function);
}

void
ExplicitProblemInterface::set_up_time_scheme()
{
    CALL_STACK_MSG();
    std::string name = utils::to_lower(this->scheme);
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

void
ExplicitProblemInterface::compute_rhs_function(Real time, const Vector & x, Vector & F)
{
    CALL_STACK_MSG();
    auto dm = this->nl_problem->get_dm();
    Vector loc_x = this->nl_problem->get_local_vector();
    Vector loc_F = this->nl_problem->get_local_vector();
    loc_x.zero();
    compute_boundary_local(time, loc_x);
    this->nl_problem->global_to_local(x, INSERT_VALUES, loc_x);
    loc_F.zero();
    compute_rhs_local(time, loc_x, loc_F);
    F.zero();
    this->nl_problem->local_to_global(loc_F, ADD_VALUES, F);
    if ((Vec) this->M_lumped_inv == nullptr) {
        auto ksp = this->nl_problem->get_ksp();
        ksp.solve(F);
    }
    else
        Vector::pointwise_mult(F, this->M_lumped_inv, F);
    this->nl_problem->restore_local_vector(loc_x);
    this->nl_problem->restore_local_vector(loc_F);
}

void
ExplicitProblemInterface::compute_rhs_local(Real time, const Vector & x, Vector & F)
{
}

} // namespace godzilla
