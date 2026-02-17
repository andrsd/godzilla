// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Section.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include "godzilla/Range.h"
#include "petscdmplex.h"

namespace godzilla {

Section::Section() : PetscObjectWrapper(nullptr) {}

Section::Section(PetscSection s) : PetscObjectWrapper(s) {}

void
Section::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionCreate(comm, &this->obj));
}

void
Section::destroy()
{
    CALL_STACK_MSG();
    // PETSC_CHECK(PetscSectionDestroy(&this->obj));
    // this->obj = nullptr;
}

void
Section::reset()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionReset(this->obj));
}

void
Section::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionView(this->obj, viewer));
}

void
Section::set_chart(Int start, Int end)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetChart(this->obj, start, end));
}

void
Section::set_chart(const Range & range)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetChart(this->obj, range.first(), range.last()));
}

void
Section::get_chart(Int & start, Int & end) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionGetChart(this->obj, &start, &end));
}

Range
Section::get_chart() const
{
    CALL_STACK_MSG();
    Int start, end;
    PETSC_CHECK(PetscSectionGetChart(this->obj, &start, &end));
    return { start, end };
}

void
Section::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetUp(this->obj));
}

void
Section::set_point_major(bool pm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetPointMajor(this->obj, pm ? PETSC_TRUE : PETSC_FALSE));
}

bool
Section::get_point_major() const
{
    CALL_STACK_MSG();
    PetscBool pm;
    PETSC_CHECK(PetscSectionGetPointMajor(this->obj, &pm));
    return pm == PETSC_TRUE;
}

Int
Section::get_max_dof() const
{
    CALL_STACK_MSG();
    Int max_dof;
    PETSC_CHECK(PetscSectionGetMaxDof(this->obj, &max_dof));
    return max_dof;
}

void
Section::set_num_fields(Int n_fields)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetNumFields(this->obj, n_fields));
}

Int
Section::get_num_fields() const
{
    CALL_STACK_MSG();
    Int n_fields;
    PETSC_CHECK(PetscSectionGetNumFields(this->obj, &n_fields));
    return n_fields;
}

void
Section::add_dof(Int point, Int n_dofs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionAddDof(this->obj, point, n_dofs));
}

void
Section::set_dof(Int point, Int n_dofs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetDof(this->obj, point, n_dofs));
}

Int
Section::get_dof(Int point) const
{
    CALL_STACK_MSG();
    Int n_dof;
    PETSC_CHECK(PetscSectionGetDof(this->obj, point, &n_dof));
    return n_dof;
}

Section
Section::get_field(Int field) const
{
    CALL_STACK_MSG();
    Section s;
    PETSC_CHECK(PetscSectionGetField(this->obj, field, s));
    s.inc_reference();
    return s;
}

void
Section::set_num_field_components(Int field, Int n_comps)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetFieldComponents(this->obj, field, n_comps));
}

Int
Section::get_num_field_components(Int field) const
{
    CALL_STACK_MSG();
    Int n_comps;
    PETSC_CHECK(PetscSectionGetFieldComponents(this->obj, field, &n_comps));
    return n_comps;
}

void
Section::add_field_constraint_dof(Int point, Int field, Int n_dofs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionAddFieldConstraintDof(this->obj, point, field, n_dofs));
}

void
Section::set_field_constraint_dof(Int point, Int field, Int n_dofs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetFieldConstraintDof(this->obj, point, field, n_dofs));
}

Int
Section::get_field_constraint_dof(Int point, Int field) const
{
    CALL_STACK_MSG();
    Int n_dofs;
    PETSC_CHECK(PetscSectionGetFieldConstraintDof(this->obj, point, field, &n_dofs));
    return n_dofs;
}

void
Section::add_field_dof(Int point, Int field, Int n_dofs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionAddFieldDof(this->obj, point, field, n_dofs));
}

void
Section::set_field_dof(Int point, Int field, Int n_dofs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetFieldDof(this->obj, point, field, n_dofs));
}

Int
Section::get_field_dof(Int point, Int field) const
{
    CALL_STACK_MSG();
    Int n_dofs;
    PETSC_CHECK(PetscSectionGetFieldDof(this->obj, point, field, &n_dofs));
    return n_dofs;
}

void
Section::set_field_name(Int field, String name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetFieldName(this->obj, field, name.c_str()));
}

String
Section::get_field_name(Int field) const
{
    CALL_STACK_MSG();
    const char * name;
    PETSC_CHECK(PetscSectionGetFieldName(this->obj, field, &name));
    return { name };
}

Int
Section::get_offset(Int point) const
{
    CALL_STACK_MSG();
    Int offset;
    PETSC_CHECK(PetscSectionGetOffset(this->obj, point, &offset));
    return offset;
}

void
Section::get_offset_range(Int & start, Int & end) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionGetOffsetRange(this->obj, &start, &end));
}

Int
Section::get_field_offset(Int point, Int field) const
{
    CALL_STACK_MSG();
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->obj, point, field, &offset));
    return offset;
}

Int
Section::get_field_point_offset(Int point, Int field) const
{
    CALL_STACK_MSG();
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldPointOffset(this->obj, point, field, &offset));
    return offset;
}

Int
Section::get_storage_size() const
{
    CALL_STACK_MSG();
    Int sz;
    PetscSectionGetStorageSize(this->obj, &sz);
    return sz;
}

Int
Section::get_constrained_storage_size() const
{
    CALL_STACK_MSG();
    Int sz;
    PETSC_CHECK(PetscSectionGetConstrainedStorageSize(this->obj, &sz));
    return sz;
}

bool
Section::has_constraints() const
{
    CALL_STACK_MSG();
    PetscBool hc;
    PETSC_CHECK(PetscSectionHasConstraints(this->obj, &hc));
    return hc == PETSC_TRUE;
}

void
Section::set_component_name(Int field, Int comp, String name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetComponentName(this->obj, field, comp, name.c_str()));
}

String
Section::get_component_name(Int field, Int comp) const
{
    CALL_STACK_MSG();
    const char * name;
    PETSC_CHECK(PetscSectionGetComponentName(this->obj, field, comp, &name));
    return { name };
}

void
Section::add_constraint_dof(Int point, Int n_dofs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionAddConstraintDof(this->obj, point, n_dofs));
}

void
Section::set_constraint_dof(Int point, Int n_dofs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetConstraintDof(this->obj, point, n_dofs));
}

Int
Section::get_constraint_dof(Int point) const
{
    CALL_STACK_MSG();
    Int n_dofs;
    PETSC_CHECK(PetscSectionGetConstraintDof(this->obj, point, &n_dofs));
    return n_dofs;
}

void
Section::set_constraint_indices(Int point, Span<Int> indices)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetConstraintIndices(this->obj, point, indices.data()));
}

void
Section::set_constraint_indices(Int point, std::initializer_list<Int> indices)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetConstraintIndices(this->obj, point, std::data(indices)));
}

const Int *
Section::get_constraint_indices(Int point) const
{
    CALL_STACK_MSG();
    const Int * indices;
    PETSC_CHECK(PetscSectionGetConstraintIndices(this->obj, point, &indices));
    return indices;
}

void
Section::set_field_constraint_indices(Int point, Int field, Span<Int> indices)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetFieldConstraintIndices(this->obj, point, field, indices.data()));
}

void
Section::set_field_constraint_indices(Int point, Int field, std::initializer_list<Int> indices)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscSectionSetFieldConstraintIndices(this->obj, point, field, std::data(indices)));
}

const Int *
Section::get_field_constraint_indices(Int point, Int field) const
{
    CALL_STACK_MSG();
    const Int * indices;
    PETSC_CHECK(PetscSectionGetFieldConstraintIndices(this->obj, point, field, &indices));
    return indices;
}

Section
Section::create(DM dm,
                const Int n_comp[],
                const Int n_dof[],
                Int n_bc,
                const Int bc_field[],
                const IS bc_comps[],
                const IS bc_points[],
                IS perm)
{
    CALL_STACK_MSG();
    PetscSection s;
    PETSC_CHECK(DMPlexCreateSection(dm,
                                    nullptr,
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
