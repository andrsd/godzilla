// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/RZSymmetry.h"
#include "godzilla/CallStack.h"
#include "godzilla/DiscreteProblemInterface.h"

namespace godzilla {

Parameters
RZSymmetry::parameters()
{
    auto params = Object::parameters();
    params.add_private_param<DiscreteProblemInterface *>("_dpi")
        .add_required_param<std::vector<Real>>("axis", "Axis vector")
        .add_param<std::vector<Real>>("point",
                                      std::vector<Real>({ 0, 0 }),
                                      "Axis point. Default value is the origin.");
    return params;
}

RZSymmetry::RZSymmetry(const Parameters & pars) :
    Object(pars),
    dpi(pars.get<DiscreteProblemInterface *>("_dpi")),
    axis(pars.get<std::vector<Real>>("axis")),
    pt(pars.get<std::vector<Real>>("point"))
{
    CALL_STACK_MSG();
}

void
RZSymmetry::create()
{
    CALL_STACK_MSG();
    Object::create();
    auto problem = this->dpi->get_problem();
    auto dim = problem->get_dimension();
    expect_true(dim == 2_D, "'RZSymmetry' can be used only with 2D problems.");
    expect_true(this->axis.size() == 2, "'axis' parameter must provide 2 components.");
    expect_true(this->pt.size() == 2, "'point' parameter must provide 2 components.");
}

Real
RZSymmetry::get_value(Real time, const DenseVector<Real, 2> & x)
{
    CALL_STACK_MSG();

    std::vector<Real> a({ x(0) - this->pt[0], x(1) - this->pt[1] });

    constexpr Int dim = 2;
    Real a_sqr = 0.;
    for (Int i = 0; i < dim; ++i)
        a_sqr += a[i] * a[i];

    Real dot = 0.;
    for (Int i = 0; i < dim; ++i)
        dot += a[i] * this->axis[i];

    Real b_sqr = 0.;
    for (Int i = 0; i < dim; ++i)
        b_sqr += this->axis[i] * this->axis[i];

    return std::sqrt(a_sqr - dot * dot / b_sqr);
}

} // namespace godzilla
