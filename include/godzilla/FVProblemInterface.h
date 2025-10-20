// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Delegate.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/NaturalRiemannBC.h"
#include "godzilla/Qtr.h"

namespace godzilla {

class AuxiliaryField;

/// Interface for finite volume problems
///
/// Any problem using PetscFV should inherit from this for unified API
class FVProblemInterface : public DiscreteProblemInterface {
public:
    using ComputeFluxDelegate =
        Delegate<void(const Real[], const Real[], const Scalar[], const Scalar[], Scalar[])>;

    FVProblemInterface(Problem * problem, const Parameters & pars);
    ~FVProblemInterface() override;

    Int get_num_fields() const override;
    std::vector<std::string> get_field_names() const override;
    const std::string & get_field_name(FieldID fid) const override;
    Int get_field_num_components(FieldID fid) const override;
    FieldID get_field_id(const std::string & name) const override;
    bool has_field_by_id(FieldID fid) const override;
    bool has_field_by_name(const std::string & name) const override;
    Order get_field_order(FieldID fid) const override;
    std::string get_field_component_name(FieldID fid, Int component) const override;
    void set_field_component_name(FieldID fid, Int component, const std::string & name) override;

    Int get_num_aux_fields() const override;
    std::vector<std::string> get_aux_field_names() const override;
    const std::string & get_aux_field_name(FieldID fid) const override;
    Int get_aux_field_num_components(FieldID fid) const override;
    FieldID get_aux_field_id(const std::string & name) const override;
    bool has_aux_field_by_id(FieldID fid) const override;
    bool has_aux_field_by_name(const std::string & name) const override;
    Order get_aux_field_order(FieldID fid) const override;
    std::string get_aux_field_component_name(FieldID fid, Int component) const override;
    void
    set_aux_field_component_name(FieldID fid, Int component, const std::string & name) override;

    /// Adds a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @param block The label this field is restricted to
    void add_field(FieldID id, const std::string & name, Int nc, const Label & block = Label());

    /// Adds a volumetric auxiliary field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @param block The label this field is restricted to
    /// @return ID of the new field
    FieldID add_aux_field(const std::string & name, Int nc, Order k, const Label & block = Label());

    /// Set a volumetric auxiliary field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    /// @param block The label this field is restricted to
    void set_aux_field(FieldID id,
                       const std::string & name,
                       Int nc,
                       Order k,
                       const Label & block = Label());

    /// Add essential boundary condition
    ///
    /// @param bc Boundary condition object parameters to add/create
    template <NaturalRiemannBCDerived OBJECT>
    OBJECT * add_boundary_condition(Parameters & pars);

protected:
    void init() override;
    void create() override;
    void set_up_ds() override;

    void create_aux_fields() override;

    /// Set up field variables
    virtual void set_up_fields() = 0;

    /// Set Riemann solver for the given field
    ///
    /// @tparam T C++ class type
    /// @param field Field ID
    /// @param instance Instance of class T
    /// @param method Member function in class T
    template <class T>
    void
    set_riemann_solver(
        FieldID field,
        T * instance,
        void (T::*method)(const Real[], const Real[], const Scalar[], const Scalar[], Scalar[]))
    {
        this->compute_flux_methods[field].bind(instance, method);
        auto ds = get_ds();
        PETSC_CHECK(PetscDSSetRiemannSolver(ds, field.value(), FVProblemInterface::compute_flux));
        PETSC_CHECK(PetscDSSetContext(ds, field.value(), &this->compute_flux_methods[field]));
    }

private:
    /// Field information
    struct FieldInfo {
        /// The name of the field
        std::string name;

        /// Field number
        FieldID id;

        /// Mesh support
        Label block;

        /// The number of components
        Int nc;

        /// The degree k of the space
        Order k;

        /// Component names
        std::vector<std::string> component_names;

        FieldInfo(const std::string & name, FieldID id, Int nc, Order k, const Label & block) :
            name(name),
            id(id),
            block(block),
            nc(nc),
            k(k)
        {
        }

        FieldInfo(const FieldInfo & other) = default;
    };

    /// Fields in the problem
    std::map<FieldID, FieldInfo> fields;

    /// Map from field name to field ID
    std::map<std::string, FieldID> fields_by_name;

    /// PETSc finite volume object
    PetscFV fvm;

    /// Auxiliary fields in the problem
    std::map<FieldID, FieldInfo> aux_fields;

    /// Map from auxiliary field name to auxiliary field ID
    std::map<std::string, FieldID> aux_fields_by_name;

    std::map<FieldID, PetscFE> aux_fe;

    std::map<FieldID, ComputeFluxDelegate> compute_flux_methods;

    std::vector<Qtr<NaturalRiemannBC>> riemann_bcs;

    static const std::string empty_name;

    static void compute_flux(Int dim,
                             Int nf,
                             const Real x[],
                             const Real n[],
                             const Scalar uL[],
                             const Scalar uR[],
                             Int n_consts,
                             const Scalar constants[],
                             Scalar flux[],
                             void * ctx);
};

template <NaturalRiemannBCDerived OBJECT>
OBJECT *
FVProblemInterface::add_boundary_condition(Parameters & pars)
{
    CALL_STACK_MSG();
    pars.set<DiscreteProblemInterface *>("_dpi", this);
    auto obj = Qtr<OBJECT>::alloc(pars);
    auto ptr = obj.get();
    this->riemann_bcs.push_back(std::move(obj));
    return ptr;
}

} // namespace godzilla
