#pragma once

#include "godzilla/GodzillaConfig.h"
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

    NO_DISCARD Int get_num_fields() const override;
    NO_DISCARD std::vector<std::string> get_field_names() const override;
    const std::string & get_field_name(Int fid) const override;
    Int get_field_num_components(Int fid) const override;
    NO_DISCARD Int get_field_id(const std::string & name) const override;
    NO_DISCARD bool has_field_by_id(Int fid) const override;
    NO_DISCARD bool has_field_by_name(const std::string & name) const override;
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
    virtual void add_field(Int id, const std::string & name, Int nc);

    /// Adds a volumetric auxiliary field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @return ID of the new field
    virtual Int add_aux_fe(const std::string & name, Int nc, Int k);

    /// Set a volumetric auxiliary field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual void set_aux_fe(Int id, const std::string & name, Int nc, Int k);

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

    /// Compute auxiliary fields
    ///
    /// @param dm DM for auxiliary fields
    void compute_aux_fields();

    void compute_label_aux_fields(DM dm,
                                  const Label & label,
                                  const std::vector<AuxiliaryField *> & auxs,
                                  Vector & a);

    void set_up_aux_fields() override;

    /// Set up field variables
    virtual void set_up_fields() = 0;

    Int get_next_id(const std::vector<Int> & ids) const;

private:
    /// Field information
    struct FieldInfo {
        /// The name of the field
        std::string name;

        /// Field number
        Int id;

        /// Mesh support
        DMLabel block;

        /// The number of components
        Int nc;

        /// The degree k of the space
        Int k;

        /// Component names
        std::vector<std::string> component_names;

        FieldInfo(const std::string & name, Int id, Int nc, Int k) :
            name(name),
            id(id),
            block(nullptr),
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

    /// Local auxiliary solution vector
    Vector a;

    static const std::string empty_name;
};

} // namespace godzilla
