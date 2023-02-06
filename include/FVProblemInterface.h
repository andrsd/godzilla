#pragma once

#include "GodzillaConfig.h"
#include "DiscreteProblemInterface.h"

namespace godzilla {

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
    NO_DISCARD Int get_field_dof(Int point, Int fid) const override;
    NO_DISCARD Vec get_solution_vector_local() const override;
    NO_DISCARD WeakForm * get_weak_form() const override;

    /// Adds a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual void add_field(Int id, const std::string & name, Int nc);

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
    virtual void compute_flux(Int dim,
                              Int nf,
                              const Real x[],
                              const Real n[],
                              const Scalar uL[],
                              const Scalar uR[],
                              Int n_consts,
                              const Scalar constants[],
                              Scalar flux[]) = 0;

    /// PETSc section
    PetscSection section;

    /// Field information
    struct FieldInfo {
        /// The name of the field
        std::string name;

        /// Field number
        Int id;

        /// The number of components
        Int nc;

        /// Component names
        std::vector<std::string> component_names;
    };

    /// Fields in the problem
    std::map<Int, FieldInfo> fields;

    /// Map from field name to field ID
    std::map<std::string, Int> fields_by_name;

    /// PETSc finite volume object
    PetscFV fvm;

    /// Local solution vector
    Vec sln;

    /// Object that manages a discrete system
    PetscDS ds;

    /// Weak form
    WeakForm * wf;

    friend void __compute_flux(Int dim,
                               Int nf,
                               const Real x[],
                               const Real n[],
                               const Scalar uL[],
                               const Scalar uR[],
                               Int n_consts,
                               const Scalar constants[],
                               Scalar flux[],
                               void * ctx);

    static const std::string empty_name;
};

} // namespace godzilla
