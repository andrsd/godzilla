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
#include "godzilla/EssentialBC.h"
#include <cassert>

namespace godzilla {

DiscreteProblemInterface::DiscreteProblemInterface(Problem * problem, const Parameters & params) :
    problem(problem),
    unstr_mesh(dynamic_cast<UnstructuredMesh *>(problem->get_mesh())),
    logger(params.get<App *>("_app")->get_logger()),
    ds(nullptr),
    dm_aux(nullptr),
    ds_aux(nullptr)
{
}

DiscreteProblemInterface::~DiscreteProblemInterface()
{
    _F_;
    this->sln.destroy();
    this->a.destroy();
    DMDestroy(&this->dm_aux);
}

Problem *
DiscreteProblemInterface::get_problem() const
{
    _F_;
    return this->problem;
}

const std::vector<InitialCondition *> &
DiscreteProblemInterface::get_initial_conditions()
{
    _F_;
    return this->ics;
}

const std::vector<InitialCondition *> &
DiscreteProblemInterface::get_aux_initial_conditions()
{
    _F_;
    return this->ics_aux;
}

void
DiscreteProblemInterface::add_initial_condition(InitialCondition * ic)
{
    _F_;
    const std::string & name = ic->get_name();
    auto it = this->ics_by_name.find(name);
    if (it == this->ics_by_name.end()) {
        this->all_ics.push_back(ic);
        this->ics_by_name[name] = ic;
    }
    else
        error("Cannot add initial condition object '{}'. Name already taken.", name);
}

bool
DiscreteProblemInterface::has_initial_condition(const std::string & name) const
{
    _F_;
    const auto & it = this->ics_by_name.find(name);
    return it != this->ics_by_name.end();
}

InitialCondition *
DiscreteProblemInterface::get_initial_condition(const std::string & name) const
{
    _F_;
    const auto & it = this->ics_by_name.find(name);
    if (it != this->ics_by_name.end())
        return it->second;
    else
        return nullptr;
}

void
DiscreteProblemInterface::add_boundary_condition(BoundaryCondition * bc)
{
    _F_;
    this->bcs.push_back(bc);
    auto nbc = dynamic_cast<NaturalBC *>(bc);
    if (nbc)
        this->natural_bcs.push_back(nbc);
    auto ebc = dynamic_cast<EssentialBC *>(bc);
    if (ebc)
        this->essential_bcs.push_back(ebc);
}

void
DiscreteProblemInterface::add_auxiliary_field(AuxiliaryField * aux)
{
    _F_;
    const std::string & name = aux->get_name();
    auto it = this->auxs_by_name.find(name);
    if (it == this->auxs_by_name.end()) {
        this->auxs.push_back(aux);
        this->auxs_by_name[name] = aux;
    }
    else
        error("Cannot add auxiliary object '{}'. Name already taken.", name);
}

bool
DiscreteProblemInterface::has_aux(const std::string & name) const
{
    _F_;
    const auto & it = this->auxs_by_name.find(name);
    return it != this->auxs_by_name.end();
}

AuxiliaryField *
DiscreteProblemInterface::get_aux(const std::string & name) const
{
    _F_;
    const auto & it = this->auxs_by_name.find(name);
    if (it != this->auxs_by_name.end())
        return it->second;
    else
        return nullptr;
}

UnstructuredMesh *
DiscreteProblemInterface::get_unstr_mesh() const
{
    _F_;
    return this->unstr_mesh;
}

const Vector &
DiscreteProblemInterface::get_solution_vector_local()
{
    _F_;
    build_local_solution_vector(this->sln);
    return this->sln;
}

std::vector<NaturalBC *>
DiscreteProblemInterface::get_natural_bcs() const
{
    return this->natural_bcs;
}

void
DiscreteProblemInterface::init()
{
    _F_;
    set_up_ds();
    set_up_initial_conditions();
    set_up_boundary_conditions();
    this->section = this->problem->get_local_section();
}

void
DiscreteProblemInterface::create()
{
    _F_;
    assert(this->problem != nullptr);
    assert(this->unstr_mesh != nullptr);

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
    _F_;
    this->sln = this->problem->create_local_vector();
}

void
DiscreteProblemInterface::create_ds()
{
    _F_;
    auto dm = this->unstr_mesh->get_dm();
    PETSC_CHECK(DMCreateDS(dm));
    PETSC_CHECK(DMGetDS(dm, &this->ds));
}

PetscDS
DiscreteProblemInterface::get_ds()
{
    _F_;
    return this->ds;
}

void
DiscreteProblemInterface::check_initial_conditions(const std::vector<InitialCondition *> & ics,
                                                   const std::map<Int, Int> & field_comps)
{
    _F_;
    auto n_ics = ics.size();
    if (n_ics == 0)
        return;

    Int n_fields = field_comps.size();
    if (n_ics == n_fields) {
        std::map<Int, InitialCondition *> ics_by_fields;
        for (auto & ic : ics) {
            Int fid = ic->get_field_id();
            if (fid == -1)
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
    _F_;
    for (auto & ic : this->all_ics) {
        auto field_name = ic->get_field_name();
        if (has_field_by_name(field_name))
            this->ics.push_back(ic);
        else if (has_aux_field_by_name(field_name))
            this->ics_aux.push_back(ic);
    }

    std::map<Int, Int> field_comps;
    for (auto & name : get_field_names()) {
        auto fid = get_field_id(name);
        field_comps[fid] = get_field_num_components(fid);
    }
    std::map<Int, Int> aux_field_comps;
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
    _F_;
    return this->dm_aux;
}

PetscDS
DiscreteProblemInterface::get_ds_aux() const
{
    _F_;
    return this->ds_aux;
}

void
DiscreteProblemInterface::set_up_auxiliary_dm(DM dm)
{
    _F_;
    if (get_num_aux_fields() == 0)
        return;

    PETSC_CHECK(DMClone(dm, &this->dm_aux));

    set_up_aux_fields();

    PETSC_CHECK(DMCreateDS(this->dm_aux));

    bool no_errors = true;
    for (auto & aux : this->auxs) {
        Int fid = aux->get_field_id();
        if (fid >= 0) {
            if (has_aux_field_by_id(fid)) {
                Int aux_nc = aux->get_num_components();
                Int field_nc = get_aux_field_num_components(fid);
                if (aux_nc == field_nc) {
                    const std::string & region_name = aux->get_region();
                    this->auxs_by_region[region_name].push_back(aux);
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
            else {
                no_errors = false;
                this->logger->error(
                    "Auxiliary field '{}' is set on auxiliary field with ID '{}', but "
                    "such ID does not exist.",
                    aux->get_name(),
                    fid);
            }
        }
    }
    if (no_errors) {
        Vec loc_a;
        PETSC_CHECK(DMCreateLocalVector(this->dm_aux, &loc_a));
        this->a = Vector(loc_a);
        PETSC_CHECK(DMSetAuxiliaryVec(dm, nullptr, 0, 0, this->a));

        PETSC_CHECK(DMGetDS(this->dm_aux, &this->ds_aux));
        PetscSection sa;
        PETSC_CHECK(DMGetLocalSection(this->dm_aux, &sa));
        this->section_aux = Section(sa);
    }
}

void
DiscreteProblemInterface::compute_global_aux_fields(DM dm,
                                                    const std::vector<AuxiliaryField *> & auxs,
                                                    Vector & a)
{
    _F_;
    auto n_auxs = get_num_aux_fields();
    std::vector<PetscFunc *> func(n_auxs, nullptr);
    std::vector<void *> ctxs(n_auxs, nullptr);

    for (const auto & aux : auxs) {
        Int fid = aux->get_field_id();
        func[fid] = aux->get_func();
        ctxs[fid] = aux->get_context();
    }

    PETSC_CHECK(DMProjectFunctionLocal(dm,
                                       get_problem()->get_time(),
                                       func.data(),
                                       ctxs.data(),
                                       INSERT_ALL_VALUES,
                                       a));
}

void
DiscreteProblemInterface::compute_label_aux_fields(DM dm,
                                                   const Label & label,
                                                   const std::vector<AuxiliaryField *> & auxs,
                                                   Vector & a)
{
    _F_;
    auto n_auxs = get_num_aux_fields();
    std::vector<PetscFunc *> func(n_auxs, nullptr);
    std::vector<void *> ctxs(n_auxs, nullptr);

    for (const auto & aux : auxs) {
        Int fid = aux->get_field_id();
        func[fid] = aux->get_func();
        ctxs[fid] = aux->get_context();
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
                                            func.data(),
                                            ctxs.data(),
                                            INSERT_ALL_VALUES,
                                            a));
    ids.restore_indices();
    ids.destroy();
}

void
DiscreteProblemInterface::compute_aux_fields()
{
    _F_;
    for (const auto & it : this->auxs_by_region) {
        const std::string & region_name = it.first;
        const std::vector<AuxiliaryField *> & auxs = it.second;
        Label label;
        if (region_name.length() > 0)
            label = get_unstr_mesh()->get_label(region_name);

        if (label.is_null())
            compute_global_aux_fields(this->dm_aux, auxs, this->a);
        else
            compute_label_aux_fields(this->dm_aux, label, auxs, this->a);
    }
}

void
DiscreteProblemInterface::set_up_boundary_conditions()
{
    _F_;
    /// TODO: refactor this into a method
    bool no_errors = true;
    for (auto & bc : this->bcs) {
        auto boundaries = bc->get_boundary();
        for (auto & bnd_name : boundaries) {
            bool exists = this->unstr_mesh->has_face_set(bnd_name);
            if (!exists) {
                no_errors = false;
                this->logger->error("Boundary condition '{}' is set on boundary '{}' which does "
                                    "not exist in the mesh.",
                                    bc->get_name(),
                                    bnd_name);
            }
        }
    }

    if (no_errors)
        for (auto & bc : this->bcs)
            bc->set_up();
}

void
DiscreteProblemInterface::set_initial_guess_from_ics()
{
    _F_;
    auto n_ics = this->ics.size();
    PetscFunc * ic_funcs[n_ics];
    void * ic_ctxs[n_ics];
    for (auto & ic : this->ics) {
        Int fid = ic->get_field_id();
        ic_funcs[fid] = ic->get_function();
        ic_ctxs[fid] = ic->get_context();
    }

    PETSC_CHECK(DMProjectFunction(this->unstr_mesh->get_dm(),
                                  this->problem->get_time(),
                                  ic_funcs,
                                  ic_ctxs,
                                  INSERT_VALUES,
                                  this->problem->get_solution_vector()));
}

void
DiscreteProblemInterface::set_up_initial_guess()
{
    _F_;
    if (!this->ics.empty())
        set_initial_guess_from_ics();
}

Int
DiscreteProblemInterface::get_field_dof(Int point, Int fid) const
{
    _F_;
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->section, point, fid, &offset));
    return offset;
}

Int
DiscreteProblemInterface::get_aux_field_dof(Int point, Int fid) const
{
    _F_;
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->section_aux, point, fid, &offset));
    return offset;
}

const Vector &
DiscreteProblemInterface::get_aux_solution_vector_local()
{
    _F_;
    return this->a;
}

void
DiscreteProblemInterface::add_boundary(DMBoundaryConditionType type,
                                       const std::string & name,
                                       const Label & label,
                                       const std::vector<Int> & ids,
                                       Int field,
                                       const std::vector<Int> & components,
                                       void (*bc_fn)(void),
                                       void (*bc_fn_t)(void),
                                       void * context)
{
    _F_;
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   type,
                                   name.c_str(),
                                   label,
                                   ids.size(),
                                   ids.data(),
                                   field,
                                   components.size(),
                                   components.size() == 0 ? nullptr : components.data(),
                                   bc_fn,
                                   bc_fn_t,
                                   context,
                                   nullptr));
}

void
DiscreteProblemInterface::add_boundary_essential(const std::string & name,
                                                 const std::string & boundary,
                                                 Int field,
                                                 const std::vector<Int> & components,
                                                 PetscFunc * fn,
                                                 PetscFunc * fn_t,
                                                 void * context)
{
    auto label = this->unstr_mesh->get_face_set_label(boundary);
    auto ids = label.get_values();
    add_boundary(DM_BC_ESSENTIAL,
                 name,
                 label,
                 ids,
                 field,
                 components,
                 reinterpret_cast<void (*)()>(fn),
                 reinterpret_cast<void (*)()>(fn_t),
                 context);
}

void
DiscreteProblemInterface::add_boundary_natural(const std::string & name,
                                               const std::string & boundary,
                                               Int field,
                                               const std::vector<Int> & components,
                                               void * context)
{
    auto label = this->unstr_mesh->get_face_set_label(boundary);
    auto ids = label.get_values();
    add_boundary(DM_BC_NATURAL, name, label, ids, field, components, nullptr, nullptr, context);
}

void
DiscreteProblemInterface::add_boundary_natural_riemann(const std::string & name,
                                                       const std::string & boundary,
                                                       Int field,
                                                       const std::vector<Int> & components,
                                                       PetscNaturalRiemannBCFunc * fn,
                                                       PetscNaturalRiemannBCFunc * fn_t,
                                                       void * context)
{
    auto label = this->unstr_mesh->get_face_set_label(boundary);
    auto ids = label.get_values();
    add_boundary(DM_BC_NATURAL_RIEMANN,
                 name,
                 label,
                 ids,
                 field,
                 components,
                 reinterpret_cast<void (*)()>(fn),
                 reinterpret_cast<void (*)()>(fn_t),
                 context);
}

void
DiscreteProblemInterface::update_aux_vector()
{
    _F_;
}

} // namespace godzilla
