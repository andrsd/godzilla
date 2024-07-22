// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/BoundaryCondition.h"
#include "godzilla/Types.h"
#include "godzilla/WeakForm.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/BndResidualFunc.h"
#include "godzilla/BndJacobianFunc.h"

namespace godzilla {

class FEProblemInterface;
class AbstractBndResidualFunc;
class BndJacobianFunc;

/// Base class for natural boundary conditions
class NaturalBC : public BoundaryCondition {
public:
    explicit NaturalBC(const Parameters & params);

    void create() override;
    void set_up() override;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    [[nodiscard]] Int get_field_id() const;

    /// Get the component numbers this boundary condition is constraining
    ///
    /// @return Vector of component numbers
    [[nodiscard]] virtual const std::vector<Int> & get_components() const = 0;

    /// Set up the weak form for the boundary integral of this boundary condition
    virtual void set_up_weak_form() = 0;

protected:
    /// Set residual statement for the boundary integral
    ///
    /// @param res_func Integrand
    template <WeakForm::ResidualKind KIND>
    void
    add_residual_block(AbstractBndResidualFunc * res_func)
    {
        throw Exception("Unsupported boundary residual functional kind");
    }

    /// Set Jacobian statement for the boundary integral
    ///
    /// @param gid Field ID
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    template <WeakForm::JacobianKind KIND>
    void
    add_jacobian_block(Int gid, BndJacobianFunc * jac_func)
    {
        throw Exception("Unsupported boundary jacobian functional kind");
    }

private:
    /// Field ID this boundary condition is attached to
    Int fid;

    /// Finite element problem this object is part of
    FEProblemInterface * fepi;

public:
    static Parameters parameters();
};

/// Set residual statement for the boundary integral
///
/// @param res_func Integrand for the test function term
template <>
inline void
NaturalBC::add_residual_block<WeakForm::BND_F0>(AbstractBndResidualFunc * res_func)
{
    CALL_STACK_MSG();
    for (auto & bnd : get_boundary())
        this->fepi->add_residual_block<WeakForm::BND_F0>(this->fid, res_func, bnd);
}

/// Set residual statement for the boundary integral
///
/// @param res_func Integrand for the test function gradient term
template <>
inline void
NaturalBC::add_residual_block<WeakForm::BND_F1>(AbstractBndResidualFunc * res_func)
{
    CALL_STACK_MSG();
    for (auto & bnd : get_boundary())
        this->fepi->add_residual_block<WeakForm::BND_F1>(this->fid, res_func, bnd);
}

/// Set Jacobian statement for the boundary integral
///
/// @param gid Field ID
/// @param jac_func Integrand for the test and basis function term
template <>
inline void
NaturalBC::add_jacobian_block<WeakForm::BND_G0>(Int gid, BndJacobianFunc * jac_func)
{
    CALL_STACK_MSG();
    for (auto & bnd : get_boundary())
        this->fepi->add_jacobian_block<WeakForm::BND_G0>(this->fid, gid, jac_func, bnd);
}

/// Set Jacobian statement for the boundary integral
///
/// @param gid Field ID
/// @param jac_func Integrand for the test function and basis function gradient term
template <>
inline void
NaturalBC::add_jacobian_block<WeakForm::BND_G1>(Int gid, BndJacobianFunc * jac_func)
{
    CALL_STACK_MSG();
    for (auto & bnd : get_boundary())
        this->fepi->add_jacobian_block<WeakForm::BND_G1>(this->fid, gid, jac_func, bnd);
}

/// Set Jacobian statement for the boundary integral
///
/// @param gid Field ID
/// @param jac_func Integrand for the test function gradient and basis function term
template <>
inline void
NaturalBC::add_jacobian_block<WeakForm::BND_G2>(Int gid, BndJacobianFunc * jac_func)
{
    CALL_STACK_MSG();
    for (auto & bnd : get_boundary())
        this->fepi->add_jacobian_block<WeakForm::BND_G2>(this->fid, gid, jac_func, bnd);
}

/// Set Jacobian statement for the boundary integral
///
/// @param gid Field ID
/// @param jac_func Integrand for the test function gradient and basis function gradient term
template <>
inline void
NaturalBC::add_jacobian_block<WeakForm::BND_G3>(Int gid, BndJacobianFunc * jac_func)
{
    CALL_STACK_MSG();
    for (auto & bnd : get_boundary())
        this->fepi->add_jacobian_block<WeakForm::BND_G3>(this->fid, gid, jac_func, bnd);
}

} // namespace godzilla
