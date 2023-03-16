#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include "petsc.h"
#include "petscfe.h"
#include "CallStack.h"
#include "DiscreteProblemInterface.h"
#include "DependencyEvaluator.h"
#include "FieldValue.h"
#include "Error.h"
#include "Types.h"

namespace godzilla {

class Problem;
class AuxiliaryField;
class WeakForm;
class ValueFunctional;

/// Interface for FE problems
///
/// Any problem using PetscFE should inherit from this for unified API
class FEProblemInterface : public DiscreteProblemInterface, public DependencyEvaluator {
public:
    FEProblemInterface(Problem * problem, const Parameters & params);
    ~FEProblemInterface() override;

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
    const Vector & get_solution_vector_local() const override;
    virtual WeakForm * get_weak_form() const;

    /// Get number of auxiliary fields
    ///
    /// @return The number of auxiliary fields
    virtual Int get_num_aux_fields() const;

    /// Get auxiliary field name
    ///
    /// @param fid Auxiliary field ID
    /// @return Auxiliary field name
    virtual const std::string & get_aux_field_name(Int fid) const;

    /// Get auxiliary field ID
    ///
    /// @param name Auxiliary field name
    /// @param Auxiliary field ID
    virtual Int get_aux_field_id(const std::string & name) const;

    /// Do we have auxiliary field with specified ID
    ///
    /// @param fid The ID of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool has_aux_field_by_id(Int fid) const;

    /// Do we have auxiliary field with specified name
    ///
    /// @param name The name of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool has_aux_field_by_name(const std::string & name) const;

    /// Check if we have an auxiliary object with a specified name
    ///
    /// @param name The name of the object
    /// @return True if the object exists, otherwise false
    virtual bool has_aux(const std::string & name) const;

    /// Get auxiliary object with a specified name
    ///
    /// @param name The name of the object
    /// @return Pointer to the auxiliary object
    virtual const AuxiliaryField * get_aux(const std::string & name) const;

    /// Adds a volumetric field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @return ID of the new field
    virtual Int add_fe(const std::string & name, Int nc, Int k);

    /// Set a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual void set_fe(Int id, const std::string & name, Int nc, Int k);

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

    /// Add auxiliary field
    ///
    /// @param aux Auxiliary field object to add
    virtual void add_auxiliary_field(AuxiliaryField * aux);

    virtual const Int & get_spatial_dimension() const;

    virtual const FieldValue & get_field_value(const std::string & field_name) const;

    virtual const FieldGradient & get_field_gradient(const std::string & field_name) const;

    virtual const FieldValue & get_field_dot(const std::string & field_name) const;

    virtual const Real & get_time_shift() const;

    virtual const Real & get_time() const;

    virtual const Normal & get_normal() const;

    virtual const Point & get_xyz() const;

    /// Integrate
    virtual PetscErrorCode integrate(PetscDS ds,
                                     Int field,
                                     Int n_elems,
                                     PetscFEGeom * cell_geom,
                                     const Scalar coefficients[],
                                     PetscDS ds_aux,
                                     const Scalar coefficientsAux[],
                                     Scalar integral[]);

    /// Integrate field over a boundary
    virtual PetscErrorCode integrate_bnd(PetscDS ds,
                                         Int field,
                                         PetscBdPointFunc obj_func,
                                         Int n_elems,
                                         PetscFEGeom * face_geom,
                                         const Scalar coefficients[],
                                         PetscDS ds_aux,
                                         const Scalar coefficientsAux[],
                                         Scalar integral[]);

    /// Integrate residual
    virtual PetscErrorCode integrate_residual(PetscDS ds,
                                              PetscFormKey key,
                                              Int n_elems,
                                              PetscFEGeom * cell_geom,
                                              const Scalar coefficients[],
                                              const Scalar coefficients_t[],
                                              PetscDS ds_aux,
                                              const Scalar coefficients_aux[],
                                              Real t,
                                              Scalar elem_vec[]);

    /// Integrate residual over a boundary
    virtual PetscErrorCode integrate_bnd_residual(PetscDS ds,
                                                  PetscFormKey key,
                                                  Int n_elems,
                                                  PetscFEGeom * face_geom,
                                                  const Scalar coefficients[],
                                                  const Scalar coefficients_t[],
                                                  PetscDS ds_aux,
                                                  const Scalar coefficients_aux[],
                                                  Real t,
                                                  Scalar elem_vec[]);

    /// Integrate Jacobian
    virtual PetscErrorCode integrate_jacobian(PetscDS ds,
                                              PetscFEJacobianType jtype,
                                              PetscFormKey key,
                                              Int n_elems,
                                              PetscFEGeom * cell_geom,
                                              const Scalar coefficients[],
                                              const Scalar coefficients_t[],
                                              PetscDS ds_aux,
                                              const Scalar coefficients_aux[],
                                              Real t,
                                              Real u_tshift,
                                              Scalar elem_mat[]);

    // Integrate Jacobian over a boundary
    virtual PetscErrorCode integrate_bnd_jacobian(PetscDS ds,
                                                  PetscFormKey key,
                                                  Int n_elems,
                                                  PetscFEGeom * face_geom,
                                                  const Scalar coefficients[],
                                                  const Scalar coefficients_t[],
                                                  PetscDS ds_aux,
                                                  const Scalar coefficients_aux[],
                                                  Real t,
                                                  Real u_tshift,
                                                  Scalar elem_mat[]);

protected:
    struct FieldInfo;

    void create() override;
    void init() override;

    virtual void allocate_objects();

    /// Create FE object from FieldInfo
    ///
    /// @param fi Field description
    void create_fe(FieldInfo & fi);

    /// Set up discretization system
    void set_up_ds() override;

    /// Set up field variables
    virtual void set_up_fields() = 0;

    void add_boundary_natural_riemann(const std::string & name,
                                      DMLabel label,
                                      const std::vector<Int> & ids,
                                      Int field,
                                      const std::vector<Int> & components,
                                      PetscNaturalRiemannBCFunc * fn,
                                      PetscNaturalRiemannBCFunc * fn_t,
                                      void * context) const override;

    /// Set up quadrature
    virtual void set_up_quadrature();

    virtual void set_up_field_null_space(DM dm);

    /// Compute auxiliary fields
    ///
    /// @param dm DM for auxiliary fields
    void compute_aux_fields();

    void compute_global_aux_fields(DM dm, const std::vector<AuxiliaryField *> & auxs, Vector & a);

    void compute_label_aux_fields(DM dm,
                                  DMLabel label,
                                  const std::vector<AuxiliaryField *> & auxs,
                                  Vector & a);

    /// Set up auxiliary DM
    virtual void set_up_auxiliary_dm(DM dm);

    /// Setup volumetric weak form terms
    virtual void set_up_weak_form() = 0;

    void sort_functionals();
    void
    sort_residual_functionals(const std::map<std::string, const ValueFunctional *> & suppliers);
    void
    sort_jacobian_functionals(const std::map<std::string, const ValueFunctional *> & suppliers);

    PetscErrorCode update_element_vec(PetscFE fe,
                                      PetscTabulation tab,
                                      Int r,
                                      Scalar tmp_basis[],
                                      Scalar tmp_basis_der[],
                                      Int e,
                                      PetscFEGeom * fe_geom,
                                      Scalar f0[],
                                      Scalar f1[],
                                      Scalar elem_vec[]);

    PetscErrorCode update_element_mat(PetscFE fe_i,
                                      PetscFE fe_j,
                                      Int r,
                                      Int q,
                                      PetscTabulation tab_i,
                                      Scalar tmp_basis_i[],
                                      Scalar tmp_basis_der_i[],
                                      PetscTabulation tab_j,
                                      Scalar tmp_basis_j[],
                                      Scalar tmp_basis_der_j[],
                                      PetscFEGeom * fe_geom,
                                      const Scalar g0[],
                                      const Scalar g1[],
                                      const Scalar g2[],
                                      const Scalar g3[],
                                      Int e_offset,
                                      Int tot_dim,
                                      Int offset_i,
                                      Int offset_j,
                                      Scalar elem_mat[]);

    PetscErrorCode evaluate_field_jets(PetscDS ds,
                                       Int nf,
                                       Int r,
                                       Int q,
                                       PetscTabulation tab[],
                                       PetscFEGeom * fe_geom,
                                       const Scalar coefficients[],
                                       const Scalar coefficients_t[],
                                       Scalar u[],
                                       Scalar u_x[],
                                       Scalar u_t[]);

    Int get_next_id(const std::vector<Int> & ids) const;

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
        DMLabel block;

        /// The number of components
        Int nc;

        /// The degree k of the space
        Int k;

        /// Component names
        std::vector<std::string> component_names;

        /// Values (used during assembling)
        FieldValue values;

        /// Gradient (used during assembling)
        FieldGradient derivs;

        /// Time derivative (used during assembling)
        FieldValue dots;

        FieldInfo(const std::string & name, Int id, Int nc, Int k, const Int & dim) :
            name(name),
            id(id),
            fe(nullptr),
            block(nullptr),
            nc(nc),
            k(k),
            values(),
            derivs(dim),
            dots()
        {
        }

        FieldInfo(const FieldInfo & other) :
            name(other.name),
            id(other.id),
            fe(other.fe),
            block(other.block),
            nc(other.nc),
            k(other.k),
            component_names(other.component_names),
            values(other.values),
            derivs(other.derivs),
            dots(other.dots)
        {
        }
    };

    /// Fields in the problem
    std::map<Int, FieldInfo> fields;

    /// Map from field name to field ID
    std::map<std::string, Int> fields_by_name;

    /// Auxiliary fields in the problem
    std::map<Int, FieldInfo> aux_fields;

    /// Map from auxiliary field name to auxiliary field ID
    std::map<std::string, Int> aux_fields_by_name;

    /// List of auxiliary field objects
    std::vector<AuxiliaryField *> auxs;

    /// Map from aux object name to the aux object
    std::map<std::string, AuxiliaryField *> auxs_by_name;

    /// Map from region to list of auxiliary field objects
    std::map<std::string, std::vector<AuxiliaryField *>> auxs_by_region;

    /// DM for auxiliary fields
    DM dm_aux;

    /// Vector for auxiliary fields
    Vector a;

    /// Local solution vector
    Vector sln;

    /// Object that manages a discrete system for aux variables
    PetscDS ds_aux;

    /// Weak formulation
    WeakForm * wf;

    /// Data used during assembling procedure
    struct AssemblyData {
        /// Spatial dimension
        Int dim;
        /// Values of primary variables
        Scalar * u;
        /// Time derivative of primary variable values
        Scalar * u_t;
        /// Gradient of primary values
        Scalar * u_x;
        /// Offset into primary variable values (when having multiple fields)
        Int * u_offset;
        /// Offset into gradient of primary variables (when having multiple fields)
        Int * u_offset_x;
        /// Spatial coordinates
        Point xyz;
        /// Outward normals when doing surface integration
        Normal normals;
        /// Values of auxiliary fields
        Scalar * a;
        /// Gradients of auxiliary fields
        Scalar * a_x;
        /// Offset into auxiliary variable values (when having multiple fields)
        Int * a_offset;
        /// Offset into gradient of auxiliary variables (when having multiple fields)
        Int * a_offset_x;
        /// Time at which are our forms evaluated (NOTE: this is not the simulation time)
        Real time;
        /// the multiplier a for dF/dU_t
        Real u_t_shift;

        AssemblyData();
    } asmbl;

    /// Functionals that must be evaluated before the weak form residual functionals
    /// associated with the PetscFormKey are evaluated
    std::map<PetscFormKey, std::vector<const ValueFunctional *>> sorted_res_functionals;
    /// Functionals that must be evaluated before the weak form Jacobian functionals
    /// associated with the PetscFormKey are evaluated
    std::map<PetscFormKey, std::vector<const ValueFunctional *>> sorted_jac_functionals;
};

} // namespace godzilla
