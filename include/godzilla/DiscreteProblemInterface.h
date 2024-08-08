// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Label.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "godzilla/Section.h"
#include "godzilla/DenseMatrix.h"
#include "godzilla/DenseMatrixSymm.h"
#include "godzilla/EssentialBC.h"
#include "godzilla/NaturalRiemannBC.h"
#include "petscds.h"
#include "petscdmplex.h"

namespace godzilla {

class Parameters;
class Logger;
class InitialCondition;
class BoundaryCondition;
class AuxiliaryField;
class EssentialBC;
class NaturalBC;

/// Interface for discrete problems
///
class DiscreteProblemInterface {
private:
    using NaturalRiemannBCDelegate =
        Delegate<void(Real, const Real *, const Real *, const Scalar *, Scalar *)>;

    class EssentialBCDelegate : public Delegate<void(Real, const Real *, Scalar *)> {
    public:
        template <typename T>
        EssentialBCDelegate(T * t,
                            void (T::*method)(Real, const Real *, Scalar *),
                            void (T::*method_t)(Real, const Real *, Scalar *)) :
            Delegate<void(Real, const Real *, Scalar *)>(t, method)
        {
            this->fn_t = [=](Real time, const Real x[], Scalar u[]) {
                return (t->*method_t)(time, x, u);
            };
        }

        void
        invoke_t(Real time, const Real x[], Scalar u[])
        {
            return this->fn_t(time, x, u);
        }

    private:
        std::function<void(Real, const Real *, Scalar *)> fn_t;
    };

public:
    DiscreteProblemInterface(Problem * problem, const Parameters & params);
    virtual ~DiscreteProblemInterface();

    /// Get unstructured mesh associated with this problem
    [[nodiscard]] UnstructuredMesh * get_unstr_mesh() const;

    /// Get problem
    ///
    /// @return Problem this interface is part of
    [[nodiscard]] Problem * get_problem() const;

    /// Get number of fields
    ///
    /// @return The number of fields
    [[nodiscard]] virtual Int get_num_fields() const = 0;

    /// Get list of all field names
    ///
    /// @return List of field names
    [[nodiscard]] virtual std::vector<std::string> get_field_names() const = 0;

    /// Get field name
    ///
    /// @param fid Field ID
    [[nodiscard]] virtual const std::string & get_field_name(Int fid) const = 0;

    /// Get number of field components
    ///
    /// @param fid Field ID
    /// @return Number of components
    [[nodiscard]] virtual Int get_field_num_components(Int fid) const = 0;

    /// Get field ID
    ///
    /// @param name Field name
    /// @param Field ID
    [[nodiscard]] virtual Int get_field_id(const std::string & name) const = 0;

    /// Do we have field with specified ID
    ///
    /// @param fid The ID of the field
    /// @return True if the field exists, otherwise False
    [[nodiscard]] virtual bool has_field_by_id(Int fid) const = 0;

    /// Do we have field with specified name
    ///
    /// @param name The name of the field
    /// @return True if the field exists, otherwise False
    [[nodiscard]] virtual bool has_field_by_name(const std::string & name) const = 0;

    /// Get field order
    ///
    /// @param fid Field ID
    /// @return Field order
    [[nodiscard]] virtual Int get_field_order(Int fid) const = 0;

    /// Get component name of a field
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @return Component name
    [[nodiscard]] virtual std::string get_field_component_name(Int fid, Int component) const = 0;

    /// Set the name of a component of afield variable
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @param name Component name
    virtual void set_field_component_name(Int fid, Int component, const std::string & name) = 0;

    /// Get number of auxiliary fields
    ///
    /// @return The number of auxiliary fields
    [[nodiscard]] virtual Int get_num_aux_fields() const = 0;

    /// Get all auxiliary field names
    ///
    /// @return All auxiliary field names
    [[nodiscard]] virtual std::vector<std::string> get_aux_field_names() const = 0;

    /// Get auxiliary field name
    ///
    /// @param fid Auxiliary field ID
    /// @return Auxiliary field name
    [[nodiscard]] virtual const std::string & get_aux_field_name(Int fid) const = 0;

    /// Get number of auxiliary field components
    ///
    /// @param fid Auxiliary field ID
    /// @return Number of components
    [[nodiscard]] virtual Int get_aux_field_num_components(Int fid) const = 0;

    /// Get auxiliary field ID
    ///
    /// @param name Auxiliary field name
    /// @return Auxiliary field ID
    [[nodiscard]] virtual Int get_aux_field_id(const std::string & name) const = 0;

    /// Do we have auxiliary field with specified ID
    ///
    /// @param fid The ID of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    [[nodiscard]] virtual bool has_aux_field_by_id(Int fid) const = 0;

    /// Do we have auxiliary field with specified name
    ///
    /// @param name The name of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    [[nodiscard]] virtual bool has_aux_field_by_name(const std::string & name) const = 0;

    /// Get auxiliary field order
    ///
    /// @param fid Auxiliary field ID
    /// @return Auxiliary field order
    [[nodiscard]] virtual Int get_aux_field_order(Int fid) const = 0;

    /// Get component name of an auxiliary field
    ///
    /// @param fid Auxiliary field ID
    /// @param component Component index
    /// @return Component name
    [[nodiscard]] virtual std::string get_aux_field_component_name(Int fid,
                                                                   Int component) const = 0;

    /// Set the name of a component of an auxiliary field variable
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @param name Component name
    virtual void set_aux_field_component_name(Int fid, Int component, const std::string & name) = 0;

    /// Get list of all initial conditions
    ///
    /// @return List of all initial conditions
    const std::vector<InitialCondition *> & get_initial_conditions();

    /// Get list of all auxiliary initial conditions
    ///
    /// @return List of all auxiliary initial conditions
    const std::vector<InitialCondition *> & get_aux_initial_conditions();

    /// Add initial condition
    ///
    /// @param ic Initial condition object to add
    void add_initial_condition(InitialCondition * ic);

    /// Check if we have an initial condition object with a specified name
    ///
    /// @param name The name of the object
    /// @return True if the object exists, otherwise false
    [[nodiscard]] bool has_initial_condition(const std::string & name) const;

    /// Get initial condition object with a specified name
    ///
    /// @param name The name of the object
    /// @return Pointer to the initial condition object
    [[nodiscard]] InitialCondition * get_initial_condition(const std::string & name) const;

    /// Add essential boundary condition
    ///
    /// @param bc Boundary condition object to add
    void add_boundary_condition(BoundaryCondition * bc);

    /// Add auxiliary field
    ///
    /// @param aux Auxiliary field object to add
    void add_auxiliary_field(AuxiliaryField * aux);

    /// Check if we have an auxiliary object with a specified name
    ///
    /// @param name The name of the object
    /// @return True if the object exists, otherwise false
    [[nodiscard]] bool has_aux(const std::string & name) const;

    /// Get auxiliary object with a specified name
    ///
    /// @param name The name of the object
    /// @return Pointer to the auxiliary object
    [[nodiscard]] AuxiliaryField * get_aux(const std::string & name) const;

    /// Return the offset into an array or local Vec for the dof associated with the given point
    ///
    /// @param point Point
    /// @param fid Field ID
    /// @return The offset
    [[nodiscard]] Int get_field_dof(Int point, Int fid) const;

    /// Return the offset into an array of local auxiliary Vec for the dof associated with the given
    /// point
    ///
    /// @param point Point
    /// @param fid Field ID
    /// @return The offset
    [[nodiscard]] Int get_aux_field_dof(Int point, Int fid) const;

    /// Get local solution vector
    ///
    /// @return Local solution vector
    [[nodiscard]] const Vector & get_solution_vector_local() const;

    /// Get local solution vector
    ///
    /// @return Local solution vector
    Vector & get_solution_vector_local();

    /// Get local auxiliary solution vector
    ///
    /// @return Local auxiliary solution vector
    [[nodiscard]] const Vector & get_aux_solution_vector_local() const;

    /// Get local auxiliary solution vector
    ///
    /// @return Local auxiliary solution vector
    Vector & get_aux_solution_vector_local();

    void add_boundary(DMBoundaryConditionType type,
                      const std::string & name,
                      const Label & label,
                      const std::vector<Int> & ids,
                      Int field,
                      const std::vector<Int> & components,
                      void (*bc_fn)(),
                      void (*bc_fn_t)(),
                      void * context);

    template <class T>
    void
    add_boundary_essential(const std::string & name,
                           const std::string & boundary,
                           Int field,
                           const std::vector<Int> & components,
                           T * instance,
                           void (T::*method)(Real, const Real[], Scalar[]),
                           void (T::*method_t)(Real, const Real[], Scalar[]))
    {
        auto label = this->unstr_mesh->get_face_set_label(boundary);
        auto ids = label.get_values();
        auto delegate = new EssentialBCDelegate(instance, method, method_t);
        this->essential_bc_delegates.push_back(delegate);
        add_boundary(DM_BC_ESSENTIAL,
                     name,
                     label,
                     ids,
                     field,
                     components,
                     reinterpret_cast<void (*)()>(invoke_essential_bc_delegate),
                     method_t ? reinterpret_cast<void (*)()>(invoke_essential_bc_delegate_t)
                              : nullptr,
                     delegate);
    }

    void add_boundary_natural(const std::string & name,
                              const std::string & boundary,
                              Int field,
                              const std::vector<Int> & components);

    template <class T>
    void
    add_boundary_natural_riemann(
        const std::string & name,
        const std::string & boundary,
        Int field,
        const std::vector<Int> & components,
        T * instance,
        void (T::*method)(Real, const Real *, const Real *, const Scalar *, Scalar *))
    {
        auto label = this->unstr_mesh->get_face_set_label(boundary);
        auto ids = label.get_values();
        auto delegate = new NaturalRiemannBCDelegate(instance, method);
        this->natural_riemann_bc_delegates.push_back(delegate);
        add_boundary(DM_BC_NATURAL_RIEMANN,
                     name,
                     label,
                     ids,
                     field,
                     components,
                     reinterpret_cast<void (*)()>(invoke_natural_riemann_bc_delegate),
                     nullptr,
                     delegate);
    }

    template <Int N>
    void set_closure(const Vector & v,
                     Int point,
                     const DenseVector<Real, N> & vec,
                     InsertMode mode) const;

    template <Int N>
    void set_closure(const Matrix & A,
                     Int point,
                     const DenseMatrix<Real, N> & mat,
                     InsertMode mode) const;

    template <Int N>
    void set_closure(const Matrix & A,
                     Int point,
                     const DenseMatrixSymm<Real, N> & mat,
                     InsertMode mode) const;

    template <Int N>
    DenseVector<Real, N> get_closure(const Vector & v, Int point) const;

    template <Int N>
    void set_aux_closure(const Vector & v,
                         Int point,
                         const DenseVector<Real, N> & vec,
                         InsertMode mode) const;

    template <Int N>
    DenseVector<Real, N> get_aux_closure(const Vector & v, Int point) const;

    /// Return read/write access to a field on a point in local array
    template <typename T>
    T get_point_local_field_ref(Int point, Int field, Scalar * array) const;

    /// Build local solution vector for this problem
    virtual void compute_solution_vector_local() = 0;

protected:
    /// Get list of all boundary conditions
    [[nodiscard]] const std::vector<BoundaryCondition *> & get_boundary_conditions() const;
    /// Get list of all essential boundary conditions
    [[nodiscard]] const std::vector<EssentialBC *> & get_essential_bcs() const;
    /// Get list of all natural boundary conditions
    [[nodiscard]] const std::vector<NaturalBC *> & get_natural_bcs() const;
    /// Distribute the problem among processors for parallel execution
    void distribute();

    virtual void init();
    virtual void create();
    virtual void allocate_objects();
    /// Create underlying PetscDS object
    void create_ds();
    /// Get underlying PetscDS object
    [[nodiscard]] PetscDS get_ds() const;

    /// Check initial conditions
    void check_initial_conditions(const std::vector<InitialCondition *> & ics,
                                  const std::map<Int, Int> & field_comps);

    void set_initial_guess_from_ics();

    virtual void set_up_initial_guess();

    /// Set up initial conditions
    virtual void set_up_initial_conditions();

    /// Set up boundary conditions
    virtual void set_up_boundary_conditions();
    /// Check existence of boundaries used by BCs
    bool check_bcs_boundaries();

    [[nodiscard]] DM get_dm_aux() const;

    [[nodiscard]] PetscDS get_ds_aux() const;

    [[nodiscard]] Section get_local_section_aux() const;

    void set_local_section_aux(const Section & section);

    virtual void create_aux_fields() = 0;

    void set_up_auxiliary_dm(DM dm);

    void compute_global_aux_fields(DM dm, const std::vector<AuxiliaryField *> & auxs, Vector & a);

    void compute_label_aux_fields(DM dm,
                                  const Label & label,
                                  const std::vector<AuxiliaryField *> & auxs,
                                  Vector & a);

    /// Compute auxiliary fields
    ///
    /// @param dm DM for auxiliary fields
    void compute_aux_fields();

    /// Update auxiliary vector
    virtual void update_aux_vector();

    [[nodiscard]] Int get_next_id(const std::vector<Int> & ids) const;

    /// Setup weak form terms
    virtual void set_up_weak_form() = 0;

private:
    /// Set up discrete system
    virtual void set_up_ds() = 0;

    /// Problem this interface is part of
    Problem * problem;

    /// Unstructured mesh
    UnstructuredMesh * unstr_mesh;

    /// Logger object
    Logger * logger;

    /// Object that manages a discrete system
    PetscDS ds;

    /// All initial condition objects
    std::vector<InitialCondition *> all_ics;

    /// Initial conditions for primary fields
    std::vector<InitialCondition *> ics;

    /// Initial conditions for auxiliary fields
    std::vector<InitialCondition *> ics_aux;

    /// Map from aux object name to the aux object
    std::map<std::string, InitialCondition *> ics_by_name;

    /// List of all boundary conditions
    std::vector<BoundaryCondition *> bcs;

    /// List of essential boundary conditions
    std::vector<EssentialBC *> essential_bcs;

    /// List of delegates for essential BC
    std::vector<EssentialBCDelegate *> essential_bc_delegates;

    /// List of natural boundary conditions
    std::vector<NaturalBC *> natural_bcs;

    /// List of delegates for natural Riemann BC
    std::vector<NaturalRiemannBCDelegate *> natural_riemann_bc_delegates;

    /// List of auxiliary field objects
    std::vector<AuxiliaryField *> auxs;

    /// Map from aux object name to the aux object
    std::map<std::string, AuxiliaryField *> auxs_by_name;

    /// Map from region to list of auxiliary field objects
    std::map<std::string, std::vector<AuxiliaryField *>> auxs_by_region;

    /// Local solution vector
    Vector sln;

    /// DM for auxiliary fields
    DM dm_aux;

    /// Auxiliary section
    Section section_aux;

    /// Object that manages a discrete system for aux variables
    PetscDS ds_aux;

    /// Vector for auxiliary fields
    Vector a;

private:
    static ErrorCode invoke_essential_bc_delegate(Int dim,
                                                  Real time,
                                                  const Real x[],
                                                  Int nc,
                                                  Scalar u[],
                                                  void * ctx);
    static ErrorCode invoke_essential_bc_delegate_t(Int dim,
                                                    Real time,
                                                    const Real x[],
                                                    Int nc,
                                                    Scalar u[],
                                                    void * ctx);
    static ErrorCode invoke_natural_riemann_bc_delegate(Real time,
                                                        const Real * c,
                                                        const Real * n,
                                                        const Scalar * xI,
                                                        Scalar * xG,
                                                        void * ctx);
};

template <Int N>
void
DiscreteProblemInterface::set_closure(const Vector & v,
                                      Int point,
                                      const DenseVector<Real, N> & vec,
                                      InsertMode mode) const
{
    auto dm = this->unstr_mesh->get_dm();
    auto local_section = this->problem->get_local_section();
    PETSC_CHECK(DMPlexVecSetClosure(dm, local_section, v, point, vec.data(), mode));
}

template <Int N>
void
DiscreteProblemInterface::set_closure(const Matrix & A,
                                      Int point,
                                      const DenseMatrix<Real, N> & mat,
                                      InsertMode mode) const
{
    auto dm = this->unstr_mesh->get_dm();
    auto global_section = this->problem->get_global_section();
    auto local_section = this->problem->get_local_section();
    PETSC_CHECK(DMPlexMatSetClosure(dm, local_section, global_section, A, point, mat.data(), mode));
}

template <Int N>
void
DiscreteProblemInterface::set_closure(const Matrix & A,
                                      Int point,
                                      const DenseMatrixSymm<Real, N> & mat,
                                      InsertMode mode) const
{
    auto dm = this->unstr_mesh->get_dm();
    auto global_section = this->problem->get_global_section();
    auto local_section = this->problem->get_local_section();
    DenseMatrix<Real, N> m = mat;
    PETSC_CHECK(DMPlexMatSetClosure(dm, local_section, global_section, A, point, m.data(), mode));
}

template <Int N>
DenseVector<Real, N>
DiscreteProblemInterface::get_closure(const Vector & v, Int point) const
{
    auto dm = this->unstr_mesh->get_dm();
    Int sz = N;
    DenseVector<Real, N> vec;
    Real * data = vec.data();
    auto local_section = this->problem->get_local_section();
    PETSC_CHECK(DMPlexVecGetClosure(dm, local_section, v, point, &sz, &data));
    return vec;
}

template <Int N>
DenseVector<Real, N>
DiscreteProblemInterface::get_aux_closure(const Vector & v, Int point) const
{
    Int sz = N;
    DenseVector<Real, N> vec;
    Real * data = vec.data();
    PETSC_CHECK(DMPlexVecGetClosure(this->dm_aux, this->section_aux, v, point, &sz, &data));
    return vec;
}

template <Int N>
void
DiscreteProblemInterface::set_aux_closure(const Vector & v,
                                          Int point,
                                          const DenseVector<Real, N> & vec,
                                          InsertMode mode) const
{
    PETSC_CHECK(DMPlexVecSetClosure(this->dm_aux, this->section_aux, v, point, vec.data(), mode));
}

template <typename T>
T
DiscreteProblemInterface::get_point_local_field_ref(Int point, Int field, Scalar * array) const
{
    auto dm = this->unstr_mesh->get_dm();
    T var;
    PETSC_CHECK(DMPlexPointLocalFieldRef(dm, point, field, array, &var));
    return var;
}

} // namespace godzilla
