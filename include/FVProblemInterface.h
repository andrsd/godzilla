#pragma once

#include "DiscreteProblemInterface.h"

namespace godzilla {

/// Interface for finite volume problems
///
/// Any problem using PetscFV should inherit from this for unified API
class FVProblemInterface : public DiscreteProblemInterface {
public:
    FVProblemInterface(Problem * problem, const Parameters & params);
    ~FVProblemInterface() override;

    PetscInt get_num_fields() const override;
    std::vector<std::string> get_field_names() const override;
    const std::string & get_field_name(PetscInt fid) const override;
    PetscInt get_field_num_components(PetscInt fid) const override;
    PetscInt get_field_id(const std::string & name) const override;
    bool has_field_by_id(PetscInt fid) const override;
    bool has_field_by_name(const std::string & name) const override;
    PetscInt get_field_order(PetscInt fid) const override;
    std::string get_field_component_name(PetscInt fid, PetscInt component) const override;
    void
    set_field_component_name(PetscInt fid, PetscInt component, const std::string & name) override;
    PetscInt get_field_dof(PetscInt point, PetscInt fid) const override;
    Vec get_solution_vector_local() const override;
    WeakForm * get_weak_form() const override;

    /// Adds a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual void add_field(PetscInt id, const std::string & name, PetscInt nc);

protected:
    void init() override;
    void create() override;
    virtual void allocate_objects();
    void set_up_ds() override;

    /// Compute flux
    ///
    /// @param dim Spatial dimension
    /// @param nf Nuber of fields
    /// @param x The coordinates at a point on the interface
    /// @param n The normal vector to the interface
    /// @param uL The state vector to the left of the interface
    /// @param uR The state vector to the right of the interface
    /// @param n_consts Number of constant parameters
    /// @param constants Constant parameters
    /// @param flux Computed flux through the interface
    virtual void compute_flux(PetscInt dim,
                              PetscInt nf,
                              const PetscReal x[],
                              const PetscReal n[],
                              const PetscScalar uL[],
                              const PetscScalar uR[],
                              PetscInt n_consts,
                              const PetscScalar constants[],
                              PetscScalar flux[]) = 0;

    /// PETSc section
    PetscSection section;

    /// Field information
    struct FieldInfo {
        /// The name of the field
        std::string name;

        /// Field number
        PetscInt id;

        /// The number of components
        PetscInt nc;

        /// Component names
        std::vector<std::string> component_names;
    };

    /// Fields in the problem
    std::map<PetscInt, FieldInfo> fields;

    /// Map from field name to field ID
    std::map<std::string, PetscInt> fields_by_name;

    /// PETSc finite volume object
    PetscFV fvm;

    /// Local solution vector
    Vec sln;

    /// Object that manages a discrete system
    PetscDS ds;

    /// Weak form
    WeakForm * wf;

    friend void __compute_flux(PetscInt dim,
                               PetscInt nf,
                               const PetscReal x[],
                               const PetscReal n[],
                               const PetscScalar uL[],
                               const PetscScalar uR[],
                               PetscInt n_consts,
                               const PetscScalar constants[],
                               PetscScalar flux[],
                               void * ctx);

    static const std::string empty_name;
};

} // namespace godzilla
