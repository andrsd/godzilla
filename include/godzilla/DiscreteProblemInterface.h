// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/InitialCondition.h"
#include "godzilla/Types.h"
#include "godzilla/Qtr.h"
#include "godzilla/Ptr.h"
#include "godzilla/Qtr.h"
#include "godzilla/Label.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "godzilla/Section.h"
#include "godzilla/DenseMatrix.h"
#include "godzilla/DenseMatrixSymm.h"
#include "godzilla/BoundaryCondition.h"
#include "godzilla/EssentialBC.h"
#include "petscds.h"
#include "petscdmplex.h"
#include <utility>

namespace godzilla {

class Parameters;
class Logger;
class InitialCondition;
class AuxiliaryField;
class NaturalBC;

/// Interface for discrete problems
///
class DiscreteProblemInterface {
public:
    DiscreteProblemInterface(Problem * problem, const Parameters & pars);
    virtual ~DiscreteProblemInterface();

    /// Get unstructured mesh associated with this problem
    UnstructuredMesh * get_mesh() const;

    /// Get problem
    ///
    /// @return Problem this interface is part of
    Problem * get_problem() const;

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
    virtual const std::string & get_field_name(FieldID fid) const = 0;

    /// Get number of field components
    ///
    /// @param fid Field ID
    /// @return Number of components
    virtual Int get_field_num_components(FieldID fid) const = 0;

    /// Get field ID
    ///
    /// @param name Field name
    /// @param Field ID
    virtual FieldID get_field_id(const std::string & name) const = 0;

    /// Do we have field with specified ID
    ///
    /// @param fid The ID of the field
    /// @return True if the field exists, otherwise False
    virtual bool has_field_by_id(FieldID fid) const = 0;

    /// Do we have field with specified name
    ///
    /// @param name The name of the field
    /// @return True if the field exists, otherwise False
    virtual bool has_field_by_name(const std::string & name) const = 0;

    /// Get field order
    ///
    /// @param fid Field ID
    /// @return Field order
    virtual Order get_field_order(FieldID fid) const = 0;

    /// Get component name of a field
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @return Component name
    virtual std::string get_field_component_name(FieldID fid, Int component) const = 0;

    /// Set the name of a component of afield variable
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @param name Component name
    virtual void set_field_component_name(FieldID fid, Int component, const std::string & name) = 0;

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
    virtual const std::string & get_aux_field_name(FieldID fid) const = 0;

    /// Get number of auxiliary field components
    ///
    /// @param fid Auxiliary field ID
    /// @return Number of components
    virtual Int get_aux_field_num_components(FieldID fid) const = 0;

    /// Get auxiliary field ID
    ///
    /// @param name Auxiliary field name
    /// @return Auxiliary field ID
    virtual FieldID get_aux_field_id(const std::string & name) const = 0;

    /// Do we have auxiliary field with specified ID
    ///
    /// @param fid The ID of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool has_aux_field_by_id(FieldID fid) const = 0;

    /// Do we have auxiliary field with specified name
    ///
    /// @param name The name of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool has_aux_field_by_name(const std::string & name) const = 0;

    /// Get auxiliary field order
    ///
    /// @param fid Auxiliary field ID
    /// @return Auxiliary field order
    virtual Order get_aux_field_order(FieldID fid) const = 0;

    /// Get component name of an auxiliary field
    ///
    /// @param fid Auxiliary field ID
    /// @param component Component index
    /// @return Component name
    virtual std::string get_aux_field_component_name(FieldID fid, Int component) const = 0;

    /// Set the name of a component of an auxiliary field variable
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @param name Component name
    virtual void
    set_aux_field_component_name(FieldID fid, Int component, const std::string & name) = 0;

    /// Get list of all initial conditions
    ///
    /// @return List of all initial conditions
    std::vector<InitialCondition *> get_initial_conditions();

    /// Get list of all auxiliary initial conditions
    ///
    /// @return List of all auxiliary initial conditions
    std::vector<InitialCondition *> get_aux_initial_conditions();

    /// Add initial condition
    ///
    /// @param pars Initial condition object parameters to add/create
    /// @return Pointer to the created initial condition object
    template <InitialConditionDerived OBJECT>
    OBJECT * add_initial_condition(Parameters & pars);

    /// Check if we have an initial condition object with a specified name
    ///
    /// @param name The name of the object
    /// @return True if the object exists, otherwise false
    bool has_initial_condition(const std::string & name) const;

    /// Get initial condition object with a specified name
    ///
    /// @param name The name of the object
    /// @return Pointer to the initial condition object
    InitialCondition * get_initial_condition(const std::string & name) const;

    /// Add essential boundary condition
    ///
    /// @param bc Boundary condition object parameters to add/create
    template <EssentialBCDerived OBJECT>
    OBJECT * add_boundary_condition(Parameters & pars);

    /// Add natual boundary condition
    ///
    /// @param par Boundary condition object parameters to add/create
    template <NaturalBCDerived OBJECT>
    OBJECT * add_boundary_condition(Parameters & pars);

    /// Add auxiliary field
    ///
    /// @param aux Auxiliary field object to add
    void add_auxiliary_field(AuxiliaryField * aux);

    /// Check if we have an auxiliary object with a specified name
    ///
    /// @param name The name of the object
    /// @return True if the object exists, otherwise false
    bool has_aux(const std::string & name) const;

    /// Get auxiliary object with a specified name
    ///
    /// @param name The name of the object
    /// @return Pointer to the auxiliary object
    AuxiliaryField * get_aux(const std::string & name) const;

    /// Return the offset into an array or local Vec for the dof associated with the given point
    ///
    /// @param point Point
    /// @param fid Field ID
    /// @return The offset
    Int get_field_dof(Int point, FieldID fid) const;

    /// Return the offset into an array of local auxiliary Vec for the dof associated with the given
    /// point
    ///
    /// @param point Point
    /// @param fid Field ID
    /// @return The offset
    Int get_aux_field_dof(Int point, FieldID fid) const;

    /// Get local solution vector
    ///
    /// @return Local solution vector
    const Vector & get_solution_vector_local() const;

    /// Get local solution vector
    ///
    /// @return Local solution vector
    Vector & get_solution_vector_local();

    /// Get local auxiliary solution vector
    ///
    /// @return Local auxiliary solution vector
    const Vector & get_aux_solution_vector_local() const;

    /// Get local auxiliary solution vector
    ///
    /// @return Local auxiliary solution vector
    Vector & get_aux_solution_vector_local();

    void add_boundary(DMBoundaryConditionType type,
                      const std::string & name,
                      const Label & label,
                      const std::vector<Int> & ids,
                      FieldID field,
                      const std::vector<Int> & components,
                      void (*bc_fn)(),
                      void (*bc_fn_t)(),
                      void * context);

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
    T get_point_local_field_ref(Int point, FieldID field, Scalar * array) const;

    /// Build local solution vector for this problem
    virtual void compute_solution_vector_local() = 0;

protected:
    /// Get list of all boundary conditions
    std::vector<BoundaryCondition *> get_boundary_conditions() const;
    /// Get list of all essential boundary conditions
    std::vector<EssentialBC *> get_essential_bcs() const;
    /// Get list of all natural boundary conditions
    std::vector<NaturalBC *> get_natural_bcs() const;
    /// Distribute the problem among processors for parallel execution
    void distribute();

    virtual void init();
    virtual void create();
    virtual void allocate_objects();
    /// Create underlying PetscDS object
    void create_ds();
    /// Get underlying PetscDS object
    PetscDS get_ds() const;

    /// Check initial conditions
    void check_initial_conditions(const std::vector<InitialCondition *> & ics,
                                  const std::map<FieldID, Int> & field_comps);

    void set_initial_guess_from_ics();

    virtual void set_up_initial_guess();

    /// Set up initial conditions
    virtual void set_up_initial_conditions();

    /// Set up boundary conditions
    virtual void set_up_boundary_conditions();
    /// Check existence of boundaries used by BCs
    bool check_bcs_boundaries();

    DM get_dm_aux() const;

    PetscDS get_ds_aux() const;

    Section get_local_section_aux() const;

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

    FieldID get_next_id(const std::vector<FieldID> & ids) const;

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
    std::vector<Qtr<InitialCondition>> all_ics;

    /// Initial conditions for primary fields
    std::vector<InitialCondition *> ics;

    /// Initial conditions for auxiliary fields
    std::vector<InitialCondition *> ics_aux;

    /// Map from aux object name to the aux object
    std::map<std::string, InitialCondition *> ics_by_name;

    /// List of all boundary conditions
    std::vector<BoundaryCondition *> bcs;

    /// List of essential boundary conditions
    std::vector<Qtr<EssentialBC>> essential_bcs;

    /// List of natural boundary conditions
    std::vector<Qtr<NaturalBC>> natural_bcs;

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
DiscreteProblemInterface::get_point_local_field_ref(Int point, FieldID field, Scalar * array) const
{
    auto dm = this->unstr_mesh->get_dm();
    T var;
    PETSC_CHECK(DMPlexPointLocalFieldRef(dm, point, field, array, &var));
    return var;
}

template <EssentialBCDerived OBJECT>
OBJECT *
DiscreteProblemInterface::add_boundary_condition(Parameters & pars)
{
    CALL_STACK_MSG();
    pars.set<DiscreteProblemInterface *>("_dpi", this);
    auto obj = Qtr<OBJECT>::alloc(pars);
    auto ptr = obj.get();
    this->bcs.push_back(ptr);
    this->essential_bcs.push_back(std::move(obj));
    return ptr;
}

template <NaturalBCDerived OBJECT>
OBJECT *
DiscreteProblemInterface::add_boundary_condition(Parameters & pars)
{
    CALL_STACK_MSG();
    pars.set<DiscreteProblemInterface *>("_dpi", this);
    auto obj = Qtr<OBJECT>::alloc(pars);
    auto ptr = obj.get();
    this->bcs.push_back(ptr);
    this->natural_bcs.push_back(std::move(obj));
    return ptr;
}

template <InitialConditionDerived OBJECT>
OBJECT *
DiscreteProblemInterface::add_initial_condition(Parameters & pars)
{
    CALL_STACK_MSG();
    pars.set<DiscreteProblemInterface *>("_dpi", this);
    auto obj = Qtr<OBJECT>::alloc(pars);
    const std::string & name = obj->get_name();
    auto it = this->ics_by_name.find(name);
    if (it == this->ics_by_name.end()) {
        auto ic = obj.get();
        this->ics_by_name[name] = ic;
        this->all_ics.push_back(std::move(obj));
        return ic;
    }
    else
        throw Exception("Cannot add initial condition object '{}'. Name already taken.", name);
}

} // namespace godzilla
