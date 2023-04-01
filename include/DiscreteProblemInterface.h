#pragma once

#include "petscds.h"
#include "Types.h"
#include "UnstructuredMesh.h"
#include "Vector.h"
#include "Matrix.h"
#include "Section.h"
#include "DenseMatrix.h"
#include "DenseMatrixSymm.h"

namespace godzilla {

class Parameters;
class Logger;
class UnstructuredMesh;
class Problem;
class InitialCondition;
class BoundaryCondition;

/// Interface for discrete problems
///
class DiscreteProblemInterface {
public:
    DiscreteProblemInterface(Problem * problem, const Parameters & params);
    virtual ~DiscreteProblemInterface() = default;

    /// Get the unstructured mesh
    ///
    /// @return Pointer to the unstructured mesh this problem is using
    virtual const UnstructuredMesh * get_mesh() const;

    /// Get number of fields
    ///
    /// @return The number of fields
    virtual Int get_num_fields() const = 0;

    /// Get list of all field names
    ///
    /// @return List of field names
    virtual std::vector<std::string> get_field_names() const = 0;

    /// Get field name
    ///
    /// @param fid Field ID
    virtual const std::string & get_field_name(Int fid) const = 0;

    /// Get number of field components
    ///
    /// @param fid Field ID
    /// @return Number of components
    virtual Int get_field_num_components(Int fid) const = 0;

    /// Get field ID
    ///
    /// @param name Field name
    /// @param Field ID
    virtual Int get_field_id(const std::string & name) const = 0;

    /// Do we have field with specified ID
    ///
    /// @param fid The ID of the field
    /// @return True if the field exists, otherwise False
    virtual bool has_field_by_id(Int fid) const = 0;

    /// Do we have field with specified name
    ///
    /// @param name The name of the field
    /// @return True if the field exists, otherwise False
    virtual bool has_field_by_name(const std::string & name) const = 0;

    /// Get field order
    ///
    /// @param fid Field ID
    /// @return Field order
    virtual Int get_field_order(Int fid) const = 0;

    /// Get component name of a field
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @return Component name
    virtual std::string get_field_component_name(Int fid, Int component) const = 0;

    /// Set the name of a component of afield variable
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @param name Component name
    virtual void set_field_component_name(Int fid, Int component, const std::string & name) = 0;

    /// Get number of auxiliary fields
    ///
    /// @return The number of auxiliary fields
    virtual Int get_num_aux_fields() const = 0;

    /// Get all auxiliary field names
    ///
    /// @return All auxiliary field names
    virtual std::vector<std::string> get_aux_field_names() const = 0;

    /// Get auxiliary field name
    ///
    /// @param fid Auxiliary field ID
    /// @return Auxiliary field name
    virtual const std::string & get_aux_field_name(Int fid) const = 0;

    /// Get number of auxiliary field components
    ///
    /// @param fid Auxiliary field ID
    /// @return Number of components
    virtual Int get_aux_field_num_components(Int fid) const = 0;

    /// Get auxiliary field ID
    ///
    /// @param name Auxiliary field name
    /// @return Auxiliary field ID
    virtual Int get_aux_field_id(const std::string & name) const = 0;

    /// Do we have auxiliary field with specified ID
    ///
    /// @param fid The ID of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool has_aux_field_by_id(Int fid) const = 0;

    /// Do we have auxiliary field with specified name
    ///
    /// @param name The name of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool has_aux_field_by_name(const std::string & name) const = 0;

    /// Get auxiliary field order
    ///
    /// @param fid Auxiliary field ID
    /// @return Auxiliary field order
    virtual Int get_aux_field_order(Int fid) const = 0;

    /// Get component name of an auxiliary field
    ///
    /// @param fid Auxiliary field ID
    /// @param component Component index
    /// @return Component name
    virtual std::string get_aux_field_component_name(Int fid, Int component) const = 0;

    /// Set the name of a component of an auxiliary field variable
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @param name Component name
    virtual void set_aux_field_component_name(Int fid, Int component, const std::string & name) = 0;

    /// Add initial condition
    ///
    /// @param ic Initial condition object to add
    virtual void add_initial_condition(InitialCondition * ic);

    /// Add essential boundary condition
    ///
    /// @param bc Boundary condition object to add
    virtual void add_boundary_condition(BoundaryCondition * bc);

    /// Return the offset into an array or local Vec for the dof associated with the given point
    ///
    /// @param point Point
    /// @param fid Field ID
    /// @return The offset
    virtual Int get_field_dof(Int point, Int fid) const;

    /// Return the offset into an array of local auxiliary Vec for the dof associated with the given
    /// point
    ///
    /// @param point Point
    /// @param fid Field ID
    /// @return The offset
    virtual Int get_aux_field_dof(Int point, Int fid) const = 0;

    /// Gets a local vector with the coordinates associated with this problem's mesh
    ///
    /// @return coordinate vector
    Vector get_coordinates_local() const;

    /// Get local solution vector
    ///
    /// @return Local solution vector
    virtual const Vector & get_solution_vector_local() const = 0;

    /// Get local auxiliary solution vector
    ///
    /// @return Local auxiliary solution vector
    virtual const Vector & get_aux_solution_vector_local(DMLabel region = nullptr) const = 0;

    virtual void add_boundary_essential(const std::string & name,
                                        DMLabel label,
                                        const std::vector<Int> & ids,
                                        Int field,
                                        const std::vector<Int> & components,
                                        PetscFunc * fn,
                                        PetscFunc * fn_t,
                                        void * context) const;
    virtual void add_boundary_natural(const std::string & name,
                                      DMLabel label,
                                      const std::vector<Int> & ids,
                                      Int field,
                                      const std::vector<Int> & components,
                                      void * context) const;
    virtual void add_boundary_natural_riemann(const std::string & name,
                                              DMLabel label,
                                              const std::vector<Int> & ids,
                                              Int field,
                                              const std::vector<Int> & components,
                                              PetscNaturalRiemannBCFunc * fn,
                                              PetscNaturalRiemannBCFunc * fn_t,
                                              void * context) const;

    template <Int N>
    void
    set_closure(Vector & v, Int point, const DenseVector<Real, N> & vec, InsertMode mode) const;

    template <Int N>
    void
    set_closure(Matrix & A, Int point, const DenseMatrix<Real, N> & mat, InsertMode mode) const;

    template <Int N>
    void
    set_closure(Matrix & A, Int point, const DenseMatrixSymm<Real, N> & mat, InsertMode mode) const;

    template <Int N>
    DenseVector<Real, N> get_closure(const Vector & v, Int point) const;

protected:
    virtual void init();
    virtual void create();
    /// Set up discrete system
    virtual void set_up_ds() = 0;

    void set_initial_guess_from_ics();

    virtual void set_up_initial_guess();

    /// Set up initial conditions
    virtual void set_up_initial_conditions();

    /// Set up boundary conditions
    virtual void set_up_boundary_conditions();

    /// Build local solution vector
    ///
    /// @param sln Global solution vector
    void build_local_solution_vector(const Vector & sln) const;

    /// Problem this interface is part of
    Problem * problem;

    /// Unstructured mesh
    const UnstructuredMesh * unstr_mesh;

    /// Logger object
    Logger * logger;

    /// Section
    Section section;

    /// Initial conditions in the problem
    std::vector<InitialCondition *> ics;

    /// List of boundary conditions
    std::vector<BoundaryCondition *> bcs;

    /// Object that manages a discrete system
    PetscDS ds;
};

template <Int N>
void
DiscreteProblemInterface::set_closure(Vector & v,
                                      Int point,
                                      const DenseVector<Real, N> & vec,
                                      InsertMode mode) const
{
    DM dm = this->unstr_mesh->get_dm();
    PETSC_CHECK(DMPlexVecSetClosure(dm, this->section, v, point, vec.get_data(), mode));
}

template <Int N>
void
DiscreteProblemInterface::set_closure(Matrix & A,
                                      Int point,
                                      const DenseMatrix<Real, N> & mat,
                                      InsertMode mode) const
{
    DM dm = this->unstr_mesh->get_dm();
    Section global_section = this->unstr_mesh->get_global_section();
    PETSC_CHECK(
        DMPlexMatSetClosure(dm, this->section, global_section, A, point, mat.get_data(), mode));
}

template <Int N>
void
DiscreteProblemInterface::set_closure(Matrix & A,
                                      Int point,
                                      const DenseMatrixSymm<Real, N> & mat,
                                      InsertMode mode) const
{
    DM dm = this->unstr_mesh->get_dm();
    Section global_section = this->unstr_mesh->get_global_section();
    DenseMatrix<Real, N> m = mat;
    PETSC_CHECK(
        DMPlexMatSetClosure(dm, this->section, global_section, A, point, m.get_data(), mode));
}

template <Int N>
DenseVector<Real, N>
DiscreteProblemInterface::get_closure(const Vector & v, Int point) const
{
    DM dm = this->unstr_mesh->get_dm();
    Int sz = N;
    DenseVector<Real, N> vec;
    Real * data = vec.get_data();
    PETSC_CHECK(DMPlexVecGetClosure(dm, this->section, v, point, &sz, &data));
    return vec;
}

} // namespace godzilla
