// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/NaturalRiemannBC.h"
#include "godzilla/CallStack.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Assert.h"
#include <numeric>

namespace godzilla {

ErrorCode
NaturalRiemannBC::invoke_delegate(Real time,
                                  const Real * c,
                                  const Real * n,
                                  const Scalar * xI,
                                  Scalar * xG,
                                  void * ctx)
{
    CALL_STACK_MSG();
    auto * bc = static_cast<NaturalRiemannBC *>(ctx);
    bc->evaluate(time, c, n, xI, xG);
    return 0;
}

Parameters
NaturalRiemannBC::parameters()
{
    auto params = BoundaryCondition::parameters();
    return params;
}

NaturalRiemannBC::NaturalRiemannBC(const Parameters & pars) :
    BoundaryCondition(pars),
    fid(FieldID::INVALID)
{
    CALL_STACK_MSG();
}

void
NaturalRiemannBC::create()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    expect_true(dpi != nullptr, "DiscreteProblemInterface is null");

    auto field_names = dpi->get_field_names();
    if (field_names.size() == 1)
        this->fid = dpi->get_field_id(field_names[0]);

    this->components = create_components();
}

FieldID
NaturalRiemannBC::get_field_id() const
{
    CALL_STACK_MSG();
    return this->fid;
}

const std::vector<Int> &
NaturalRiemannBC::get_components() const
{
    CALL_STACK_MSG();
    return this->components;
}

std::vector<Int>
NaturalRiemannBC::create_components()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    auto n_comps = dpi->get_field_num_components(this->fid);
    std::vector<Int> comps(n_comps);
    std::iota(comps.begin(), comps.end(), 0);
    return comps;
}

void
NaturalRiemannBC::set_up()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    auto mesh = dpi->get_mesh();
    for (auto & boundary : get_boundary()) {
        auto label = mesh->get_face_set_label(boundary);
        auto ids = label.get_values();
        dpi->add_boundary(DM_BC_NATURAL_RIEMANN,
                          get_name(),
                          label,
                          ids,
                          this->fid,
                          this->components,
                          reinterpret_cast<void (*)()>(invoke_delegate),
                          nullptr,
                          this);
    }
}

} // namespace godzilla
