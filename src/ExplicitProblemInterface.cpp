// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ExplicitProblemInterface.h"
#include "petscdmplex.h"

namespace godzilla {

Parameters
ExplicitProblemInterface::parameters()
{
    auto params = TransientProblemInterface::parameters();
    return params;
}

ExplicitProblemInterface::ExplicitProblemInterface(NonlinearProblem & problem,
                                                   const Parameters & pars) :
    TransientProblemInterface(problem, pars),
    nl_problem_(problem)
{
}

const Matrix &
ExplicitProblemInterface::get_mass_matrix() const
{
    CALL_STACK_MSG();
    return this->M_;
}

Matrix &
ExplicitProblemInterface::get_mass_matrix()
{
    CALL_STACK_MSG();
    return this->M_;
}

const Vector &
ExplicitProblemInterface::get_lumped_mass_matrix() const
{
    CALL_STACK_MSG();
    return this->M_lumped_inv_;
}

Vector &
ExplicitProblemInterface::get_lumped_mass_matrix()
{
    CALL_STACK_MSG();
    return this->M_lumped_inv_;
}

void
ExplicitProblemInterface::set_up_callbacks()
{
    CALL_STACK_MSG();
    TransientProblemInterface::set_up_callbacks();
    set_rhs_function(ref(*this), &ExplicitProblemInterface::compute_rhs_function);
}

void
ExplicitProblemInterface::allocate_mass_matrix()
{
    CALL_STACK_MSG();
    this->M_ = this->nl_problem_->create_matrix();
}

void
ExplicitProblemInterface::allocate_lumped_mass_matrix()
{
    CALL_STACK_MSG();
    this->M_lumped_inv_ = this->nl_problem_->create_global_vector();
}

void
ExplicitProblemInterface::create_mass_matrix()
{
    CALL_STACK_MSG();
    auto dm = this->nl_problem_->get_dm();
    PETSC_CHECK(DMCreateMassMatrix(dm, dm, this->M_));
    this->nl_problem_->set_ksp_operators(this->M_, this->M_);
}

void
ExplicitProblemInterface::create_mass_matrix_lumped()
{
    CALL_STACK_MSG();
    auto dm = this->nl_problem_->get_dm();
#if PETSC_VERSION_GE(3, 22, 0)
    PETSC_CHECK(DMCreateMassMatrixLumped(dm, NULL, this->M_lumped_inv_));
#else
    PETSC_CHECK(DMCreateMassMatrixLumped(dm, this->M_lumped_inv));
#endif
    this->M_lumped_inv_.reciprocal();
}

void
ExplicitProblemInterface::compute_rhs_function(Real time, const Vector & x, Vector & F)
{
    CALL_STACK_MSG();
    auto loc_x = this->nl_problem_->get_local_vector();
    auto loc_F = this->nl_problem_->get_local_vector();
    loc_x.zero();
    compute_boundary_local(time, loc_x);
    this->nl_problem_->global_to_local(x, INSERT_VALUES, loc_x);
    loc_F.zero();
    compute_rhs_local(time, loc_x, loc_F);
    F.zero();
    this->nl_problem_->local_to_global(loc_F, ADD_VALUES, F);
    if ((Vec) this->M_lumped_inv_ == nullptr) {
        auto ksp = this->nl_problem_->get_ksp();
        ksp.solve(F);
    }
    else
        pointwise_mult(F, this->M_lumped_inv_, F);
    this->nl_problem_->restore_local_vector(loc_x);
    this->nl_problem_->restore_local_vector(loc_F);
}

void
ExplicitProblemInterface::compute_rhs_local(Real /* time */,
                                            const Vector & /* x */,
                                            Vector & /* F */)
{
}

void
ExplicitProblemInterface::compute_boundary_local(Real time, Vector & x)
{
    CALL_STACK_MSG();
    auto dm = this->nl_problem_->get_dm();
    PETSC_CHECK(DMPlexTSComputeBoundary(dm, time, x, nullptr, this));
}

} // namespace godzilla
