#include "Godzilla.h"
#include "CallStack.h"
#include "FEProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "AuxiliaryField.h"
#include "Types.h"
#include "App.h"
#include "Logger.h"
#include <assert.h>

namespace godzilla {

FEProblemInterface::FEProblemInterface(Problem * problem, const Parameters & params) :
    DiscreteProblemInterface(problem, params),
    section(nullptr),
    qorder(PETSC_DETERMINE),
    dm_aux(nullptr),
    a(nullptr)
{
}

FEProblemInterface::~FEProblemInterface()
{
    _F_;
    for (auto & kv : this->fields) {
        FieldInfo & fi = kv.second;
        PetscFEDestroy(&fi.fe);
    }
    for (auto & kv : this->aux_fields) {
        FieldInfo & fi = kv.second;
        PetscFEDestroy(&fi.fe);
    }

    VecDestroy(&this->a);
    DMDestroy(&this->dm_aux);
}

void
FEProblemInterface::create()
{
    _F_;
    DiscreteProblemInterface::create();
    for (auto & aux : this->auxs)
        aux->create();
}

void
FEProblemInterface::init()
{
    _F_;
    DiscreteProblemInterface::init();

    DM dm = this->unstr_mesh->get_dm();
    PETSC_CHECK(DMGetLocalSection(dm, &this->section));
    DM cdm = dm;
    while (cdm) {
        set_up_auxiliary_dm(cdm);
        set_up_field_null_space(cdm);

        PETSC_CHECK(DMCopyDisc(dm, cdm));
        PETSC_CHECK(DMGetCoarseDM(cdm, &cdm));
    }
}

PetscInt
FEProblemInterface::get_num_fields() const
{
    _F_;
    return this->fields.size();
}

std::vector<std::string>
FEProblemInterface::get_field_names() const
{
    _F_;
    std::vector<std::string> infos;
    for (const auto & it : this->fields)
        infos.push_back(it.second.name);

    return infos;
}

const std::string &
FEProblemInterface::get_field_name(PetscInt fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.name;
    else
        error("Field with ID = '%d' does not exist.", fid);
}

PetscInt
FEProblemInterface::get_field_order(PetscInt fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.k;
    else
        error("Field with ID = '%d' does not exist.", fid);
}

PetscInt
FEProblemInterface::get_field_num_components(PetscInt fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.nc;
    else
        error("Field with ID = '%d' does not exist.", fid);
}

PetscInt
FEProblemInterface::get_field_id(const std::string & name) const
{
    _F_;
    const auto & it = this->fields_by_name.find(name);
    if (it != this->fields_by_name.end())
        return it->second;
    else
        error("Field '%s' does not exist. Typo?", name);
}

bool
FEProblemInterface::has_field_by_id(PetscInt fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    return it != this->fields.end();
}

bool
FEProblemInterface::has_field_by_name(const std::string & name) const
{
    _F_;
    const auto & it = this->fields_by_name.find(name);
    return it != this->fields_by_name.end();
}

std::string
FEProblemInterface::get_field_component_name(PetscInt fid, PetscInt component) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end()) {
        const FieldInfo & fi = it->second;
        if (fi.nc == 1)
            return std::string("");
        else {
            assert(component < it->second.nc && component < it->second.component_names.size());
            return it->second.component_names.at(component);
        }
    }
    else
        error("Field with ID = '%d' does not exist.", fid);
}

void
FEProblemInterface::set_field_component_name(PetscInt fid,
                                             PetscInt component,
                                             const std::string name)
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
        error("Field with ID = '%d' does not exist.", fid);
}

PetscInt
FEProblemInterface::get_field_dof(PetscInt point, PetscInt fid) const
{
    _F_;
    PetscInt offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->section, point, fid, &offset));
    return offset;
}

const std::string &
FEProblemInterface::get_aux_field_name(PetscInt fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.name;
    else
        error("Auxiliary field with ID = '%d' does not exist.", fid);
}

PetscInt
FEProblemInterface::get_aux_field_id(const std::string & name) const
{
    _F_;
    const auto & it = this->aux_fields_by_name.find(name);
    if (it != this->aux_fields_by_name.end())
        return it->second;
    else
        error("Auxiliary field '%s' does not exist. Typo?", name);
}

bool
FEProblemInterface::has_aux_field_by_id(PetscInt fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    return it != this->aux_fields.end();
}

bool
FEProblemInterface::has_aux_field_by_name(const std::string & name) const
{
    _F_;
    const auto & it = this->aux_fields_by_name.find(name);
    return it != this->aux_fields_by_name.end();
}

void
FEProblemInterface::add_fe(PetscInt id, const std::string & name, PetscInt nc, PetscInt k)
{
    _F_;
    auto it = this->fields.find(id);
    if (it == this->fields.end()) {
        FieldInfo fi = { name, id, nullptr, nullptr, nc, k, {} };
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (unsigned int i = 0; i < nc; i++)
                fi.component_names[i] = fmt::sprintf("%d", i);
        }
        this->fields[id] = fi;
        this->fields_by_name[name] = id;
    }
    else
        error("Cannot add field '%s' with ID = %d. ID already exists.", name, id);
}

void
FEProblemInterface::add_aux_fe(PetscInt id, const std::string & name, PetscInt nc, PetscInt k)
{
    _F_;
    auto it = this->aux_fields.find(id);
    if (it == this->aux_fields.end()) {
        FieldInfo fi = { name, id, nullptr, nullptr, nc, k };
        this->aux_fields[id] = fi;
        this->aux_fields_by_name[name] = id;
    }
    else
        error("Cannot add auxiliary field '%s' with ID = %d. ID is already taken.", name, id);
}

void
FEProblemInterface::add_auxiliary_field(AuxiliaryField * aux)
{
    _F_;
    this->auxs.push_back(aux);
}

void
FEProblemInterface::set_up_field_null_space(DM dm)
{
}

void
FEProblemInterface::create_fe(FieldInfo & fi)
{
    _F_;
    const MPI_Comm & comm = this->unstr_mesh->get_comm();
    PetscInt dim = this->problem->get_dimension();
    PetscBool is_simplex = this->unstr_mesh->is_simplex() ? PETSC_TRUE : PETSC_FALSE;

    PETSC_CHECK(PetscFECreateLagrange(comm, dim, fi.nc, is_simplex, fi.k, this->qorder, &fi.fe));
    PETSC_CHECK(PetscFESetName(fi.fe, fi.name.c_str()));
}

void
FEProblemInterface::set_up_ds()
{
    _F_;
    for (auto & it : this->fields)
        create_fe(it.second);
    for (auto & it : this->aux_fields)
        create_fe(it.second);

    set_up_quadrature();

    DM dm = this->unstr_mesh->get_dm();
    for (auto & it : this->fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(DMSetField(dm, fi.id, fi.block, (PetscObject) fi.fe));
    }
    PETSC_CHECK(DMCreateDS(dm));
    PETSC_CHECK(DMGetDS(dm, &this->ds));

    set_up_weak_form();
}

void
FEProblemInterface::set_up_quadrature()
{
    _F_;
    assert(this->fields.size() > 0);
    auto first = this->fields.begin();
    FieldInfo & first_fi = first->second;
    for (auto it = ++first; it != this->fields.end(); ++it) {
        FieldInfo & fi = it->second;
        PETSC_CHECK(PetscFECopyQuadrature(first_fi.fe, fi.fe));
    }
    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(PetscFECopyQuadrature(first_fi.fe, fi.fe));
    }
}

void
FEProblemInterface::compute_global_aux_fields(DM dm,
                                              const std::vector<AuxiliaryField *> & auxs,
                                              Vec a)
{
    _F_;
    PetscInt n_auxs = this->aux_fields.size();
    PetscFunc ** func = new PetscFunc *[n_auxs];
    void ** ctxs = new void *[n_auxs];
    for (PetscInt i = 0; i < n_auxs; i++) {
        func[i] = nullptr;
        ctxs[i] = nullptr;
    }

    for (const auto & aux : auxs) {
        PetscInt fid = aux->get_field_id();
        func[fid] = aux->get_func();
        ctxs[fid] = aux->get_context();
    }

    PETSC_CHECK(
        DMProjectFunctionLocal(dm, this->problem->get_time(), func, ctxs, INSERT_ALL_VALUES, a));

    delete[] func;
    delete[] ctxs;
}

void
FEProblemInterface::compute_label_aux_fields(DM dm,
                                             DMLabel label,
                                             const std::vector<AuxiliaryField *> & auxs,
                                             Vec a)
{
    _F_;
    PetscInt n_auxs = this->aux_fields.size();
    PetscFunc ** func = new PetscFunc *[n_auxs];
    void ** ctxs = new void *[n_auxs];
    for (PetscInt i = 0; i < n_auxs; i++) {
        func[i] = nullptr;
        ctxs[i] = nullptr;
    }

    for (const auto & aux : auxs) {
        PetscInt fid = aux->get_field_id();
        func[fid] = aux->get_func();
        ctxs[fid] = aux->get_context();
    }

    IS is;
    PETSC_CHECK(DMLabelGetValueIS(label, &is));
    PetscInt n_ids;
    PETSC_CHECK(ISGetSize(is, &n_ids));
    const PetscInt * ids;
    PETSC_CHECK(ISGetIndices(is, &ids));
    PETSC_CHECK(DMProjectFunctionLabelLocal(dm,
                                            this->problem->get_time(),
                                            label,
                                            n_ids,
                                            ids,
                                            PETSC_DETERMINE,
                                            nullptr,
                                            func,
                                            ctxs,
                                            INSERT_ALL_VALUES,
                                            a));
    PETSC_CHECK(ISRestoreIndices(is, &ids));
    PETSC_CHECK(ISDestroy(&is));

    delete[] func;
    delete[] ctxs;
}

void
FEProblemInterface::compute_aux_fields()
{
    _F_;
    for (const auto & it : this->auxs_by_region) {
        const std::string & region_name = it.first;
        const std::vector<AuxiliaryField *> & auxs = it.second;
        DMLabel label = nullptr;
        if (region_name.length() > 0)
            label = this->unstr_mesh->get_label(region_name);

        if (label == nullptr)
            compute_global_aux_fields(this->dm_aux, auxs, this->a);
        else
            compute_label_aux_fields(this->dm_aux, label, auxs, this->a);
    }
}

void
FEProblemInterface::set_up_auxiliary_dm(DM dm)
{
    _F_;
    if (this->aux_fields.size() == 0)
        return;

    PETSC_CHECK(DMClone(dm, &this->dm_aux));

    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(DMSetField(this->dm_aux, fi.id, fi.block, (PetscObject) fi.fe));
    }

    PETSC_CHECK(DMCreateDS(this->dm_aux));

    bool no_errors = true;
    for (auto & aux : this->auxs) {
        PetscInt fid = aux->get_field_id();
        if (has_aux_field_by_id(fid)) {
            PetscInt aux_nc = aux->get_num_components();
            PetscInt field_nc = this->aux_fields[fid].nc;
            if (aux_nc == field_nc) {
                const std::string & region_name = aux->get_region();
                this->auxs_by_region[region_name].push_back(aux);
            }
            else {
                no_errors = false;
                this->logger->error("Auxiliary field '%s' has %d component(s), but is set on a "
                                    "field with %d component(s).",
                                    aux->get_name(),
                                    aux_nc,
                                    field_nc);
            }
        }
        else {
            no_errors = false;
            this->logger->error("Auxiliary field '%s' is set on auxiliary field with ID '%d', but "
                                "such ID does not exist.",
                                aux->get_name(),
                                fid);
        }
    }
    if (no_errors) {
        PETSC_CHECK(DMCreateLocalVector(this->dm_aux, &this->a));
        PETSC_CHECK(DMSetAuxiliaryVec(dm, nullptr, 0, 0, this->a));
    }
}

} // namespace godzilla
