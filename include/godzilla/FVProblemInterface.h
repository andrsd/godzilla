// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/DiscreteProblemInterface.h"

namespace godzilla {

class AuxiliaryField;

/// Interface for finite volume problems
///
/// Any problem using PetscFV should inherit from this for unified API
class FVProblemInterface : public DiscreteProblemInterface {
public:
    FVProblemInterface(Problem * problem, const Parameters & params);
    ~FVProblemInterface() override;

    [[nodiscard]] Int get_num_fields() const override;
    [[nodiscard]] std::vector<std::string> get_field_names() const override;
    const std::string & get_field_name(Int fid) const override;
    Int get_field_num_components(Int fid) const override;
    [[nodiscard]] Int get_field_id(const std::string & name) const override;
    [[nodiscard]] bool has_field_by_id(Int fid) const override;
    [[nodiscard]] bool has_field_by_name(const std::string & name) const override;
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
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @param block The label this field is restricted to
    void add_field(Int id, const std::string & name, Int nc, const Label & block = Label());

    /// Adds a volumetric auxiliary field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @param block The label this field is restricted to
    /// @return ID of the new field
    Int add_aux_fe(const std::string & name, Int nc, Int k, const Label & block = Label());

    /// Set a volumetric auxiliary field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @param block The label this field is restricted to
    void set_aux_fe(Int id, const std::string & name, Int nc, Int k, const Label & block = Label());

    /// Method to compute flux across an edge
    ///
    /// @param dim[in] Spatial dimension
    /// @param nf[in] Number of fields
    /// @param x[in] Edge centroid
    /// @param n[in] Normal
    /// @param uL[in] Solution on the "left" side
    /// @param uR[in] Solution on the "right" side
    /// @param n_consts[in] Number of constants
    /// @param constants[in] Constants
    /// @param flux[out] Computed flux
    /// @return PETSc error code, zero means success
    virtual PetscErrorCode compute_flux(PetscInt dim,
                                        PetscInt nf,
                                        const PetscReal x[],
                                        const PetscReal n[],
                                        const PetscScalar uL[],
                                        const PetscScalar uR[],
                                        PetscInt n_consts,
                                        const PetscScalar constants[],
                                        PetscScalar flux[]) = 0;

protected:
    void init() override;
    void create() override;
    void set_up_ds() override;
    void add_boundary_essential(const std::string & name,
                                const std::string & boundary,
                                Int field,
                                const std::vector<Int> & components,
                                PetscFunc * fn,
                                PetscFunc * fn_t,
                                void * context) override;
    void add_boundary_natural(const std::string & name,
                              const std::string & boundary,
                              Int field,
                              const std::vector<Int> & components,
                              void * context) override;

    void set_up_aux_fields() override;

    /// Set up field variables
    virtual void set_up_fields() = 0;

private:
    /// Field information
    struct FieldInfo {
        /// The name of the field
        std::string name;

        /// Field number
        Int id;

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
            block(block),
            nc(nc),
            k(k)
        {
        }

        FieldInfo(const FieldInfo & other) :
            name(other.name),
            id(other.id),
            block(other.block),
            nc(other.nc),
            k(other.k),
            component_names(other.component_names)
        {
        }
    };

    /// Fields in the problem
    std::map<Int, FieldInfo> fields;

    /// Map from field name to field ID
    std::map<std::string, Int> fields_by_name;

    /// PETSc finite volume object
    PetscFV fvm;

    /// Auxiliary fields in the problem
    std::map<Int, FieldInfo> aux_fields;

    /// Map from auxiliary field name to auxiliary field ID
    std::map<std::string, Int> aux_fields_by_name;

    std::map<Int, PetscFE> aux_fe;

    static const std::string empty_name;
};

} // namespace godzilla
