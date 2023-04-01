#include "AuxiliaryField.h"
#include "CallStack.h"
#include "UnstructuredMesh.h"
#include "FEProblemInterface.h"

namespace godzilla {

Parameters
AuxiliaryField::parameters()
{
    Parameters params = Object::parameters();
    params.add_private_param<const FEProblemInterface *>("_fepi", nullptr);
    params.add_param<std::string>("field", "", "Name of the field.");
    params.add_param<std::string>("region",
                                  "",
                                  "Label name where this auxiliary field is defined.");
    return params;
}

AuxiliaryField::AuxiliaryField(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    fepi(get_param<FEProblemInterface *>("_fepi")),
    field(get_param<std::string>("field")),
    region(get_param<std::string>("region")),
    fid(-1),
    label(nullptr),
    block_id(-1)
{
}

void
AuxiliaryField::create()
{
    _F_;
    if (this->region.length() > 0) {
        const UnstructuredMesh * mesh = this->fepi->get_mesh();
        if (mesh->has_label(this->region)) {
            this->label = mesh->get_label(this->region);
            // this->block_id = mesh->get_cell_set_id(this->region);
            this->block_id = -9999;
        }
        //        if (mesh->has_cell_set(this->region)) {
        //            this->label = mesh->get_cell_set_label(this->region);
        //            this->block_id = mesh->get_cell_set_id(this->region);
        //        }
        //        else if (mesh->has_face_set(this->region)) {
        //            this->label = mesh->get_face_set_label(this->region);
        //            this->block_id = mesh->get_face_set_id(this->region);
        //        }
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

DMLabel
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
        return this->fepi->get_aux_field_id(this->field);
    else
        return this->fepi->get_aux_field_id(this->get_name());
}

void *
AuxiliaryField::get_context()
{
    _F_;
    return this;
}

} // namespace godzilla
