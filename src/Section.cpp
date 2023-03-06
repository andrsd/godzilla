#include "Section.h"
#include "CallStack.h"
#include "Error.h"

namespace godzilla {

Section::Section() : section(nullptr) {}

Section::Section(PetscSection s) : section(s) {}

void
Section::create(MPI_Comm comm)
{
    _F_;
    PETSC_CHECK(PetscSectionCreate(comm, &this->section));
}

void
Section::destroy()
{
    _F_;
    PETSC_CHECK(PetscSectionDestroy(&this->section));
    this->section = nullptr;
}

void
Section::reset()
{
    _F_;
    PETSC_CHECK(PetscSectionReset(this->section));
}

void
Section::view(PetscViewer viewer) const
{
    _F_;
    PETSC_CHECK(PetscSectionView(this->section, viewer));
}

void
Section::set_chart(Int start, Int end)
{
    _F_;
    PETSC_CHECK(PetscSectionSetChart(this->section, start, end));
}

void
Section::get_chart(Int & start, Int & end) const
{
    _F_;
    PETSC_CHECK(PetscSectionGetChart(this->section, &start, &end));
}

void
Section::set_up()
{
    _F_;
    PETSC_CHECK(PetscSectionSetUp(this->section));
}

void
Section::set_point_major(bool pm)
{
    _F_;
    PETSC_CHECK(PetscSectionSetPointMajor(this->section, pm ? PETSC_TRUE : PETSC_FALSE));
}

bool
Section::get_point_major() const
{
    _F_;
    PetscBool pm;
    PETSC_CHECK(PetscSectionGetPointMajor(this->section, &pm));
    return pm == PETSC_TRUE;
}

Int
Section::get_max_dof() const
{
    _F_;
    Int max_dof;
    PETSC_CHECK(PetscSectionGetMaxDof(this->section, &max_dof));
    return max_dof;
}

void
Section::set_num_fields(Int n_fields)
{
    _F_;
    PETSC_CHECK(PetscSectionSetNumFields(this->section, n_fields));
}

Int
Section::get_num_fields() const
{
    _F_;
    Int n_fields;
    PETSC_CHECK(PetscSectionGetNumFields(this->section, &n_fields));
    return n_fields;
}

void
Section::add_dof(Int point, Int n_dofs)
{
    _F_;
    PETSC_CHECK(PetscSectionAddDof(this->section, point, n_dofs));
}

void
Section::set_dof(Int point, Int n_dofs)
{
    _F_;
    PETSC_CHECK(PetscSectionSetDof(this->section, point, n_dofs));
}

Int
Section::get_dof(Int point) const
{
    _F_;
    Int n_dof;
    PETSC_CHECK(PetscSectionGetDof(this->section, point, &n_dof));
    return n_dof;
}

Section
Section::get_field(Int field) const
{
    _F_;
    PetscSection s;
    PETSC_CHECK(PetscSectionGetField(this->section, field, &s));
    return { s };
}

void
Section::set_field_components(Int field, Int n_comps)
{
    _F_;
    PETSC_CHECK(PetscSectionSetFieldComponents(this->section, field, n_comps));
}

Int
Section::get_field_components(Int field) const
{
    _F_;
    Int n_comps;
    PETSC_CHECK(PetscSectionGetFieldComponents(this->section, field, &n_comps));
    return n_comps;
}

void
Section::add_field_constraint_dof(Int point, Int field, Int n_dofs)
{
    _F_;
    PETSC_CHECK(PetscSectionAddFieldConstraintDof(this->section, point, field, n_dofs));
}

void
Section::set_field_constraint_dof(Int point, Int field, Int n_dofs)
{
    _F_;
    PETSC_CHECK(PetscSectionSetFieldConstraintDof(this->section, point, field, n_dofs));
}

Int
Section::get_field_constraint_dof(Int point, Int field) const
{
    _F_;
    Int n_dofs;
    PETSC_CHECK(PetscSectionGetFieldConstraintDof(this->section, point, field, &n_dofs));
    return n_dofs;
}

void
Section::add_field_dof(Int point, Int field, Int n_dofs)
{
    _F_;
    PETSC_CHECK(PetscSectionAddFieldDof(this->section, point, field, n_dofs));
}

void
Section::set_field_dof(Int point, Int field, Int n_dofs)
{
    _F_;
    PETSC_CHECK(PetscSectionSetFieldDof(this->section, point, field, n_dofs));
}

Int
Section::get_field_dof(Int point, Int field) const
{
    _F_;
    Int n_dofs;
    PETSC_CHECK(PetscSectionGetFieldDof(this->section, point, field, &n_dofs));
    return n_dofs;
}

void
Section::set_field_name(Int field, const std::string & name)
{
    _F_;
    PETSC_CHECK(PetscSectionSetFieldName(this->section, field, name.c_str()));
}

std::string
Section::get_field_name(Int field) const
{
    _F_;
    const char * name;
    PETSC_CHECK(PetscSectionGetFieldName(this->section, field, &name));
    return { name };
}

Int
Section::get_offset(Int point) const
{
    _F_;
    Int offset;
    PETSC_CHECK(PetscSectionGetOffset(this->section, point, &offset));
    return offset;
}

void
Section::get_offset_range(Int & start, Int & end) const
{
    _F_;
    PETSC_CHECK(PetscSectionGetOffsetRange(this->section, &start, &end));
}

Int
Section::get_field_offset(Int point, Int field) const
{
    _F_;
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->section, point, field, &offset));
    return offset;
}

Int
Section::get_field_point_offset(Int point, Int field) const
{
    _F_;
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldPointOffset(this->section, point, field, &offset));
    return offset;
}

Int
Section::get_storage_size() const
{
    _F_;
    Int sz;
    PetscSectionGetStorageSize(this->section, &sz);
    return sz;
}

bool
Section::has_constraints() const
{
    _F_;
    PetscBool hc;
    PETSC_CHECK(PetscSectionHasConstraints(this->section, &hc));
    return hc == PETSC_TRUE;
}

void
Section::set_component_name(Int field, Int comp, const std::string & name)
{
    _F_;
    PETSC_CHECK(PetscSectionSetComponentName(this->section, field, comp, name.c_str()));
}

std::string
Section::get_component_name(Int field, Int comp) const
{
    _F_;
    const char * name;
    PETSC_CHECK(PetscSectionGetComponentName(this->section, field, comp, &name));
    return { name };
}

void
Section::add_constraint_dof(Int point, Int n_dofs)
{
    _F_;
    PETSC_CHECK(PetscSectionAddConstraintDof(this->section, point, n_dofs));
}

void
Section::set_constraint_dof(Int point, Int n_dofs)
{
    _F_;
    PETSC_CHECK(PetscSectionSetConstraintDof(this->section, point, n_dofs));
}

Int
Section::get_constraint_dof(Int point) const
{
    _F_;
    Int n_dofs;
    PETSC_CHECK(PetscSectionGetConstraintDof(this->section, point, &n_dofs));
    return n_dofs;
}

Section
Section::create(DM dm,
                DMLabel label[],
                const Int n_comp[],
                const Int n_dof[],
                Int n_bc,
                const Int bc_field[],
                const IS bc_comps[],
                const IS bc_points[],
                IS perm)
{
    PetscSection s;
    PETSC_CHECK(DMPlexCreateSection(dm,
                                    label,
                                    n_comp,
                                    n_dof,
                                    n_bc,
                                    bc_field,
                                    bc_comps,
                                    bc_points,
                                    perm,
                                    &s));
    return { s };
}

} // namespace godzilla
