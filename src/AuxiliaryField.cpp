// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/AuxiliaryField.h"
#include "godzilla/CallStack.h"
#include "godzilla/Exception.h"
#include "godzilla/Ref.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/DiscreteProblemInterface.h"

namespace godzilla {

Parameters
AuxiliaryField::parameters()
{
    auto params = Object::parameters();
    params.add_private_param<LateRef<DiscreteProblemInterface>>("_dpi")
        .add_param<String>("field", "", "Name of the field.")
        .add_param<String>("region", "", "Label name where this auxiliary field is defined.");
    return params;
}

AuxiliaryField::AuxiliaryField(const Parameters & pars) :
    Object(pars),
    PrintInterface(this),
    dpi_(pars.get<Ref<DiscreteProblemInterface>>("_dpi")),
    mesh_(this->dpi_->get_mesh()),
    field_(pars.get<String>("field")),
    fid_(FieldID::INVALID),
    region_(pars.get<String>("region")),
    block_id_(-1)
{
    CALL_STACK_MSG();
    if (this->field_.length() == 0)
        this->field_ = this->get_name();
}

Ref<UnstructuredMesh>
AuxiliaryField::get_mesh() const
{
    CALL_STACK_MSG();
    return this->mesh_;
}

Ref<Problem>
AuxiliaryField::get_problem() const
{
    CALL_STACK_MSG();
    return this->dpi_->get_problem();
}

Dimension
AuxiliaryField::get_dimension() const
{
    CALL_STACK_MSG();
    return get_problem()->get_dimension();
}

void
AuxiliaryField::create()
{
    CALL_STACK_MSG();
    if (this->region_.length() > 0) {
        expect_true(this->mesh_->has_label(this->region_),
                    fmt::format("Region '{}' does not exists. Typo?", this->region_));
        this->label_ = this->mesh_->get_label(this->region_);
        this->block_id_ = this->mesh_->get_cell_set_id(this->region_).value();
    }

    auto id = this->dpi_->get_aux_field_id(this->field_);
    expect_true(id.has_value(),
                fmt::format("Auxiliary field '{}' does not exist. Typo?", this->field_));
    this->fid_ = id.value();
}

String
AuxiliaryField::get_region() const
{
    CALL_STACK_MSG();
    return this->region_;
}

Int
AuxiliaryField::get_block_id() const
{
    CALL_STACK_MSG();
    return this->block_id_;
}

const Label &
AuxiliaryField::get_label() const
{
    CALL_STACK_MSG();
    return this->label_;
}

FieldID
AuxiliaryField::get_field_id() const
{
    CALL_STACK_MSG();
    return this->fid_;
}

String
AuxiliaryField::get_field() const
{
    return this->field_;
}

} // namespace godzilla
