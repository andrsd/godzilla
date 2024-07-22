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
template <WeakForm::ResidualKind>
class ResidualFunc;
class AbstractResidualFunctional;
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

    [[nodiscard]] Int get_num_fields() const override;
    [[nodiscard]] std::vector<std::string> get_field_names() const override;
    [[nodiscard]] const std::string & get_field_name(Int fid) const override;
    [[nodiscard]] Int get_field_num_components(Int fid) const override;
    [[nodiscard]] Int get_field_id(const std::string & name) const override;
    [[nodiscard]] bool has_field_by_id(Int fid) const override;
    [[nodiscard]] bool has_field_by_name(const std::string & name) const override;
    [[nodiscard]] Int get_field_order(Int fid) const override;
    [[nodiscard]] std::string get_field_component_name(Int fid, Int component) const override;
    void set_field_component_name(Int fid, Int component, const std::string & name) override;

    [[nodiscard]] Int get_num_aux_fields() const override;
    [[nodiscard]] std::vector<std::string> get_aux_field_names() const override;
    [[nodiscard]] const std::string & get_aux_field_name(Int fid) const override;
    [[nodiscard]] Int get_aux_field_num_components(Int fid) const override;
    [[nodiscard]] Int get_aux_field_id(const std::string & name) const override;
    [[nodiscard]] bool has_aux_field_by_id(Int fid) const override;
    [[nodiscard]] bool has_aux_field_by_name(const std::string & name) const override;
    [[nodiscard]] Int get_aux_field_order(Int fid) const override;
    [[nodiscard]] std::string get_aux_field_component_name(Int fid, Int component) const override;
    void set_aux_field_component_name(Int fid, Int component, const std::string & name) override;

    [[nodiscard]] WeakForm * get_weak_form() const;

    /// Adds a volumetric field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @param block The mesh region this field is restricted to
    /// @return ID of the new field
    Int add_field(const std::string & name, Int nc, Int k, const Label & block = Label());

    /// Set a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @param block The mesh region this field is restricted to
    void set_field(Int id, const std::string & name, Int nc, Int k, const Label & block = Label());

    /// Adds a volumetric auxiliary field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @param block The mesh region this field is restricted to
    /// @return ID of the new field
    Int add_aux_field(const std::string & name, Int nc, Int k, const Label & block = Label());

    /// Set a volumetric auxiliary field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @param block The mesh region this field is restricted to
    void
    set_aux_field(Int id, const std::string & name, Int nc, Int k, const Label & block = Label());

    [[nodiscard]] const Int & get_spatial_dimension() const;

    [[nodiscard]] const FieldValue & get_field_value(const std::string & field_name) const;

    [[nodiscard]] const FieldGradient & get_field_gradient(const std::string & field_name) const;

    [[nodiscard]] const FieldValue & get_field_dot(const std::string & field_name) const;

    [[nodiscard]] const Real & get_time_shift() const;

    [[nodiscard]] const Real & get_assembly_time() const;

    [[nodiscard]] const Normal & get_normal() const;

    [[nodiscard]] const Point & get_xyz() const;

    /// Add residual statement for a field variable
    ///
    /// @param fid Field ID
    /// @param res_func Integrand
    /// @param region Region name where this residual statement is active
    template <WeakForm::ResidualKind KIND>
    void
    add_residual_block(Int fid,
                       AbstractResidualFunctional * res_fn,
                       const std::string & region = "")
    {
        throw Exception("Unsupported residual functional kind");
    }

    /// Add Jacobian statement for a field variable
    ///
    /// @param fid Test field number
    /// @param gid Field number
    /// @param jac_fn Integrand
    /// @param region Region name where this Jacobian statement is active
    template <WeakForm::JacobianKind KIND>
    void
    add_jacobian_block(Int fid, Int gid, JacobianFunc * jac_fn, const std::string & region = "")
    {
        throw Exception("Unsupported residual functional kind");
    }

    /// Integrate residual
    ErrorCode integrate_residual(PetscDS ds,
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
    ErrorCode integrate_bnd_residual(PetscDS ds,
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
    ErrorCode integrate_jacobian(PetscDS ds,
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
    ErrorCode integrate_bnd_jacobian(PetscDS ds,
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
    [[nodiscard]] const std::map<Int, FieldInfo> & get_fields() const;

    void create() override;
    void init() override;

    /// Create FE object from FieldInfo
    ///
    /// @param fi Field description
    void create_fe(FieldInfo & fi);

    /// Set up discretization system
    void set_up_ds() override;

    void set_up_assembly_data();
    void set_up_assembly_data_aux();

    /// Set up field variables
    virtual void set_up_fields() = 0;

    /// Set up quadrature
    virtual void set_up_quadrature();

    virtual void set_up_field_null_space(DM dm);

    void create_aux_fields() override;

    void sort_functionals();

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
    void add_weak_form_residual_block(WeakForm::ResidualKind kind,
                                      Int fid,
                                      AbstractResidualFunctional * res_fn,
                                      const std::string & region);

    void add_weak_form_bnd_residual_block(WeakForm::ResidualKind kind,
                                          Int fid,
                                          AbstractResidualFunctional * res_fn,
                                          const std::string & region);

    void add_weak_form_jacobian_block(WeakForm::JacobianKind kind,
                                      Int fid,
                                      Int gid,
                                      JacobianFunc * jac_fn,
                                      const std::string & region);

    void add_weak_form_bnd_jacobian_block(WeakForm::JacobianKind kind,
                                          Int fid,
                                          Int gid,
                                          JacobianFunc * jac_fn,
                                          const std::string & boundary);

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
        /// Values (used during assembling)
        FieldValue values;
        /// Gradient (used during assembling)
        FieldGradient derivs;
        /// Time derivative (used during assembling)
        FieldValue dots;

        FieldInfo(const std::string & name, Int id, Int nc, Int k, Int dim, const Label & block) :
            name(name),
            id(id),
            fe(nullptr),
            block(block),
            nc(nc),
            k(k),
            values(nc),
            derivs(dim, nc),
            dots(nc)
        {
        }

        FieldInfo(const FieldInfo & other) = default;
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

        explicit AssemblyData(Int dim);
    } * asmbl;

    /// Functionals that must be evaluated before the weak form residual functionals
    /// associated with the PetscFormKey are evaluated
    std::map<PetscFormKey, std::vector<const ValueFunctional *>> sorted_res_functionals;
    /// Functionals that must be evaluated before the weak form Jacobian functionals
    /// associated with the PetscFormKey are evaluated
    std::map<PetscFormKey, std::vector<const ValueFunctional *>> sorted_jac_functionals;
};

/// Add residual statement for a field variable
///
/// @param fid Field ID
/// @param res_fn Integrand for the test function term
/// @param region Region name where this residual statement is active
template <>
inline void
FEProblemInterface::add_residual_block<WeakForm::F0>(Int fid,
                                                     AbstractResidualFunctional * res_fn,
                                                     const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_residual_block(WeakForm::F0, fid, res_fn, region);
}

/// Add residual statement for a field variable
///
/// @param fid Field ID
/// @param res_fn Integrand for the test function gradient term
/// @param region Region name where this residual statement is active
template <>
inline void
FEProblemInterface::add_residual_block<WeakForm::F1>(Int fid,
                                                     AbstractResidualFunctional * res_fn,
                                                     const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_residual_block(WeakForm::F1, fid, res_fn, region);
}

/// Add boundary residual statement for a field variable
///
/// @param fid Field ID
/// @param res_fn Integrand for the test function term
/// @param region Boundary name where this residual statement is active
template <>
inline void
FEProblemInterface::add_residual_block<WeakForm::BND_F0>(Int fid,
                                                         AbstractResidualFunctional * res_fn,
                                                         const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_bnd_residual_block(WeakForm::BND_F0, fid, res_fn, region);
}

/// Add boundary residual statement for a field variable
///
/// @param fid Field ID
/// @param res_fn Integrand for the test function gradient term
/// @param region Boundary name where this residual statement is active
template <>
inline void
FEProblemInterface::add_residual_block<WeakForm::BND_F1>(Int fid,
                                                         AbstractResidualFunctional * res_fn,
                                                         const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_bnd_residual_block(WeakForm::BND_F1, fid, res_fn, region);
}

// Jacobian blocks

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::G0>(Int fid,
                                                     Int gid,
                                                     JacobianFunc * jac_fn,
                                                     const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_jacobian_block(WeakForm::G0, fid, gid, jac_fn, region);
}

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::G1>(Int fid,
                                                     Int gid,
                                                     JacobianFunc * jac_fn,
                                                     const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_jacobian_block(WeakForm::G1, fid, gid, jac_fn, region);
}

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::G2>(Int fid,
                                                     Int gid,
                                                     JacobianFunc * jac_fn,
                                                     const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_jacobian_block(WeakForm::G2, fid, gid, jac_fn, region);
}

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::G3>(Int fid,
                                                     Int gid,
                                                     JacobianFunc * jac_fn,
                                                     const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_jacobian_block(WeakForm::G3, fid, gid, jac_fn, region);
}

// Preconditioner blocks

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::GP0>(Int fid,
                                                      Int gid,
                                                      JacobianFunc * jac_fn,
                                                      const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_jacobian_block(WeakForm::GP0, fid, gid, jac_fn, region);
}

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::GP1>(Int fid,
                                                      Int gid,
                                                      JacobianFunc * jac_fn,
                                                      const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_jacobian_block(WeakForm::GP1, fid, gid, jac_fn, region);
}

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::GP2>(Int fid,
                                                      Int gid,
                                                      JacobianFunc * jac_fn,
                                                      const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_jacobian_block(WeakForm::GP2, fid, gid, jac_fn, region);
}

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::GP3>(Int fid,
                                                      Int gid,
                                                      JacobianFunc * jac_fn,
                                                      const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_jacobian_block(WeakForm::GP3, fid, gid, jac_fn, region);
}

// Boundary jacobian blocks

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::BND_G0>(Int fid,
                                                         Int gid,
                                                         JacobianFunc * jac_fn,
                                                         const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_bnd_jacobian_block(WeakForm::BND_G0, fid, gid, jac_fn, region);
}

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::BND_G1>(Int fid,
                                                         Int gid,
                                                         JacobianFunc * jac_fn,
                                                         const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_bnd_jacobian_block(WeakForm::BND_G1, fid, gid, jac_fn, region);
}

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::BND_G2>(Int fid,
                                                         Int gid,
                                                         JacobianFunc * jac_fn,
                                                         const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_bnd_jacobian_block(WeakForm::BND_G2, fid, gid, jac_fn, region);
}

template <>
inline void
FEProblemInterface::add_jacobian_block<WeakForm::BND_G3>(Int fid,
                                                         Int gid,
                                                         JacobianFunc * jac_fn,
                                                         const std::string & region)
{
    CALL_STACK_MSG();
    add_weak_form_bnd_jacobian_block(WeakForm::BND_G3, fid, gid, jac_fn, region);
}

} // namespace godzilla
