// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Error.h"
#include "godzilla/Expected.h"
#include "godzilla/Parameters.h"
#include "godzilla/CallStack.h"
#include "godzilla/App.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Problem.h"
#include "godzilla/Logger.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/BoundaryCondition.h"
#include "godzilla/AuxiliaryField.h"
#include "godzilla/NaturalBC.h"
#include "godzilla/Exception.h"
#include "godzilla/Types.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Assert.h"
#include <set>

namespace godzilla {

DiscreteProblemInterface::DiscreteProblemInterface(Problem & problem, const Parameters & pars) :
    problem_(problem),
    unstr_mesh_(dynamic_ref_cast<UnstructuredMesh>(pars.get<Ref<Mesh>>("mesh"))),
    ds_(nullptr),
    dm_aux_(nullptr),
    ds_aux_(nullptr)
{
    CALL_STACK_MSG();
}

DiscreteProblemInterface::~DiscreteProblemInterface()
{
    CALL_STACK_MSG();
    DMDestroy(&this->dm_aux_);
}

Ref<Problem>
DiscreteProblemInterface::get_problem() const
{
    CALL_STACK_MSG();
    return this->problem_;
}

std::vector<Ref<InitialCondition>>
DiscreteProblemInterface::get_initial_conditions()
{
    CALL_STACK_MSG();
    return this->ics_;
}

std::vector<Ref<InitialCondition>>
DiscreteProblemInterface::get_aux_initial_conditions()
{
    CALL_STACK_MSG();
    return this->ics_aux_;
}

bool
DiscreteProblemInterface::has_initial_condition(String name) const
{
    CALL_STACK_MSG();
    const auto & it = this->ics_by_name_.find(name);
    return it != this->ics_by_name_.end();
}

Optional<Ref<InitialCondition>>
DiscreteProblemInterface::get_initial_condition(String name) const
{
    CALL_STACK_MSG();
    const auto & it = this->ics_by_name_.find(name);
    if (it != this->ics_by_name_.end())
        return it->second;
    else
        return {};
}

Expected<Ref<AuxiliaryField>, ErrorCode>
DiscreteProblemInterface::get_aux(String name) const
{
    CALL_STACK_MSG();
    const auto & it = this->auxs_by_name_.find(name);
    if (it != this->auxs_by_name_.end())
        return it->second;
    else
        return Unexpected(ErrorCode::NotFound);
}

Ref<UnstructuredMesh>
DiscreteProblemInterface::get_mesh() const
{
    CALL_STACK_MSG();
    return this->unstr_mesh_;
}

const Vector &
DiscreteProblemInterface::get_solution_vector_local() const
{
    CALL_STACK_MSG();
    return this->sln_;
}

Vector &
DiscreteProblemInterface::get_solution_vector_local()
{
    CALL_STACK_MSG();
    return this->sln_;
}

std::vector<Ref<BoundaryCondition>>
DiscreteProblemInterface::get_boundary_conditions() const
{
    CALL_STACK_MSG();
    std::vector<Ref<BoundaryCondition>> ret;
    ret.reserve(this->bcs_.size());
    for (auto & bc : this->bcs_) {
        ret.push_back(ref(*bc));
    }
    return ret;
}

std::vector<Ref<EssentialBC>>
DiscreteProblemInterface::get_essential_bcs() const
{
    CALL_STACK_MSG();
    return this->essential_bcs_;
}

std::vector<Ref<NaturalBC>>
DiscreteProblemInterface::get_natural_bcs() const
{
    CALL_STACK_MSG();
    return this->natural_bcs_;
}

void
DiscreteProblemInterface::distribute()
{
    CALL_STACK_MSG();
    auto part = this->problem_->get_partitioner();
    part.set_up();

    this->unstr_mesh_->set_partitioner(part);
    this->unstr_mesh_->distribute(this->problem_->get_partition_overlap());
}

void
DiscreteProblemInterface::init()
{
    CALL_STACK_MSG();
    set_up_ds();
    set_up_initial_conditions();
    set_up_boundary_conditions();
}

void
DiscreteProblemInterface::create()
{
    CALL_STACK_MSG();
    for (auto & ic : this->all_ics_)
        ic->create();
    for (auto & bc : this->bcs_)
        bc->create();
    for (auto & aux : this->auxs_)
        aux->create();
}

void
DiscreteProblemInterface::allocate_objects()
{
    CALL_STACK_MSG();
    this->sln_ = this->problem_->create_local_vector();
}

void
DiscreteProblemInterface::create_ds()
{
    CALL_STACK_MSG();
    auto dm = this->unstr_mesh_->get_dm();
    PETSC_CHECK(DMCreateDS(dm));
    PETSC_CHECK(DMGetDS(dm, &this->ds_));
}

PetscDS
DiscreteProblemInterface::get_ds() const
{
    CALL_STACK_MSG();
    return this->ds_;
}

void
DiscreteProblemInterface::check_initial_conditions(const std::vector<Ref<InitialCondition>> & ics,
                                                   const std::map<FieldID, Int> & field_comps)
{
    CALL_STACK_MSG();
    auto n_ics = ics.size();
    if (n_ics == 0)
        return;

    auto n_fields = field_comps.size();
    expect_true(n_ics == n_fields,
                fmt::format("Provided {} field(s), but {} initial condition(s).", n_fields, n_ics));
    // std::map<FieldID, Ref<InitialCondition>> ics_by_fields;
    std::map<FieldID, bool> ics_by_fields;
    for (auto & ic : ics) {
        auto fid = ic->get_field_id();
        if (fid == FieldID::INVALID)
            continue;
        const auto & it = ics_by_fields.find(fid);
        if (it == ics_by_fields.end()) {
            Int ic_nc = ic->get_num_components();
            Int field_nc = field_comps.at(fid);
            if (ic_nc == field_nc)
                ics_by_fields[fid] = true;
            else
                this->problem_->error(
                    "Initial condition '{}' operates on {} components, but is set on a field "
                    "with {} components.",
                    ic->get_name(),
                    ic_nc,
                    field_nc);
        }
        else
            // TODO: improve this error message
            this->problem_->error(
                "Initial condition '{}' is being applied to a field that already has an "
                "initial condition.",
                ic->get_name());
    }
}

void
DiscreteProblemInterface::set_up_initial_conditions()
{
    CALL_STACK_MSG();
    for (auto & ic : this->all_ics_) {
        auto field_name = ic->get_field_name();
        if (has_field_by_name(field_name))
            this->ics_.push_back(ref(*ic));
        else if (has_aux_field_by_name(field_name))
            this->ics_aux_.push_back(ref(*ic));
    }

    std::map<FieldID, Int> field_comps;
    for (auto & name : get_field_names()) {
        auto fid = get_field_id(name).value();
        field_comps[fid] = get_field_num_components(fid).value();
    }
    std::map<FieldID, Int> aux_field_comps;
    for (auto & name : get_aux_field_names()) {
        auto fid = get_aux_field_id(name).value();
        aux_field_comps[fid] = get_aux_field_num_components(fid).value();
    }
    check_initial_conditions(this->ics_, field_comps);
    check_initial_conditions(this->ics_aux_, aux_field_comps);
}

DM
DiscreteProblemInterface::get_dm_aux() const
{
    CALL_STACK_MSG();
    return this->dm_aux_;
}

PetscDS
DiscreteProblemInterface::get_ds_aux() const
{
    CALL_STACK_MSG();
    return this->ds_aux_;
}

Section
DiscreteProblemInterface::get_local_section_aux() const
{
    CALL_STACK_MSG();
    return this->section_aux_;
}

void
DiscreteProblemInterface::set_local_section_aux(const Section & section)
{
    CALL_STACK_MSG();
    this->section_aux_ = section;
}

void
DiscreteProblemInterface::set_up_auxiliary_dm(DM dm)
{
    CALL_STACK_MSG();
    if (get_num_aux_fields() == 0)
        return;

    this->dm_aux_ = clone(dm);

    create_aux_fields();

    PETSC_CHECK(DMCreateDS(this->dm_aux_));

    bool no_errors = true;
    for (auto & aux : this->auxs_) {
        try {
            auto fld_name = aux->get_field();
            auto fid = get_aux_field_id(fld_name);
            expect_true(fid.has_value(),
                        fmt::format("Auxiliary field '{}' does not exist", fld_name));
            auto aux_nc = aux->get_num_components();
            auto field_nc = get_aux_field_num_components(fid.value()).value();
            if (aux_nc == field_nc) {
                String region_name = aux->get_region();
                this->auxs_by_region_[region_name].push_back(ref(*aux));
            }
            else {
                no_errors = false;
                this->problem_->error(
                    "Auxiliary field '{}' has {} component(s), but is set on a field with {} "
                    "component(s).",
                    aux->get_name(),
                    aux_nc,
                    field_nc);
            }
        }
        catch (Exception & e) {
            no_errors = false;
            this->problem_->error("Auxiliary field '{}' does not exist.", aux->get_field());
        }
    }
    if (no_errors) {
        this->a_ = godzilla::create_local_vector(this->dm_aux_);
        PETSC_CHECK(DMSetAuxiliaryVec(dm, nullptr, 0, 0, this->a_));

        PETSC_CHECK(DMGetDS(this->dm_aux_, &this->ds_aux_));
        Section sa;
        PETSC_CHECK(DMGetLocalSection(this->dm_aux_, sa));
        sa.inc_reference();
        set_local_section_aux(sa);
    }
}

void
DiscreteProblemInterface::compute_global_aux_fields(DM dm,
                                                    const std::vector<Ref<AuxiliaryField>> & auxs,
                                                    Vector & a)
{
    CALL_STACK_MSG();
    auto n_auxs = get_num_aux_fields();
    std::vector<PetscFunc *> funcs(n_auxs, nullptr);
    std::vector<FunctionDelegate> delegates(n_auxs);
    for (const auto & aux : auxs) {
        auto fid = aux->get_field_id();
        funcs[fid.value()] = internal::invoke_function_delegate;
        delegates[fid.value()].bind(aux, &AuxiliaryField::evaluate);
    }
    std::vector<void *> contexts;
    for (auto & d : delegates) {
        if (d)
            contexts.push_back(&d);
        else
            contexts.push_back(nullptr);
    }
    PETSC_CHECK(DMProjectFunctionLocal(dm,
                                       get_problem()->get_time(),
                                       funcs.data(),
                                       contexts.data(),
                                       INSERT_ALL_VALUES,
                                       a));
}

void
DiscreteProblemInterface::compute_label_aux_fields(DM dm,
                                                   const Label & label,
                                                   const std::vector<Ref<AuxiliaryField>> & auxs,
                                                   Vector & a)
{
    CALL_STACK_MSG();
    auto n_auxs = get_num_aux_fields();
    std::vector<PetscFunc *> funcs(n_auxs, nullptr);
    std::vector<FunctionDelegate> delegates(n_auxs);
    for (const auto & aux : auxs) {
        auto fid = aux->get_field_id();
        funcs[fid.value()] = internal::invoke_function_delegate;
        delegates[fid.value()].bind(aux, &AuxiliaryField::evaluate);
    }
    std::vector<void *> contexts;
    for (auto & d : delegates) {
        if (d)
            contexts.push_back(&d);
        else
            contexts.push_back(nullptr);
    }
    auto ids = label.get_value_index_set();
    auto vals = ids.borrow_indices();
    PETSC_CHECK(DMProjectFunctionLabelLocal(dm,
                                            get_problem()->get_time(),
                                            label,
                                            vals.size(),
                                            vals.data(),
                                            PETSC_DETERMINE,
                                            nullptr,
                                            funcs.data(),
                                            contexts.data(),
                                            INSERT_ALL_VALUES,
                                            a));
}

void
DiscreteProblemInterface::compute_aux_fields()
{
    CALL_STACK_MSG();
    for (const auto & [region_name, auxs] : this->auxs_by_region_) {
        Label label;
        if (region_name.length() > 0)
            label = get_mesh()->get_label(region_name);

        if (label.is_null())
            compute_global_aux_fields(this->dm_aux_, auxs, this->a_);
        else
            compute_label_aux_fields(this->dm_aux_, label, auxs, this->a_);
    }
}

bool
DiscreteProblemInterface::check_bcs_boundaries()
{
    CALL_STACK_MSG();
    bool no_errors = true;
    for (auto & bc : this->bcs_) {
        auto boundaries = bc->get_boundary();
        for (auto & bnd_name : boundaries) {
            bool exists = this->unstr_mesh_->has_face_set(bnd_name) ||
                          this->unstr_mesh_->has_vertex_set(bnd_name);
            if (!exists) {
                no_errors = false;
                this->problem_->error(
                    "Boundary condition '{}' is set on boundary '{}' which does not exist in the "
                    "mesh.",
                    bc->get_name(),
                    bnd_name);
            }
        }
    }
    return no_errors;
}

void
DiscreteProblemInterface::set_up_boundary_conditions()
{
    CALL_STACK_MSG();
    bool no_errors = check_bcs_boundaries();
    if (no_errors)
        for (auto & bc : this->bcs_)
            bc->set_up();
}

void
DiscreteProblemInterface::set_initial_guess_from_ics()
{
    CALL_STACK_MSG();
    auto n_ics = this->ics_.size();
    std::vector<PetscFunc *> funcs(n_ics);
    std::vector<void *> contexts(n_ics);
    for (auto & ic : this->ics_) {
        auto fid = ic->get_field_id();
        funcs[fid.value()] = InitialCondition::invoke_delegate;
        contexts[fid.value()] = ic.operator->();
    }
    PETSC_CHECK(DMProjectFunction(this->unstr_mesh_->get_dm(),
                                  this->problem_->get_time(),
                                  funcs.data(),
                                  contexts.data(),
                                  INSERT_VALUES,
                                  this->problem_->get_solution_vector()));
}

void
DiscreteProblemInterface::set_up_initial_guess()
{
    CALL_STACK_MSG();
    if (!this->ics_.empty())
        set_initial_guess_from_ics();
}

Int
DiscreteProblemInterface::get_field_dof(Int point, FieldID fid) const
{
    CALL_STACK_MSG();
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->problem_->get_local_section(),
                                           point,
                                           fid.value(),
                                           &offset));
    return offset;
}

Int
DiscreteProblemInterface::get_aux_field_dof(Int point, FieldID fid) const
{
    CALL_STACK_MSG();
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->section_aux_, point, fid.value(), &offset));
    return offset;
}

const Vector &
DiscreteProblemInterface::get_aux_solution_vector_local() const
{
    CALL_STACK_MSG();
    return this->a_;
}

Vector &
DiscreteProblemInterface::get_aux_solution_vector_local()
{
    CALL_STACK_MSG();
    return this->a_;
}

void
DiscreteProblemInterface::add_boundary(DMBoundaryConditionType type,
                                       String name,
                                       const Label & label,
                                       Span<Int> ids,
                                       FieldID field,
                                       Span<Int> components,
                                       void (*bc_fn)(),
                                       void (*bc_fn_t)(),
                                       void * context)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscDSAddBoundary(this->ds_,
                                   type,
                                   name.c_str(),
                                   label,
                                   ids.size(),
                                   ids.data(),
                                   field.value(),
                                   components.size(),
                                   components.is_empty() ? nullptr : components.data(),
                                   bc_fn,
                                   bc_fn_t,
                                   context,
                                   nullptr));
}

void
DiscreteProblemInterface::update_aux_vector()
{
    CALL_STACK_MSG();
}

FieldID
DiscreteProblemInterface::get_next_id(const std::vector<FieldID> & ids) const
{
    CALL_STACK_MSG();
    std::set<Int> s;
    for (auto & id : ids)
        s.insert(id.value());
    for (Int id = 0; id < std::numeric_limits<Int>::max(); ++id)
        if (s.find(id) == s.end())
            return FieldID(id);
    return FieldID::INVALID;
}

} // namespace godzilla
