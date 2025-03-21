// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/DiscreteProblemInterface.h"

namespace godzilla {

class AuxiliaryField;

/// Interface for discontinuous galerkin finite element problems
///
class DGProblemInterface : public DiscreteProblemInterface {
public:
    DGProblemInterface(Problem * problem, const Parameters & params);
    ~DGProblemInterface() override;

    Int get_num_fields() const override;
    std::vector<std::string> get_field_names() const override;
    const std::string & get_field_name(Int fid) const override;
    Int get_field_num_components(Int fid) const override;
    Int get_field_id(const std::string & name) const override;
    bool has_field_by_id(Int fid) const override;
    bool has_field_by_name(const std::string & name) const override;
    Int get_field_order(Int fid) const override;
    std::string get_field_component_name(Int fid, Int component) const override;
    void set_field_component_name(Int fid, Int component, const std::string & name) override;

    Int get_num_aux_fields() const override;
    std::vector<std::string> get_aux_field_names() const override;
    const std::string & get_aux_field_name(Int fid) const override;
    Int get_aux_field_num_components(Int fid) const override;
    Int get_aux_field_id(const std::string & name) const override;
    bool has_aux_field_by_id(Int fid) const override;
    bool has_aux_field_by_name(const std::string & name) const override;
    Int get_aux_field_order(Int fid) const override;
    std::string get_aux_field_component_name(Int fid, Int component) const override;
    void set_aux_field_component_name(Int fid, Int component, const std::string & name) override;

    /// Adds a volumetric field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @return ID of the new field
    Int add_field(const std::string & name, Int nc, Int k, const Label & block = Label());

    /// Set a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    void set_field(Int id, const std::string & name, Int nc, Int k, const Label & block = Label());

    /// Adds a volumetric auxiliary field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @return ID of the new field
    Int add_aux_field(const std::string & name, Int nc, Int k, const Label & block = Label());

    /// Set a volumetric auxiliary field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    void
    set_aux_field(Int id, const std::string & name, Int nc, Int k, const Label & block = Label());

    /// Get field degree of freedom
    ///
    /// @param elem Element ID
    /// @param local_node Local node index
    /// @param fid Field ID
    /// @return Degree of freedom
    Int get_field_dof(Int elem, Int local_node, Int fid) const;

    /// Get auxiliary field degree of freedom
    ///
    /// @param elem Element ID
    /// @param local_node Local node index
    /// @param fid Auxiliary field ID
    /// @return Degree of freedom
    Int get_aux_field_dof(Int elem, Int local_node, Int fid) const;

protected:
    Int get_num_nodes_per_elem(Int c) const;
    PetscFE get_fe(Int fid) const;

    void init() override;
    void create() override;
    void set_up_ds() override;

    void create_section();
    void set_up_section_constraint_dofs(Section & section);
    void set_up_section_constraint_indicies(Section & section);

    void set_up_assembly_data_aux();

    /// Set up field variables
    virtual void set_up_fields() = 0;

    void create_aux_fields() override;

    void set_up_weak_form() override;

private:
    /// Local section
    Section section;

    /// Local section for auxiliary fields
    Section section_aux;

    /// Quadrature order
    Int qorder;

    /// Field information
    struct FieldInfo {
        /// The name of the field
        std::string name;
        /// Field number
        Int id;
        /// FE object
        PetscFE fe;
        /// Mesh support
        Label block;
        /// The number of components
        Int nc;
        /// The degree k of the space
        Int k;
        /// Component names
        std::vector<std::string> component_names;

        FieldInfo(const std::string & name, Int id, Int nc, Int k, const Label & block) :
            name(name),
            id(id),
            fe(nullptr),
            block(block),
            nc(nc),
            k(k)
        {
        }

        FieldInfo(const FieldInfo & other) = default;
    };

    void create_fe(FieldInfo & fi);

    /// Fields in the problem
    std::map<Int, FieldInfo> fields;

    /// Map from field name to field ID
    std::map<std::string, Int> fields_by_name;

    /// Auxiliary fields in the problem
    std::map<Int, FieldInfo> aux_fields;

    /// Map from auxiliary field name to auxiliary field ID
    std::map<std::string, Int> aux_fields_by_name;

    std::map<Int, PetscFE> aux_fe;

    static const std::string empty_name;
};

} // namespace godzilla
