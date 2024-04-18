// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>
#include <map>
#include "petsc.h"
#include "petscfe.h"
#include "godzilla/CallStack.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/DependencyEvaluator.h"
#include "godzilla/FieldValue.h"
#include "godzilla/Error.h"
#include "godzilla/Types.h"
#include "godzilla/WeakForm.h"

namespace godzilla {

class Problem;
class ResidualFunc;
class JacobianFunc;
class ValueFunctional;

/// Interface for FE problems
///
/// Any problem using PetscFE should inherit from this for unified API
class FEProblemInterface : public DiscreteProblemInterface, public DependencyEvaluator {
    struct FieldInfo;

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

    virtual WeakForm * get_weak_form() const;

    /// Adds a volumetric field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @return ID of the new field
    Int add_fe(const std::string & name, Int nc, Int k);

    /// Set a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    void set_fe(Int id, const std::string & name, Int nc, Int k);

    /// Adds a volumetric auxiliary field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @return ID of the new field
    Int add_aux_fe(const std::string & name, Int nc, Int k);

    /// Set a volumetric auxiliary field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    void set_aux_fe(Int id, const std::string & name, Int nc, Int k);

    const Int & get_spatial_dimension() const;

    const FieldValue & get_field_value(const std::string & field_name) const;

    const FieldGradient & get_field_gradient(const std::string & field_name) const;

    const FieldValue & get_field_dot(const std::string & field_name) const;

    const Real & get_time_shift() const;

    const Real & get_assembly_time() const;

    const Normal & get_normal() const;

    const Point & get_xyz() const;

    /// Add residual statement for a field variable
    ///
    /// @param fid Field ID
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    /// @param region Region name where this residual statement is active
    virtual void add_residual_block(Int fid,
                                    ResidualFunc * f0,
                                    ResidualFunc * f1,
                                    const std::string & region = "");

    /// Add boundary residual statement for a field variable
    ///
    /// @param fid Field ID
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    /// @param boundary Boundary name where this residual statement is active
    virtual void add_boundary_residual_block(Int fid,
                                             ResidualFunc * f0,
                                             ResidualFunc * f1,
                                             const std::string & boundary);

    /// Add Jacobian statement for a field variable
    ///
    /// @param fid Test field number
    /// @param gid Field number
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    /// @param region Region name where this Jacobian statement is active
    virtual void add_jacobian_block(Int fid,
                                    Int gid,
                                    JacobianFunc * g0,
                                    JacobianFunc * g1,
                                    JacobianFunc * g2,
                                    JacobianFunc * g3,
                                    const std::string & region = "");

    /// Add Jacobian preconditioner statement for a field variable
    ///
    /// @param fid Test field number
    /// @param gid Field number
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    /// @param region Region name where this Jacobian statement is active
    virtual void add_jacobian_preconditioner_block(Int fid,
                                                   Int gid,
                                                   JacobianFunc * g0,
                                                   JacobianFunc * g1,
                                                   JacobianFunc * g2,
                                                   JacobianFunc * g3,
                                                   const std::string & region = "");

    /// Add boundary Jacobian statement for a field variable
    ///
    /// @param fid Test field number
    /// @param gid Field number
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    /// @param boundary Boundary name where this Jacobian statement is active
    virtual void add_boundary_jacobian_block(Int fid,
                                             Int gid,
                                             JacobianFunc * g0,
                                             JacobianFunc * g1,
                                             JacobianFunc * g2,
                                             JacobianFunc * g3,
                                             const std::string & boundary);

    void add_weak_form_residual_block(WeakForm::ResidualKind kind,
                                      Int field_id,
                                      ResidualFunc * f,
                                      const Label & label = Label(),
                                      Int val = 0,
                                      Int part = 0);

    void add_weak_form_jacobian_block(WeakForm::JacobianKind kind,
                                      Int fid,
                                      Int gid,
                                      JacobianFunc * g,
                                      const Label & label = Label(),
                                      Int val = 0,
                                      Int part = 0);

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
    const std::map<Int, FieldInfo> & get_fields() const;

    void create() override;
    void init() override;

    /// Create FE object from FieldInfo
    ///
    /// @param fi Field description
    void create_fe(FieldInfo & fi);

    /// Set up discretization system
    void set_up_ds() override;

    virtual void set_up_assembly_data();
    virtual void set_up_assembly_data_aux();

    /// Set up field variables
    virtual void set_up_fields() = 0;

    void add_boundary_natural_riemann(const std::string & name,
                                      const std::string & boundary,
                                      Int field,
                                      const std::vector<Int> & components,
                                      PetscNaturalRiemannBCFunc * fn,
                                      PetscNaturalRiemannBCFunc * fn_t,
                                      void * context) override;

    /// Set up quadrature
    virtual void set_up_quadrature();

    virtual void set_up_field_null_space(DM dm);

    void set_up_aux_fields() override;

    /// Setup volumetric weak form terms
    virtual void set_up_weak_form() = 0;

    virtual void sort_functionals();
    void
    sort_residual_functionals(const std::map<std::string, const ValueFunctional *> & suppliers);
    void
    sort_jacobian_functionals(const std::map<std::string, const ValueFunctional *> & suppliers);

    void update_element_vec(PetscFE fe,
                            PetscTabulation tab,
                            Int r,
                            Scalar tmp_basis[],
                            Scalar tmp_basis_der[],
                            Int e,
                            PetscFEGeom * fe_geom,
                            Scalar f0[],
                            Scalar f1[],
                            Scalar elem_vec[]);

    void update_element_mat(PetscFE fe_i,
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

    void evaluate_field_jets(PetscDS ds,
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

private:
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

        FieldInfo(const std::string & name, Int id, Int nc, Int k, Int dim) :
            name(name),
            id(id),
            fe(nullptr),
            block(nullptr),
            nc(nc),
            k(k),
            values(nc),
            derivs(dim, nc),
            dots(nc)
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
        /// Spatial coordinates
        Point xyz;
        /// Outward normals when doing surface integration
        Normal normals;
        /// Values of auxiliary fields
        Scalar * a;
        /// Gradients of auxiliary fields
        Scalar * a_x;
        /// Time at which are our forms evaluated (NOTE: this is not the simulation time)
        Real time;
        /// the multiplier a for dF/dU_t
        Real u_t_shift;

        AssemblyData(Int dim);
    } * asmbl;

    /// Functionals that must be evaluated before the weak form residual functionals
    /// associated with the PetscFormKey are evaluated
    std::map<PetscFormKey, std::vector<const ValueFunctional *>> sorted_res_functionals;
    /// Functionals that must be evaluated before the weak form Jacobian functionals
    /// associated with the PetscFormKey are evaluated
    std::map<PetscFormKey, std::vector<const ValueFunctional *>> sorted_jac_functionals;
};

} // namespace godzilla
