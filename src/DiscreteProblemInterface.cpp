// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

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

DiscreteProblemInterface::DiscreteProblemInterface(Problem * problem, const Parameters & pars) :
    problem(problem),
    unstr_mesh(dynamic_cast<UnstructuredMesh *>(pars.get<Mesh *>("mesh"))),
    logger(pars.get<App *>("app")->get_logger()),
    ds(nullptr),
    dm_aux(nullptr),
    ds_aux(nullptr)
{
    CALL_STACK_MSG();
    assert_true(this->problem != nullptr, "Problem is null");
    assert_true(this->unstr_mesh != nullptr, "Mesh must be UnstructuredMesh");
}

DiscreteProblemInterface::~DiscreteProblemInterface()
{
    CALL_STACK_MSG();
    DMDestroy(&this->dm_aux);
}

Problem *
DiscreteProblemInterface::get_problem() const
{
    CALL_STACK_MSG();
    return this->problem;
}

std::vector<InitialCondition *>
DiscreteProblemInterface::get_initial_conditions()
{
    CALL_STACK_MSG();
    return this->ics;
}

std::vector<InitialCondition *>
DiscreteProblemInterface::get_aux_initial_conditions()
{
    CALL_STACK_MSG();
    return this->ics_aux;
}

bool
DiscreteProblemInterface::has_initial_condition(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->ics_by_name.find(name);
    return it != this->ics_by_name.end();
}

Optional<InitialCondition *>
DiscreteProblemInterface::get_initial_condition(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->ics_by_name.find(name);
    if (it != this->ics_by_name.end())
        return it->second;
    else
        return {};
}

bool
DiscreteProblemInterface::has_aux(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->auxs_by_name.find(name);
    return it != this->auxs_by_name.end();
}

AuxiliaryField *
DiscreteProblemInterface::get_aux(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->auxs_by_name.find(name);
    if (it != this->auxs_by_name.end())
        return it->second;
    else
        return nullptr;
}

UnstructuredMesh *
DiscreteProblemInterface::get_mesh() const
{
    CALL_STACK_MSG();
    return this->unstr_mesh;
}

const Vector &
DiscreteProblemInterface::get_solution_vector_local() const
{
    CALL_STACK_MSG();
    return this->sln;
}

Vector &
DiscreteProblemInterface::get_solution_vector_local()
{
    CALL_STACK_MSG();
    return this->sln;
}

std::vector<BoundaryCondition *>
DiscreteProblemInterface::get_boundary_conditions() const
{
    CALL_STACK_MSG();
    std::vector<BoundaryCondition *> ret;
    ret.reserve(this->bcs.size());
    for (auto & bc : this->bcs) {
        ret.push_back(bc.get());
    }
    return ret;
}

std::vector<EssentialBC *>
DiscreteProblemInterface::get_essential_bcs() const
{
    CALL_STACK_MSG();
    return this->essential_bcs;
}

std::vector<NaturalBC *>
DiscreteProblemInterface::get_natural_bcs() const
{
    CALL_STACK_MSG();
    return this->natural_bcs;
}

void
DiscreteProblemInterface::distribute()
{
    CALL_STACK_MSG();
    auto part = this->problem->get_partitioner();
    part.set_up();

    this->unstr_mesh->set_partitioner(part);
    this->unstr_mesh->distribute(this->problem->get_partition_overlap());
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
    for (auto & ic : this->all_ics)
        ic->create();
    for (auto & bc : this->bcs)
        bc->create();
    for (auto & aux : this->auxs)
        aux->create();
}

void
DiscreteProblemInterface::allocate_objects()
{
    CALL_STACK_MSG();
    this->sln = this->problem->create_local_vector();
}

void
DiscreteProblemInterface::create_ds()
{
    CALL_STACK_MSG();
    auto dm = this->unstr_mesh->get_dm();
    PETSC_CHECK(DMCreateDS(dm));
    PETSC_CHECK(DMGetDS(dm, &this->ds));
}

PetscDS
DiscreteProblemInterface::get_ds() const
{
    CALL_STACK_MSG();
    return this->ds;
}

void
DiscreteProblemInterface::check_initial_conditions(const std::vector<InitialCondition *> & ics,
                                                   const std::map<FieldID, Int> & field_comps)
{
    CALL_STACK_MSG();
    auto n_ics = ics.size();
    if (n_ics == 0)
        return;

    auto n_fields = field_comps.size();
    if (n_ics == n_fields) {
        std::map<FieldID, InitialCondition *> ics_by_fields;
        for (auto & ic : ics) {
            auto fid = ic->get_field_id();
            if (fid == FieldID::INVALID)
                continue;
            const auto & it = ics_by_fields.find(fid);
            if (it == ics_by_fields.end()) {
                Int ic_nc = ic->get_num_components();
                Int field_nc = field_comps.at(fid);
                if (ic_nc == field_nc)
                    ics_by_fields[fid] = ic;
                else
                    this->logger->error("Initial condition '{}' operates on {} components, but is "
                                        "set on a field with {} components.",
                                        ic->get_name(),
                                        ic_nc,
                                        field_nc);
            }
            else
                // TODO: improve this error message
                this->logger->error(
                    "Initial condition '{}' is being applied to a field that already "
                    "has an initial condition.",
                    ic->get_name());
        }
    }
    else
        this->logger->error("Provided {} field(s), but {} initial condition(s).", n_fields, n_ics);
}

void
DiscreteProblemInterface::set_up_initial_conditions()
{
    CALL_STACK_MSG();
    for (auto & ic : this->all_ics) {
        auto field_name = ic->get_field_name();
        if (has_field_by_name(field_name))
            this->ics.push_back(ic.get());
        else if (has_aux_field_by_name(field_name))
            this->ics_aux.push_back(ic.get());
    }

    std::map<FieldID, Int> field_comps;
    for (auto & name : get_field_names()) {
        auto fid = get_field_id(name);
        field_comps[fid] = get_field_num_components(fid);
    }
    std::map<FieldID, Int> aux_field_comps;
    for (auto & name : get_aux_field_names()) {
        auto fid = get_aux_field_id(name);
        aux_field_comps[fid] = get_aux_field_num_components(fid);
    }
    check_initial_conditions(this->ics, field_comps);
    check_initial_conditions(this->ics_aux, aux_field_comps);
}

DM
DiscreteProblemInterface::get_dm_aux() const
{
    CALL_STACK_MSG();
    return this->dm_aux;
}

PetscDS
DiscreteProblemInterface::get_ds_aux() const
{
    CALL_STACK_MSG();
    return this->ds_aux;
}

Section
DiscreteProblemInterface::get_local_section_aux() const
{
    CALL_STACK_MSG();
    return this->section_aux;
}

void
DiscreteProblemInterface::set_local_section_aux(const Section & section)
{
    CALL_STACK_MSG();
    this->section_aux = section;
}

void
DiscreteProblemInterface::set_up_auxiliary_dm(DM dm)
{
    CALL_STACK_MSG();
    if (get_num_aux_fields() == 0)
        return;

    this->dm_aux = clone(dm);

    create_aux_fields();

    PETSC_CHECK(DMCreateDS(this->dm_aux));

    bool no_errors = true;
    for (auto & aux : this->auxs) {
        try {
            auto fid = aux->get_field_id();
            Int aux_nc = aux->get_num_components();
            Int field_nc = get_aux_field_num_components(fid);
            if (aux_nc == field_nc) {
                const std::string & region_name = aux->get_region();
                this->auxs_by_region[region_name].push_back(aux.get());
            }
            else {
                no_errors = false;
                this->logger->error("Auxiliary field '{}' has {} component(s), but is set on a "
                                    "field with {} component(s).",
                                    aux->get_name(),
                                    aux_nc,
                                    field_nc);
            }
        }
        catch (Exception & e) {
            no_errors = false;
            this->logger->error("Auxiliary field '{}' does not exist.", aux->get_field());
        }
    }
    if (no_errors) {
        this->a = godzilla::create_local_vector(this->dm_aux);
        PETSC_CHECK(DMSetAuxiliaryVec(dm, nullptr, 0, 0, this->a));

        PETSC_CHECK(DMGetDS(this->dm_aux, &this->ds_aux));
        Section sa;
        PETSC_CHECK(DMGetLocalSection(this->dm_aux, sa));
        sa.inc_reference();
        set_local_section_aux(sa);
    }
}

void
DiscreteProblemInterface::compute_global_aux_fields(DM dm,
                                                    const std::vector<AuxiliaryField *> & auxs,
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
                                                   const std::vector<AuxiliaryField *> & auxs,
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
    ids.get_indices();
    PETSC_CHECK(DMProjectFunctionLabelLocal(dm,
                                            get_problem()->get_time(),
                                            label,
                                            ids.get_size(),
                                            ids.data(),
                                            PETSC_DETERMINE,
                                            nullptr,
                                            funcs.data(),
                                            contexts.data(),
                                            INSERT_ALL_VALUES,
                                            a));
    ids.restore_indices();
}

void
DiscreteProblemInterface::compute_aux_fields()
{
    CALL_STACK_MSG();
    for (const auto & [region_name, auxs] : this->auxs_by_region) {
        Label label;
        if (!region_name.empty())
            label = get_mesh()->get_label(region_name);

        if (label.is_null())
            compute_global_aux_fields(this->dm_aux, auxs, this->a);
        else
            compute_label_aux_fields(this->dm_aux, label, auxs, this->a);
    }
}

bool
DiscreteProblemInterface::check_bcs_boundaries()
{
    CALL_STACK_MSG();
    bool no_errors = true;
    for (auto & bc : this->bcs) {
        auto boundaries = bc->get_boundary();
        for (auto & bnd_name : boundaries) {
            bool exists = this->unstr_mesh->has_face_set(bnd_name) ||
                          this->unstr_mesh->has_vertex_set(bnd_name);
            if (!exists) {
                no_errors = false;
                this->logger->error("Boundary condition '{}' is set on boundary '{}' which does "
                                    "not exist in the mesh.",
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
        for (auto & bc : this->bcs)
            bc->set_up();
}

void
DiscreteProblemInterface::set_initial_guess_from_ics()
{
    CALL_STACK_MSG();
    auto n_ics = this->ics.size();
    std::vector<PetscFunc *> funcs(n_ics);
    std::vector<void *> contexts(n_ics);
    for (auto & ic : this->ics) {
        auto fid = ic->get_field_id();
        funcs[fid.value()] = InitialCondition::invoke_delegate;
        contexts[fid.value()] = ic;
    }
    PETSC_CHECK(DMProjectFunction(this->unstr_mesh->get_dm(),
                                  this->problem->get_time(),
                                  funcs.data(),
                                  contexts.data(),
                                  INSERT_VALUES,
                                  this->problem->get_solution_vector()));
}

void
DiscreteProblemInterface::set_up_initial_guess()
{
    CALL_STACK_MSG();
    if (!this->ics.empty())
        set_initial_guess_from_ics();
}

Int
DiscreteProblemInterface::get_field_dof(Int point, FieldID fid) const
{
    CALL_STACK_MSG();
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->problem->get_local_section(),
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
    PETSC_CHECK(PetscSectionGetFieldOffset(this->section_aux, point, fid.value(), &offset));
    return offset;
}

const Vector &
DiscreteProblemInterface::get_aux_solution_vector_local() const
{
    CALL_STACK_MSG();
    return this->a;
}

Vector &
DiscreteProblemInterface::get_aux_solution_vector_local()
{
    CALL_STACK_MSG();
    return this->a;
}

void
DiscreteProblemInterface::add_boundary(DMBoundaryConditionType type,
                                       const std::string & name,
                                       const Label & label,
                                       const std::vector<Int> & ids,
                                       FieldID field,
                                       const std::vector<Int> & components,
                                       void (*bc_fn)(),
                                       void (*bc_fn_t)(),
                                       void * context)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   type,
                                   name.c_str(),
                                   label,
                                   ids.size(),
                                   ids.data(),
                                   field.value(),
                                   components.size(),
                                   components.empty() ? nullptr : components.data(),
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
