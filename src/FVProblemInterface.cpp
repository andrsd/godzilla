// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/FVProblemInterface.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/AuxiliaryField.h"
#include "godzilla/Logger.h"
#include "godzilla/Utils.h"
#include "godzilla/Exception.h"
#include <cassert>

namespace godzilla {

void
FVProblemInterface::compute_flux(Int dim,
                                 Int nf,
                                 const Real x[],
                                 const Real n[],
                                 const Scalar u_l[],
                                 const Scalar u_r[],
                                 Int n_consts,
                                 const Scalar constants[],
                                 Scalar flux[],
                                 void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<ComputeFluxDelegate *>(ctx);
    method->invoke(x, n, u_l, u_r, flux);
}

const std::string FVProblemInterface::empty_name;

FVProblemInterface::FVProblemInterface(Problem * problem, const Parameters & params) :
    DiscreteProblemInterface(problem, params),
    fvm(nullptr)
{
    CALL_STACK_MSG();
}

FVProblemInterface::~FVProblemInterface()
{
    CALL_STACK_MSG();
    for (auto & kv : this->aux_fe) {
        auto & fe = kv.second;
        PetscFEDestroy(&fe);
    }
}

void
FVProblemInterface::init()
{
    CALL_STACK_MSG();
    DiscreteProblemInterface::init();

    auto comm = get_mesh()->get_comm();
    Int dim = get_problem()->get_dimension();
    PetscBool is_simplex = get_mesh()->is_simplex() ? PETSC_TRUE : PETSC_FALSE;
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

    auto dm = get_mesh()->get_dm();
    DM cdm = dm;
    while (cdm) {
        set_up_auxiliary_dm(cdm);

        PETSC_CHECK(DMCopyDisc(dm, cdm));
        PETSC_CHECK(DMGetCoarseDM(cdm, &cdm));
        if (cdm)
            PETSC_CHECK(DMLocalizeCoordinates(cdm));
    }
}

Int
FVProblemInterface::get_num_fields() const
{
    CALL_STACK_MSG();
    // because there is one field with 'n' components which are the individual fields
    return 1;
}

std::vector<std::string>
FVProblemInterface::get_field_names() const
{
    CALL_STACK_MSG();
    std::vector<std::string> infos;
    infos.push_back(empty_name);
    return infos;
}

const std::string &
FVProblemInterface::get_field_name(Int fid) const
{
    CALL_STACK_MSG();
    if (fid == 0) {
        if (this->fields.size() == 1)
            return this->fields.at(0).name;
        else
            return empty_name;
    }
    else
        throw Exception("Field with ID = '{}' does not exist.", fid);
}

Int
FVProblemInterface::get_field_num_components(Int fid) const
{
    CALL_STACK_MSG();
    if (fid == 0) {
        Int n_comps = 0;
        for (auto & it : this->fields)
            n_comps += it.second.nc;
        return n_comps;
    }
    else
        throw Exception("Field with ID = '{}' does not exist.", fid);
}

Int
FVProblemInterface::get_field_id(const std::string & name) const
{
    CALL_STACK_MSG();
    return 0;
}

bool
FVProblemInterface::has_field_by_id(Int fid) const
{
    CALL_STACK_MSG();
    if (fid == 0)
        return true;
    else
        return false;
}

bool
FVProblemInterface::has_field_by_name(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->fields_by_name.find(name);
    return it != this->fields_by_name.end();
}

Int
FVProblemInterface::get_field_order(Int fid) const
{
    CALL_STACK_MSG();
    if (fid == 0)
        return 0;
    else
        throw NotImplementedException("Multiple-field problems are not implemented");
}

std::string
FVProblemInterface::get_field_component_name(Int fid, Int component) const
{
    CALL_STACK_MSG();
    if (fid == 0) {
        const char * name;
        PETSC_CHECK(PetscFVGetComponentName(this->fvm, component, &name));
        return { name };
    }
    else
        throw NotImplementedException("Multiple-field problems are not implemented");
}

void
FVProblemInterface::set_field_component_name(Int fid, Int component, const std::string & name)
{
    CALL_STACK_MSG();
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end()) {
        if (it->second.nc > 1) {
            assert(component < it->second.nc && component < it->second.component_names.size());
            it->second.component_names[component] = name;
        }
        else
            throw Exception("Unable to set component name for single-component field");
    }
    else
        throw Exception("Field with ID = '{}' does not exist.", fid);
}

Int
FVProblemInterface::get_num_aux_fields() const
{
    CALL_STACK_MSG();
    return (Int) this->aux_fields.size();
}

std::vector<std::string>
FVProblemInterface::get_aux_field_names() const
{
    CALL_STACK_MSG();
    std::vector<std::string> names;
    names.reserve(this->aux_fields.size());
    for (const auto & it : this->aux_fields)
        names.push_back(it.second.name);
    return names;
}

const std::string &
FVProblemInterface::get_aux_field_name(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.name;
    else
        throw Exception("Auxiliary field with ID = '{}' does not exist.", fid);
}

Int
FVProblemInterface::get_aux_field_num_components(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.nc;
    else
        throw Exception("Auxiliary field with ID = '{}' does not exist.", fid);
}

Int
FVProblemInterface::get_aux_field_id(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields_by_name.find(name);
    if (it != this->aux_fields_by_name.end())
        return it->second;
    else
        throw Exception("Auxiliary field '{}' does not exist. Typo?", name);
}

bool
FVProblemInterface::has_aux_field_by_id(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    return it != this->aux_fields.end();
}

bool
FVProblemInterface::has_aux_field_by_name(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields_by_name.find(name);
    return it != this->aux_fields_by_name.end();
}

Int
FVProblemInterface::get_aux_field_order(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.k;
    else
        throw Exception("Auxiliary field with ID = '{}' does not exist.", fid);
}

std::string
FVProblemInterface::get_aux_field_component_name(Int fid, Int component) const
{
    CALL_STACK_MSG();
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
        throw Exception("Auxiliary field with ID = '{}' does not exist.", fid);
}

void
FVProblemInterface::set_aux_field_component_name(Int fid, Int component, const std::string & name)
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end()) {
        if (it->second.nc > 1) {
            assert(component < it->second.nc && component < it->second.component_names.size());
            it->second.component_names[component] = name;
        }
        else
            throw Exception("Unable to set component name for single-component field");
    }
    else
        throw Exception("Auxiliary field with ID = '{}' does not exist.", fid);
}

void
FVProblemInterface::add_field(Int id, const std::string & name, Int nc, const Label & block)
{
    CALL_STACK_MSG();
    auto it = this->fields.find(id);
    if (it == this->fields.end()) {
        FieldInfo fi(name, id, nc, 0, block);
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (Int i = 0; i < nc; ++i)
                fi.component_names[i] = fmt::format("{:d}", i);
        }
        this->fields.emplace(id, fi);
        this->fields_by_name[name] = id;
    }
    else
        throw Exception("Cannot add field '{}' with ID = {}. ID already exists.", name, id);
}

Int
FVProblemInterface::add_aux_field(const std::string & name, Int nc, Int k, const Label & block)
{
    CALL_STACK_MSG();
    std::vector<Int> keys = utils::map_keys(this->aux_fields);
    Int id = get_next_id(keys);
    set_aux_field(id, name, nc, k, block);
    return id;
}

void
FVProblemInterface::set_aux_field(Int id,
                                  const std::string & name,
                                  Int nc,
                                  Int k,
                                  const Label & block)
{
    CALL_STACK_MSG();
    auto it = this->aux_fields.find(id);
    if (it == this->aux_fields.end()) {
        FieldInfo fi(name, id, nc, k, block);
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (unsigned int i = 0; i < nc; ++i)
                fi.component_names[i] = fmt::format("{:d}", i);
        }
        this->aux_fields.emplace(id, fi);
        this->aux_fields_by_name[name] = id;
        this->aux_fe[id] = nullptr;
    }
    else
        throw Exception("Cannot add auxiliary field '{}' with ID = {}. ID is already taken.",
                        name,
                        id);
}

void
FVProblemInterface::create()
{
    CALL_STACK_MSG();
    set_up_fields();
    DiscreteProblemInterface::create();
    get_mesh()->construct_ghost_cells();
    get_mesh()->localize_coordinates();

#if PETSC_VERSION_GE(3, 21, 0)
    if (get_mesh()->get_dimension() == 1)
        throw Exception("FV in 1D is not possible due to a bug in PETSc. Use PETSc 3.20 instead.");
#endif
}

void
FVProblemInterface::set_up_ds()
{
    CALL_STACK_MSG();
    auto comm = get_mesh()->get_comm();

    PETSC_CHECK(PetscFVCreate(comm, &this->fvm));
    PETSC_CHECK(PetscFVSetType(this->fvm, PETSCFVUPWIND));

    Int n_comps = 0;
    for (auto & it : this->fields)
        n_comps += it.second.nc;
    PETSC_CHECK(PetscFVSetNumComponents(this->fvm, n_comps));

    PETSC_CHECK(PetscFVSetSpatialDimension(this->fvm, get_mesh()->get_dimension()));

    for (Int id = 0, c = 0; id < this->fields.size(); ++id) {
        const FieldInfo & fi = this->fields.at(id);
        if (fi.nc == 1) {
            PETSC_CHECK(PetscFVSetComponentName(this->fvm, c, fi.name.c_str()));
        }
        else {
            for (Int i = 0; i < fi.nc; ++i) {
                std::string name = fmt::format("{}_{}", fi.name, fi.component_names[i]);
                PETSC_CHECK(PetscFVSetComponentName(this->fvm, c + i, name.c_str()));
            }
        }
        c += fi.nc;
    }

    auto dm = get_mesh()->get_dm();
    PETSC_CHECK(DMAddField(dm, nullptr, (PetscObject) this->fvm));
    create_ds();

    set_up_weak_form();
}

void
FVProblemInterface::create_aux_fields()
{
    CALL_STACK_MSG();
    auto dm_aux = get_dm_aux();
    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(DMSetField(dm_aux, fi.id, fi.block, (PetscObject) this->aux_fe.at(fi.id)));
    }
}

} // namespace godzilla
