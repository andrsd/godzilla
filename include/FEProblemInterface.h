#pragma once

#include <string>
#include <vector>
#include <map>
#include "petsc.h"
#include "petscfe.h"
#include "DiscreteProblemInterface.h"

namespace godzilla {

class Problem;
class AuxiliaryField;
class WeakForm;

/// Interface for FE problems
///
/// Any problem using PetscFE should inherit from this for unified API
class FEProblemInterface : public DiscreteProblemInterface {
public:
    FEProblemInterface(Problem * problem, const Parameters & params);
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
    virtual PetscInt get_field_dof(PetscInt point, PetscInt fid) const override;
    virtual Vec get_solution_vector_local() const override;
    virtual WeakForm * get_weak_form() const override;

    /// Get number of auxiliary fields
    ///
    /// @return The number of auxiliary fields
    virtual PetscInt get_num_aux_fields() const;

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

    const PetscInt & get_spatial_dimension() const;

    const PetscScalar * get_field_value(const std::string & field_name) const;

    const PetscScalar * get_field_gradient(const std::string & field_name) const;

    const PetscScalar * get_field_dot(const std::string & field_name) const;

    const PetscReal & get_time_shift() const;

    const PetscReal & get_time() const;

    PetscReal * const & get_normal() const;

    PetscReal * const & get_xyz() const;

    /// Integrate
    virtual PetscErrorCode integrate(PetscDS ds,
                                     PetscInt field,
                                     PetscInt n_elems,
                                     PetscFEGeom * cell_geom,
                                     const PetscScalar coefficients[],
                                     PetscDS ds_aux,
                                     const PetscScalar coefficientsAux[],
                                     PetscScalar integral[]);

    /// Integrate field over a boundary
    virtual PetscErrorCode integrate_bnd(PetscDS ds,
                                         PetscInt field,
                                         PetscBdPointFunc obj_func,
                                         PetscInt n_elems,
                                         PetscFEGeom * face_geom,
                                         const PetscScalar coefficients[],
                                         PetscDS ds_aux,
                                         const PetscScalar coefficientsAux[],
                                         PetscScalar integral[]);

    /// Integrate residual
    virtual PetscErrorCode integrate_residual(PetscDS ds,
                                              PetscFormKey key,
                                              PetscInt n_elems,
                                              PetscFEGeom * cell_geom,
                                              const PetscScalar coefficients[],
                                              const PetscScalar coefficients_t[],
                                              PetscDS ds_aux,
                                              const PetscScalar coefficients_aux[],
                                              PetscReal t,
                                              PetscScalar elem_vec[]);

    /// Integrate residual over a boundary
    virtual PetscErrorCode integrate_bnd_residual(PetscDS ds,
                                                  PetscFormKey key,
                                                  PetscInt n_elems,
                                                  PetscFEGeom * face_geom,
                                                  const PetscScalar coefficients[],
                                                  const PetscScalar coefficients_t[],
                                                  PetscDS ds_aux,
                                                  const PetscScalar coefficients_aux[],
                                                  PetscReal t,
                                                  PetscScalar elem_vec[]);

    /// Integrate Jacobian
    virtual PetscErrorCode integrate_jacobian(PetscDS ds,
                                              PetscFEJacobianType jtype,
                                              PetscFormKey key,
                                              PetscInt n_elems,
                                              PetscFEGeom * cell_geom,
                                              const PetscScalar coefficients[],
                                              const PetscScalar coefficients_t[],
                                              PetscDS ds_aux,
                                              const PetscScalar coefficients_aux[],
                                              PetscReal t,
                                              PetscReal u_tshift,
                                              PetscScalar elem_mat[]);

    // Integrate Jacobian over a boundary
    virtual PetscErrorCode integrate_bnd_jacobian(PetscDS ds,
                                                  PetscFormKey key,
                                                  PetscInt n_elems,
                                                  PetscFEGeom * face_geom,
                                                  const PetscScalar coefficients[],
                                                  const PetscScalar coefficients_t[],
                                                  PetscDS ds_aux,
                                                  const PetscScalar coefficients_aux[],
                                                  PetscReal t,
                                                  PetscReal u_tshift,
                                                  PetscScalar elem_mat[]);

protected:
    struct FieldInfo;

    virtual void create() override;
    virtual void init() override;
    virtual void allocate_objects();

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

    PetscErrorCode update_element_vec(PetscFE fe,
                                      PetscTabulation tab,
                                      PetscInt r,
                                      PetscScalar tmp_basis[],
                                      PetscScalar tmp_basis_der[],
                                      PetscInt e,
                                      PetscFEGeom * fe_geom,
                                      PetscScalar f0[],
                                      PetscScalar f1[],
                                      PetscScalar elem_vec[]);

    PetscErrorCode update_element_mat(PetscFE fe_i,
                                      PetscFE fe_j,
                                      PetscInt r,
                                      PetscInt q,
                                      PetscTabulation tab_i,
                                      PetscScalar tmp_basis_i[],
                                      PetscScalar tmp_basis_der_i[],
                                      PetscTabulation tab_j,
                                      PetscScalar tmp_basis_j[],
                                      PetscScalar tmp_basis_der_j[],
                                      PetscFEGeom * fe_geom,
                                      const PetscScalar g0[],
                                      const PetscScalar g1[],
                                      const PetscScalar g2[],
                                      const PetscScalar g3[],
                                      PetscInt e_offset,
                                      PetscInt tot_dim,
                                      PetscInt offset_i,
                                      PetscInt offset_j,
                                      PetscScalar elem_mat[]);

    PetscErrorCode evaluate_field_jets(PetscDS ds,
                                       PetscInt nf,
                                       PetscInt r,
                                       PetscInt q,
                                       PetscTabulation tab[],
                                       PetscFEGeom * fe_geom,
                                       const PetscScalar coefficients[],
                                       const PetscScalar coefficients_t[],
                                       PetscScalar u[],
                                       PetscScalar u_x[],
                                       PetscScalar u_t[]);

    /// PETSc section
    PetscSection section;

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

    /// Local solution vector
    Vec sln;

    /// Object that manages a discrete system
    PetscDS ds;

    /// Object that manages a discrete system for aux variables
    PetscDS ds_aux;

    /// Weak formulation
    WeakForm * wf;

    /// Spatial dimension
    PetscInt dim;

    PetscScalar * u;
    PetscScalar * u_t;
    PetscScalar * u_x;
    PetscInt * u_offset;
    PetscInt * u_offset_x;
    PetscReal * xyz;
    PetscReal * normals;
    PetscScalar * au;
    PetscScalar * au_x;
    PetscInt * au_offset;
    PetscInt * au_offset_x;
    /// Time at which are our forms evaluated (NOTE: this is not the simulation time)
    PetscReal time;
    /// the multiplier a for dF/dU_t
    PetscReal u_t_shift;
};

} // namespace godzilla
