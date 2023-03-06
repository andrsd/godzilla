#pragma once

#include "petscsection.h"
#include "petscviewer.h"
#include "Types.h"

namespace godzilla {

class Section {
public:
    Section();
    Section(PetscSection s);

    void create(MPI_Comm comm);
    void destroy();
    void reset();
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    void set_chart(Int start, Int end);
    void get_chart(Int & start, Int & end) const;

    void set_up();

    void set_point_major(bool pm = true);
    bool get_point_major() const;

    Int get_max_dof() const;

    void set_num_fields(Int n_fields);
    Int get_num_fields() const;

    void add_dof(Int point, Int n_dofs);
    void set_dof(Int point, Int n_dofs);
    Int get_dof(Int point) const;

    Section get_field(Int field) const;

    void set_field_components(Int field, Int n_comps);
    Int get_field_components(Int field) const;

    void add_field_constraint_dof(Int point, Int field, Int n_dofs);
    void set_field_constraint_dof(Int point, Int field, Int n_dofs);
    Int get_field_constraint_dof(Int point, Int field) const;

    void add_field_dof(Int point, Int field, Int n_dofs);
    void set_field_dof(Int point, Int field, Int n_dofs);
    Int get_field_dof(Int point, Int field) const;

    void set_field_name(Int field, const std::string & name);
    std::string get_field_name(Int field) const;

    Int get_offset(Int point) const;
    void get_offset_range(Int & start, Int & end) const;
    Int get_field_offset(Int point, Int field) const;
    Int get_field_point_offset(Int point, Int field) const;

    Int get_storage_size() const;

    bool has_constraints() const;

    void set_component_name(Int field, Int comp, const std::string & name);
    std::string get_component_name(Int field, Int comp) const;

    void add_constraint_dof(Int point, Int n_dofs);
    void set_constraint_dof(Int point, Int n_dofs);
    Int get_constraint_dof(Int point) const;

    operator const PetscSection &() const { return this->section; }

    operator PetscSection &() { return this->section; }

    static Section create(DM dm,
                          DMLabel label[],
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
