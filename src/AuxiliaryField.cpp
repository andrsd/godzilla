// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/AuxiliaryField.h"
#include "godzilla/CallStack.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/DiscreteProblemInterface.h"

namespace godzilla {

Parameters
AuxiliaryField::parameters()
{
    Parameters params = Object::parameters();
    params.add_private_param<DiscreteProblemInterface *>("_dpi", nullptr);
    params.add_param<std::string>("field", "", "Name of the field.");
    params.add_param<std::string>("region",
                                  "",
                                  "Label name where this auxiliary field is defined.");
    return params;
}

AuxiliaryField::AuxiliaryField(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    dpi(get_param<DiscreteProblemInterface *>("_dpi")),
    mesh(nullptr),
    field(get_param<std::string>("field")),
    region(get_param<std::string>("region")),
    block_id(-1)
{
    CALL_STACK_MSG();
    if (this->field.empty())
        this->field = this->get_name();
}

UnstructuredMesh *
AuxiliaryField::get_mesh() const
{
    CALL_STACK_MSG();
    return this->mesh;
}

Problem *
AuxiliaryField::get_problem() const
{
    CALL_STACK_MSG();
    return this->dpi->get_problem();
}

Int
AuxiliaryField::get_dimension() const
{
    CALL_STACK_MSG();
    return get_problem()->get_dimension();
}

void
AuxiliaryField::create()
{
    CALL_STACK_MSG();
    this->mesh = this->dpi->get_mesh();
    if (!this->region.empty()) {
        if (this->mesh->has_label(this->region)) {
            this->label = this->mesh->get_label(this->region);
            this->block_id = this->mesh->get_cell_set_id(this->region);
        }
        else
            log_error("Region '{}' does not exists. Typo?", this->region);
    }
}

const std::string &
AuxiliaryField::get_region() const
{
    CALL_STACK_MSG();
    return this->region;
}

Int
AuxiliaryField::get_block_id() const
{
    CALL_STACK_MSG();
    return this->block_id;
}

const Label &
AuxiliaryField::get_label() const
{
    CALL_STACK_MSG();
    return this->label;
}

FieldID
AuxiliaryField::get_field_id() const
{
    CALL_STACK_MSG();
    return this->dpi->get_aux_field_id(this->field);
}

const std::string &
AuxiliaryField::get_field() const
{
    return this->field;
}

} // namespace godzilla
