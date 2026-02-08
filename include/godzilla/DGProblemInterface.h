// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Error.h"
#include "godzilla/String.h"

namespace godzilla {

class AuxiliaryField;

/// Interface for discontinuous galerkin finite element problems
///
class DGProblemInterface : public DiscreteProblemInterface {
public:
    DGProblemInterface(Problem & problem, const Parameters & pars);
    ~DGProblemInterface() override;

    Int get_num_fields() const override;
    std::vector<String> get_field_names() const override;
    Expected<String, ErrorCode> get_field_name(FieldID fid) const override;
    Expected<Int, ErrorCode> get_field_num_components(FieldID fid) const override;
    Expected<FieldID, ErrorCode> get_field_id(String name) const override;
    bool has_field_by_id(FieldID fid) const override;
    bool has_field_by_name(String name) const override;
    Expected<Order, ErrorCode> get_field_order(FieldID fid) const override;
    Expected<String, ErrorCode> get_field_component_name(FieldID fid, Int component) const override;
    void set_field_component_name(FieldID fid, Int component, String name) override;

    Int get_num_aux_fields() const override;
    std::vector<String> get_aux_field_names() const override;
    Expected<String, ErrorCode> get_aux_field_name(FieldID fid) const override;
    Expected<Int, ErrorCode> get_aux_field_num_components(FieldID fid) const override;
    Expected<FieldID, ErrorCode> get_aux_field_id(String name) const override;
    bool has_aux_field_by_id(FieldID fid) const override;
    bool has_aux_field_by_name(String name) const override;
    Expected<Order, ErrorCode> get_aux_field_order(FieldID fid) const override;
    Expected<String, ErrorCode> get_aux_field_component_name(FieldID fid,
                                                             Int component) const override;
    void set_aux_field_component_name(FieldID fid, Int component, String name) override;

    /// Adds a volumetric field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @return ID of the new field
    FieldID add_field(String name, Int nc, Order k, const Label & block = Label());

    /// Set a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    void set_field(FieldID id, String name, Int nc, Order k, const Label & block = Label());

    /// Adds a volumetric auxiliary field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @return ID of the new field
    FieldID add_aux_field(String name, Int nc, Order k, const Label & block = Label());

    /// Set a volumetric auxiliary field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    void set_aux_field(FieldID id, String name, Int nc, Order k, const Label & block = Label());

    /// Get field degree of freedom
    ///
    /// @param elem Element ID
    /// @param local_node Local node index
    /// @param fid Field ID
    /// @return Degree of freedom
    Int get_field_dof(Int elem, Int local_node, FieldID fid) const;

    /// Get auxiliary field degree of freedom
    ///
    /// @param elem Element ID
    /// @param local_node Local node index
    /// @param fid Auxiliary field ID
    /// @return Degree of freedom
    Int get_aux_field_dof(Int elem, Int local_node, FieldID fid) const;

protected:
    Int get_num_nodes_per_elem(Int c) const;
    Expected<PetscFE, ErrorCode> get_fe(FieldID fid) const;

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
        String name;
        /// Field number
        FieldID id;
        /// FE object
        PetscFE fe;
        /// Mesh support
        Label block;
        /// The number of components
        Int nc;
        /// The degree k of the space
        Order k;
        /// Component names
        std::vector<String> component_names;

        FieldInfo(String name, FieldID id, Int nc, Order k, const Label & block) :
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
    std::map<FieldID, FieldInfo> fields;

    /// Map from field name to field ID
    std::map<String, FieldID> fields_by_name;

    /// Auxiliary fields in the problem
    std::map<FieldID, FieldInfo> aux_fields;

    /// Map from auxiliary field name to auxiliary field ID
    std::map<String, FieldID> aux_fields_by_name;

    std::map<FieldID, PetscFE> aux_fe;

    static const String empty_name;
};

} // namespace godzilla
