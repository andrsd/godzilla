#include "AuxiliaryField.h"
#include "CallStack.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "DiscreteProblemInterface.h"

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
    mesh(nullptr),
    dpi(get_param<DiscreteProblemInterface *>("_dpi")),
    field(get_param<std::string>("field")),
    region(get_param<std::string>("region")),
    fid(-1),
    block_id(-1)
{
    _F_;
    this->mesh = dynamic_cast<UnstructuredMesh *>(get_problem()->get_mesh());
}

UnstructuredMesh *
AuxiliaryField::get_mesh() const
{
    _F_;
    return this->mesh;
}

Problem *
AuxiliaryField::get_problem() const
{
    _F_;
    return this->dpi->get_problem();
}

void
AuxiliaryField::create()
{
    _F_;
    if (this->region.length() > 0) {
        auto mesh = get_mesh();
        if (mesh->has_label(this->region)) {
            this->label = mesh->get_label(this->region);
            this->block_id = mesh->get_cell_set_id(this->region);
        }
        else
            log_error("Region '{}' does not exists. Typo?", this->region);
    }
}

const std::string &
AuxiliaryField::get_region() const
{
    _F_;
    return this->region;
}

Int
AuxiliaryField::get_block_id() const
{
    _F_;
    return this->block_id;
}

const Label &
AuxiliaryField::get_label() const
{
    _F_;
    return this->label;
}

Int
AuxiliaryField::get_field_id() const
{
    _F_;
    if (this->field.length() > 0)
        return this->dpi->get_aux_field_id(this->field);
    else
        return this->dpi->get_aux_field_id(this->get_name());
}

void *
AuxiliaryField::get_context()
{
    _F_;
    return this;
}

} // namespace godzilla
