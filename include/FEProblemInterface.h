#pragma once

#include <string>
#include <vector>
#include "petsc.h"
#include "petscfe.h"
#include "DiscreteProblemInterface.h"

namespace godzilla {

class Problem;
class AuxiliaryField;

/// Interface for FE problems
///
/// Any problem using PetscFE should inherit from this for unified API
class FEProblemInterface : public DiscreteProblemInterface {
public:
    FEProblemInterface(Problem * problem, const InputParameters & params);
    virtual ~FEProblemInterface();

    virtual PetscInt get_num_fields() const override;
    virtual std::vector<std::string> get_field_names() const override;
    virtual const std::string & get_field_name(PetscInt fid) const override;
    virtual PetscInt get_field_num_components(PetscInt fid) const override;
    virtual PetscInt get_field_id(const std::string & name) const override;
    virtual bool has_field_by_id(PetscInt fid) const override;
    virtual bool has_field_by_name(const std::string & name) const override;
    virtual PetscInt get_field_order(PetscInt fid) const override;
    virtual std::string get_field_component_name(PetscInt fid, PetscInt component) const override;
    virtual void
    set_field_component_name(PetscInt fid, PetscInt component, const std::string name) override;

    /// Get auxiliary field name
    ///
    /// @param fid Auxiliary field ID
    /// @return Auxiliary field name
    virtual const std::string & get_aux_field_name(PetscInt fid) const;

    /// Get auxiliary field ID
    ///
    /// @param name Auxiliary field name
    /// @param Auxiliary field ID
    virtual PetscInt get_aux_field_id(const std::string & name) const;

    /// Do we have auxiliary field with specified ID
    ///
    /// @param fid The ID of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool has_aux_field_by_id(PetscInt fid) const;

    /// Do we have auxiliary field with specified name
    ///
    /// @param name The name of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool has_aux_field_by_name(const std::string & name) const;

    /// Adds a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual void add_fe(PetscInt id, const std::string & name, PetscInt nc, PetscInt k);

    /// Adds a volumetric auxiliary field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual void add_aux_fe(PetscInt id, const std::string & name, PetscInt nc, PetscInt k);

    /// Add auxiliary field
    ///
    /// @param aux Auxiliary field object to add
    virtual void add_auxiliary_field(AuxiliaryField * aux);

protected:
    struct FieldInfo;

    virtual void create() override;
    virtual void init() override;

    /// Create FE object from FieldInfo
    ///
    /// @param fi Field description
    void create_fe(FieldInfo & fi);

    /// Set up discretization system
    virtual void set_up_ds() override;

    /// Set up quadrature
    virtual void set_up_quadrature();

    virtual void set_up_field_null_space(DM dm);

    /// Compute auxiliary fields
    ///
    /// @param dm DM for auxiliary fields
    void compute_aux_fields();

    void compute_global_aux_fields(DM dm, const std::vector<AuxiliaryField *> & auxs, Vec a);

    void compute_label_aux_fields(DM dm,
                                  DMLabel label,
                                  const std::vector<AuxiliaryField *> & auxs,
                                  Vec a);

    /// Set up auxiliary DM
    virtual void set_up_auxiliary_dm(DM dm);

    /// Setup volumetric weak form terms
    virtual void set_up_weak_form() = 0;

    /// Quadrature order
    PetscInt qorder;

    /// Field information
    struct FieldInfo {
        /// The name of the field
        std::string name;

        /// Field number
        PetscInt id;

        /// FE object
        PetscFE fe;

        /// Mesh support
        DMLabel block;

        /// The number of components
        PetscInt nc;

        /// The degree k of the space
        PetscInt k;

        /// Component names
        std::vector<std::string> component_names;
    };

    /// Fields in the problem
    std::map<PetscInt, FieldInfo> fields;

    /// Map from field name to field ID
    std::map<std::string, PetscInt> fields_by_name;

    /// Auxiliary fields in the problem
    std::map<PetscInt, FieldInfo> aux_fields;

    /// Map from auxiliary field name to auxiliary field ID
    std::map<std::string, PetscInt> aux_fields_by_name;

    /// List of auxiliary field objects
    std::vector<AuxiliaryField *> auxs;

    /// Map from region to list of auxiliary field objects
    std::map<std::string, std::vector<AuxiliaryField *>> auxs_by_region;

    /// DM for auxiliary fields
    DM dm_aux;

    /// Vector for auxiliary fields
    Vec a;
};

} // namespace godzilla
