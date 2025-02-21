// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "petscsection.h"
#include "petscdm.h"
#include "petscviewer.h"
#include <vector>

namespace godzilla {

class Range;

class Section {
public:
    Section();
    Section(PetscSection s);

    /// Allocates a Section and sets the map contents to the default.
    ///
    /// @param comm  the MPI communicator
    void create(MPI_Comm comm);

    /// Frees the Section
    void destroy();

    /// Frees all Section data, the Section is then as if `create` had just been called.
    void reset();

    /// Views a Section
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    /// Sets the range [start, end) in which points (indices) lie for this Section on this MPI
    /// process
    ///
    /// @param start The first point
    /// @param end One past the last point
    void set_chart(Int start, Int end);

    /// Sets the range [start, end) in which points (indices) lie for this Section on this MPI
    /// process
    ///
    /// @param range The range
    void set_chart(const Range & range);

    /// Returns the range [start, end) in which points (indices) lie for this Section on this MPI
    /// process
    ///
    /// @param start The first point
    /// @param end One past the last point
    void get_chart(Int & start, Int & end) const;

    /// Returns the range [start, end) in which points (indices) lie for this Section on this MPI
    /// process
    ///
    /// @return The range
    Range get_chart() const;

    /// Calculate offsets based upon the number of degrees of freedom for each point in preparation
    /// for use of the Section
    void set_up();

    /// Sets the flag for dof ordering
    ///
    /// @param pm `true` for point major ordering, otherwise it will be field major
    void set_point_major(bool pm = true);

    /// Returns the flag for dof ordering
    ///
    /// @return `true` if it is point major, `false` if it is field major
    bool get_point_major() const;

    /// Return the maximum number of degrees of freedom on any point in the Section
    ///
    /// @return The maximum dof
    Int get_max_dof() const;

    /// Sets the number of fields in the Section
    ///
    /// @param n_fields The number of fields
    void set_num_fields(Int n_fields);

    /// Returns the number of fields in Section
    ///
    /// @return The number of fields defined, or 0 if none were defined
    Int get_num_fields() const;

    /// Adds to the total number of degrees of freedom associated with a given point
    ///
    /// @param point The point
    /// @param n_dofs The number of additional dof
    void add_dof(Int point, Int n_dofs);

    /// Sets the total number of degrees of freedom associated with a given point
    ///
    /// @param point The point
    /// @param n_dofs The number of dof
    void set_dof(Int point, Int n_dofs);

    /// Return the total number of degrees of freedom associated with a given point
    ///
    /// @param point The point
    /// @return The number of dof
    Int get_dof(Int point) const;

    /// Get the Section associated with a single field
    ///
    /// @param field The field number
    /// @return The `Section` for the given field, note the chart of the section is not set
    Section get_field(Int field) const;

    /// Sets the number of field components for the given field
    ///
    /// @param field The field number
    /// @param n_comps The number of field components
    void set_num_field_components(Int field, Int n_comps);

    /// Returns the number of field components for the given field
    ///
    /// @param field The field number
    /// @return The number of field components
    Int get_num_field_components(Int field) const;

    /// Increment the number of constrained degrees of freedom associated with a given field on a
    /// point
    ///
    /// @param point The point
    /// @param field The field
    /// @param n_dofs The number of additional dof which are fixed by constraints
    void add_field_constraint_dof(Int point, Int field, Int n_dofs);

    /// Set the number of constrained degrees of freedom associated with a given field on a point
    ///
    /// @param point The point
    /// @param field The field number
    /// @param n_dofs The number of dof which are fixed by constraints
    void set_field_constraint_dof(Int point, Int field, Int n_dofs);

    /// Return the number of constrained degrees of freedom associated with a given field on a point
    ///
    /// @param point The point
    /// @param field The field
    /// @return The number of dof which are fixed by constraints
    Int get_field_constraint_dof(Int point, Int field) const;

    /// Adds a number of degrees of freedom associated with a field on a given point
    ///
    /// @param point The point
    /// @param field The field
    /// @param n_dofs The number of dof
    void add_field_dof(Int point, Int field, Int n_dofs);

    /// Sets the number of degrees of freedom associated with a field on a given point
    ///
    /// @param point The point
    /// @param field The field
    /// @param n_dofs The number of dof
    void set_field_dof(Int point, Int field, Int n_dofs);

    /// Return the number of degrees of freedom associated with a field on a given point
    ///
    /// @param point The point
    /// @param field The field
    /// @return The number of dof
    Int get_field_dof(Int point, Int field) const;

    /// Sets the name of a field in the Section
    ///
    /// @param field The field number
    /// @param name The field name
    void set_field_name(Int field, const std::string & name);

    /// Returns the name of a field in the Section
    ///
    /// @param field The field number
    /// @return The field name
    std::string get_field_name(Int field) const;

    /// Return the offset into an array or `Vector` for the dof associated with the given point
    ///
    /// @param point The point
    /// @return The offset
    Int get_offset(Int point) const;

    /// Return the full range of offsets [`start`, `end`) for the Section
    ///
    /// @param start The minimum offset
    /// @param end One more than the maximum offset
    void get_offset_range(Int & start, Int & end) const;

    /// Return the offset into an array or `Vector` for the field dof associated with the given
    /// point
    ///
    /// @param point The point
    /// @param field The field
    /// @return The offset
    Int get_field_offset(Int point, Int field) const;

    /// Return the offset for the first field dof associated with the given point relative to the
    /// offset for that point for the unnamed default field’s first dof
    ///
    /// @param point The point
    /// @param field The field
    /// @return The offset
    Int get_field_point_offset(Int point, Int field) const;

    /// Return the size of an array or local Vector capable of holding all the degrees of freedom
    /// defined in a Section
    ///
    /// @return The size of an array which can hold all the dofs
    Int get_storage_size() const;

    /// Determine whether the Section has constrained dofs
    ///
    /// @return `true` if the section has constrained dofs, `false` otherwise
    bool has_constraints() const;

    /// Sets the name of a field component in the Section
    ///
    /// @param field The field number
    /// @param comp The component number
    /// @param name The component name
    void set_component_name(Int field, Int comp, const std::string & name);

    /// Gets the name of a field component in the Section
    ///
    /// @param field The field number
    /// @param comp The component number
    /// @return The component name
    std::string get_component_name(Int field, Int comp) const;

    /// Increment the number of constrained degrees of freedom associated with a given point
    ///
    /// @param point The point
    /// @param n_dofs The number of additional dof which are fixed by constraints
    void add_constraint_dof(Int point, Int n_dofs);

    /// Set the number of constrained degrees of freedom associated with a given point
    ///
    /// @param point The point
    /// @param n_dofs The number of dof which are fixed by constraints
    void set_constraint_dof(Int point, Int n_dofs);

    /// Return the number of constrained degrees of freedom associated with a given point.
    ///
    /// @param point The point
    /// @return The number of dof which are fixed by constraints
    Int get_constraint_dof(Int point) const;

    /// Set the point dof numbers, in [0, dof), which are constrained
    ///
    /// @param point The point
    /// @param indices The constrained dofs
    void set_constraint_indices(Int point, const std::vector<Int> & indices);

    /// Get the point dof numbers, in [0, dof), which are constrained for a given point
    ///
    /// @param point The point
    /// @return The constrained dofs
    const Int * get_constraint_indices(Int point) const;

    /// Set the field dof numbers, in [0, fdof), which are constrained
    ///
    /// @param point The point
    /// @param field The field number
    /// @param indices The constrained dofs
    void set_field_constraint_indices(Int point, Int field, const std::vector<Int> & indices);

    /// Get the field dof numbers, in [0, fdof), which are constrained
    ///
    /// @param point The point
    /// @param field The field number
    /// @return The constrained dofs sorted in ascending order
    const Int * get_field_constraint_indices(Int point, Int field) const;

    operator const PetscSection &() const { return this->section; }

    operator PetscSection &() { return this->section; }

    operator bool() const { return this->section != nullptr; }

    static Section create(DM dm,
                          const Int n_comp[],
                          const Int n_dof[],
                          Int n_bc,
                          const Int bc_field[],
                          const IS bc_comps[],
                          const IS bc_points[],
                          IS perm);

private:
    PetscSection section;
};

} // namespace godzilla
