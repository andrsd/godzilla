// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/NaturalBC.h"
#include "godzilla/CallStack.h"
#include "godzilla/Assert.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/BndResidualFunc.h"
#include "godzilla/BndJacobianFunc.h"
#include "godzilla/Types.h"

namespace godzilla {

Parameters
NaturalBC::parameters()
{
    auto params = BoundaryCondition::parameters();
    params.add_param<std::string>("field", "Field name");
    return params;
}

NaturalBC::NaturalBC(const Parameters & pars) :
    BoundaryCondition(pars),
    fid(FieldID::INVALID),
    field_name(pars.get<Optional<std::string>>("field")),
    fepi(dynamic_cast<FEProblemInterface *>(get_discrete_problem_interface()))
{
    CALL_STACK_MSG();
}

void
NaturalBC::create()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    assert_true(dpi != nullptr, "DiscreteProblemInterface is null");

    auto field_names = dpi->get_field_names();
    if (field_names.size() == 1) {
        this->fid = dpi->get_field_id(field_names[0]);
    }
    else if (field_names.size() > 1) {
        if (this->field_name.has_value()) {
            if (dpi->has_field_by_name(this->field_name.value()))
                this->fid = dpi->get_field_id(this->field_name.value());
            else
                log_error("Field '{}' does not exists. Typo?", field_name.value());
        }
        else
            log_error("Use the 'field' parameter to assign this boundary condition to an existing "
                      "field.");
    }
}

FieldID
NaturalBC::get_field_id() const
{
    CALL_STACK_MSG();
    return this->fid;
}

void
NaturalBC::set_up()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    for (auto & bnd : get_boundary()) {
        if (get_field_id() != FieldID::INVALID)
            dpi->add_boundary_natural(get_name(), bnd, get_field_id(), get_components());
    }
}

void
NaturalBC::add_residual_block(BndResidualFunc * f0, BndResidualFunc * f1)
{
    CALL_STACK_MSG();
    for (auto & bnd : get_boundary())
        this->fepi->add_boundary_residual_block(this->fid, f0, f1, bnd);
}

void
NaturalBC::add_jacobian_block(FieldID gid,
                              BndJacobianFunc * g0,
                              BndJacobianFunc * g1,
                              BndJacobianFunc * g2,
                              BndJacobianFunc * g3)
{
    CALL_STACK_MSG();
    for (auto & bnd : get_boundary())
        this->fepi->add_boundary_jacobian_block(this->fid, gid, g0, g1, g2, g3, bnd);
}

} // namespace godzilla
