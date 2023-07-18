#include "Godzilla.h"
#include "CallStack.h"
#include "FVProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "AuxiliaryField.h"
#include "Logger.h"
#include "Utils.h"
#include <cassert>

namespace godzilla {

const std::string FVProblemInterface::empty_name;

FVProblemInterface::FVProblemInterface(Problem * problem, const Parameters & params) :
    DiscreteProblemInterface(problem, params),
    fvm(nullptr)
{
    _F_;
}

FVProblemInterface::~FVProblemInterface()
{
    _F_;
    for (auto & kv : this->aux_fe) {
        auto & fe = kv.second;
        PetscFEDestroy(&fe);
    }
    this->a.destroy();
    this->sln.destroy();
}

void
FVProblemInterface::init()
{
    _F_;
    DiscreteProblemInterface::init();

    const MPI_Comm & comm = this->unstr_mesh->get_comm();
    Int dim = this->problem->get_dimension();
    PetscBool is_simplex = this->unstr_mesh->is_simplex() ? PETSC_TRUE : PETSC_FALSE;
    for (auto & it : this->aux_fields) {
        auto fi = it.second;
        PETSC_CHECK(PetscFECreateLagrange(comm,
                                          dim,
                                          fi.nc,
                                          is_simplex,
                                          fi.k,
                                          PETSC_DETERMINE,
                                          &this->aux_fe.at(fi.id)));
    }

    DM dm = this->unstr_mesh->get_dm();
    DM cdm = dm;
    while (cdm) {
        set_up_auxiliary_dm(cdm);

        PETSC_CHECK(DMCopyDisc(dm, cdm));
        PETSC_CHECK(DMGetCoarseDM(cdm, &cdm));
    }
}

Int
FVProblemInterface::get_num_fields() const
{
    _F_;
    // because there is one field with 'n' components which are the individual fields
    return 1;
}

std::vector<std::string>
FVProblemInterface::get_field_names() const
{
    _F_;
    std::vector<std::string> infos;
    infos.push_back(empty_name);
    return infos;
}

const std::string &
FVProblemInterface::get_field_name(Int fid) const
{
    _F_;
    if (fid == 0) {
        Int nc;
        PETSC_CHECK(PetscFVGetNumComponents(this->fvm, &nc));
        if (nc == 1)
            return this->fields.at(0).name;
        else
            return empty_name;
    }
    else
        error("Field with ID = '{}' does not exist.", fid);
}

Int
FVProblemInterface::get_field_num_components(Int fid) const
{
    _F_;
    if (fid == 0) {
        Int n_comps = 0;
        for (auto & it : this->fields)
            n_comps += it.second.nc;
        return n_comps;
    }
    else
        error("Field with ID = '{}' does not exist.", fid);
}

Int
FVProblemInterface::get_field_id(const std::string & name) const
{
    _F_;
    return 0;
}

bool
FVProblemInterface::has_field_by_id(Int fid) const
{
    _F_;
    if (fid == 0)
        return true;
    else
        return false;
}

bool
FVProblemInterface::has_field_by_name(const std::string & name) const
{
    _F_;
    return false;
}

Int
FVProblemInterface::get_field_order(Int fid) const
{
    _F_;
    if (fid == 0)
        return 0;
    else
        error("Multiple-field problems are not implemented");
}

std::string
FVProblemInterface::get_field_component_name(Int fid, Int component) const
{
    _F_;
    if (fid == 0) {
        const char * name;
        PETSC_CHECK(PetscFVGetComponentName(this->fvm, component, &name));
        return { name };
    }
    else
        error("Multiple-field problems are not implemented");
}

void
FVProblemInterface::set_field_component_name(Int fid, Int component, const std::string & name)
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end()) {
        if (it->second.nc > 1) {
            assert(component < it->second.nc && component < it->second.component_names.size());
            it->second.component_names[component] = name;
        }
        else
            error("Unable to set component name for single-component field");
    }
    else
        error("Field with ID = '{}' does not exist.", fid);
}

Int
FVProblemInterface::get_num_aux_fields() const
{
    _F_;
    return (Int) this->aux_fields.size();
}

std::vector<std::string>
FVProblemInterface::get_aux_field_names() const
{
    _F_;
    std::vector<std::string> names;
    names.reserve(this->aux_fields.size());
    for (const auto & it : this->aux_fields)
        names.push_back(it.second.name);
    return names;
}

const std::string &
FVProblemInterface::get_aux_field_name(Int fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.name;
    else
        error("Auxiliary field with ID = '{}' does not exist.", fid);
}

Int
FVProblemInterface::get_aux_field_num_components(Int fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.nc;
    else
        error("Auxiliary field with ID = '{}' does not exist.", fid);
}

Int
FVProblemInterface::get_aux_field_id(const std::string & name) const
{
    _F_;
    const auto & it = this->aux_fields_by_name.find(name);
    if (it != this->aux_fields_by_name.end())
        return it->second;
    else
        error("Auxiliary field '{}' does not exist. Typo?", name);
}

bool
FVProblemInterface::has_aux_field_by_id(Int fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    return it != this->aux_fields.end();
}

bool
FVProblemInterface::has_aux_field_by_name(const std::string & name) const
{
    _F_;
    const auto & it = this->aux_fields_by_name.find(name);
    return it != this->aux_fields_by_name.end();
}

Int
FVProblemInterface::get_aux_field_order(Int fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.k;
    else
        error("Auxiliary field with ID = '{}' does not exist.", fid);
}

std::string
FVProblemInterface::get_aux_field_component_name(Int fid, Int component) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end()) {
        const FieldInfo & fi = it->second;
        if (fi.nc == 1)
            return { "" };
        else {
            assert(component < it->second.nc && component < it->second.component_names.size());
            return it->second.component_names.at(component);
        }
    }
    else
        error("Auxiliary field with ID = '{}' does not exist.", fid);
}

void
FVProblemInterface::set_aux_field_component_name(Int fid, Int component, const std::string & name)
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end()) {
        if (it->second.nc > 1) {
            assert(component < it->second.nc && component < it->second.component_names.size());
            it->second.component_names[component] = name;
        }
        else
            error("Unable to set component name for single-component field");
    }
    else
        error("Auxiliary field with ID = '{}' does not exist.", fid);
}

const Vector &
FVProblemInterface::get_solution_vector_local() const
{
    _F_;
    build_local_solution_vector(this->sln);
    return this->sln;
}

const Vector &
FVProblemInterface::get_aux_solution_vector_local() const
{
    return this->a;
}

void
FVProblemInterface::add_field(Int id, const std::string & name, Int nc)
{
    _F_;
    auto it = this->fields.find(id);
    if (it == this->fields.end()) {
        FieldInfo fi(name, id, nc, 0);
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (Int i = 0; i < nc; i++)
                fi.component_names[i] = fmt::format("{:d}", i);
        }
        this->fields.emplace(id, fi);
        this->fields_by_name[name] = id;
    }
    else
        error("Cannot add field '{}' with ID = {}. ID already exists.", name, id);
}

Int
FVProblemInterface::add_aux_fe(const std::string & name, Int nc, Int k)
{
    _F_;
    std::vector<Int> keys = utils::map_keys(this->aux_fields);
    Int id = get_next_id(keys);
    set_aux_fe(id, name, nc, k);
    return id;
}

void
FVProblemInterface::set_aux_fe(Int id, const std::string & name, Int nc, Int k)
{
    _F_;
    auto it = this->aux_fields.find(id);
    if (it == this->aux_fields.end()) {
        FieldInfo fi(name, id, nc, k);
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (unsigned int i = 0; i < nc; i++)
                fi.component_names[i] = fmt::format("{:d}", i);
        }
        this->aux_fields.emplace(id, fi);
        this->aux_fields_by_name[name] = id;
        this->aux_fe[id] = nullptr;
    }
    else
        error("Cannot add auxiliary field '{}' with ID = {}. ID is already taken.", name, id);
}

void
FVProblemInterface::create()
{
    _F_;
    const_cast<UnstructuredMesh *>(this->unstr_mesh)->construct_ghost_cells();
    set_up_fields();
    DiscreteProblemInterface::create();
}

void
FVProblemInterface::allocate_objects()
{
    _F_;
    this->sln = this->problem->create_local_vector();
}

void
FVProblemInterface::set_up_ds()
{
    _F_;
    const MPI_Comm & comm = this->unstr_mesh->get_comm();

    PETSC_CHECK(PetscFVCreate(comm, &this->fvm));
    PETSC_CHECK(PetscFVSetType(this->fvm, PETSCFVUPWIND));

    Int n_comps = 0;
    for (auto & it : this->fields)
        n_comps += it.second.nc;
    PETSC_CHECK(PetscFVSetNumComponents(this->fvm, n_comps));

    PETSC_CHECK(PetscFVSetSpatialDimension(this->fvm, this->unstr_mesh->get_dimension()));

    for (Int id = 0, c = 0; id < this->fields.size(); id++) {
        const FieldInfo & fi = this->fields.at(id);
        if (fi.nc == 1) {
            PETSC_CHECK(PetscFVSetComponentName(this->fvm, c, fi.name.c_str()));
        }
        else {
            for (Int i = 0; i < fi.nc; i++) {
                std::string name = fmt::sprintf("%s_%s", fi.name, fi.component_names[i]);
                PETSC_CHECK(PetscFVSetComponentName(this->fvm, c + i, name.c_str()));
            }
        }
        c += fi.nc;
    }

    DM dm = this->unstr_mesh->get_dm();
    PETSC_CHECK(DMAddField(dm, nullptr, (PetscObject) this->fvm));
    PETSC_CHECK(DMCreateDS(dm));
    PETSC_CHECK(DMGetDS(dm, &this->ds));
}

void
FVProblemInterface::compute_global_aux_fields(DM dm,
                                              const std::vector<AuxiliaryField *> & auxs,
                                              Vector & a)
{
    _F_;
    auto n_auxs = this->aux_fields.size();
    std::vector<PetscFunc *> func(n_auxs, nullptr);
    std::vector<void *> ctxs(n_auxs, nullptr);

    for (const auto & aux : auxs) {
        Int fid = aux->get_field_id();
        func[fid] = aux->get_func();
        ctxs[fid] = aux->get_context();
    }

    PETSC_CHECK(DMProjectFunctionLocal(dm,
                                       this->problem->get_time(),
                                       func.data(),
                                       ctxs.data(),
                                       INSERT_ALL_VALUES,
                                       a));
}

void
FVProblemInterface::compute_label_aux_fields(DM dm,
                                             const Label & label,
                                             const std::vector<AuxiliaryField *> & auxs,
                                             Vector & a)
{
    _F_;
    auto n_auxs = this->aux_fields.size();
    std::vector<PetscFunc *> func(n_auxs, nullptr);
    std::vector<void *> ctxs(n_auxs, nullptr);

    for (const auto & aux : auxs) {
        Int fid = aux->get_field_id();
        func[fid] = aux->get_func();
        ctxs[fid] = aux->get_context();
    }

    auto ids = label.get_values();
    ids.get_indices();
    PETSC_CHECK(DMProjectFunctionLabelLocal(dm,
                                            this->problem->get_time(),
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
FVProblemInterface::compute_aux_fields()
{
    _F_;
    for (const auto & it : this->auxs_by_region) {
        const std::string & region_name = it.first;
        const std::vector<AuxiliaryField *> & auxs = it.second;
        Label label;
        if (region_name.length() > 0)
            label = this->unstr_mesh->get_label(region_name);

        if (label.is_null())
            compute_global_aux_fields(this->dm_aux, auxs, this->a);
        else
            compute_label_aux_fields(this->dm_aux, label, auxs, this->a);
    }
}

void
FVProblemInterface::set_up_auxiliary_dm(DM dm)
{
    _F_;
    if (this->aux_fields.empty())
        return;

    PETSC_CHECK(DMClone(dm, &this->dm_aux));

    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(
            DMSetField(this->dm_aux, fi.id, fi.block, (PetscObject) this->aux_fe.at(fi.id)));
    }

    PETSC_CHECK(DMCreateDS(this->dm_aux));

    bool no_errors = true;
    for (auto & aux : this->auxs) {
        Int fid = aux->get_field_id();
        if (has_aux_field_by_id(fid)) {
            Int aux_nc = aux->get_num_components();
            Int field_nc = this->aux_fields.at(fid).nc;
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
            this->logger->error("Auxiliary field '{}' is set on auxiliary field with ID '{}', but "
                                "such ID does not exist.",
                                aux->get_name(),
                                fid);
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
FVProblemInterface::add_boundary_essential(const std::string & name,
                                           const Label & label,
                                           const std::vector<Int> & ids,
                                           Int field,
                                           const std::vector<Int> & components,
                                           PetscFunc * fn,
                                           PetscFunc * fn_t,
                                           void * context) const
{
    _F_;
    error("Essential BCs are not supported for FV problems");
}

void
FVProblemInterface::add_boundary_natural(const std::string & name,
                                         const Label & label,
                                         const std::vector<Int> & ids,
                                         Int field,
                                         const std::vector<Int> & components,
                                         void * context) const
{
    _F_;
    error("Natural BCs are not supported for FV problems");
}

Int
FVProblemInterface::get_next_id(const std::vector<Int> & ids) const
{
    std::set<Int> s;
    for (auto & id : ids)
        s.insert(id);
    for (Int id = 0; id < std::numeric_limits<Int>::max(); id++)
        if (s.find(id) == s.end())
            return id;
    return -1;
}

} // namespace godzilla
