// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/FVProblemInterface.h"
#include "godzilla/Expected.h"
#include "godzilla/Types.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/AuxiliaryField.h"
#include "godzilla/Formatters.h"
#include "godzilla/Utils.h"
#include "godzilla/Exception.h"
#include "godzilla/Assert.h"

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

const String FVProblemInterface::empty_name;

FVProblemInterface::FVProblemInterface(Problem & problem, const Parameters & pars) :
    DiscreteProblemInterface(problem, pars),
    fvm(nullptr)
{
    CALL_STACK_MSG();
}

FVProblemInterface::~FVProblemInterface()
{
    CALL_STACK_MSG();
    for (auto & [_, fe] : this->aux_fe)
        PetscFEDestroy(&fe);
    PETSC_CHECK(PetscFVDestroy(&this->fvm));
}

void
FVProblemInterface::init()
{
    CALL_STACK_MSG();
    DiscreteProblemInterface::init();

    auto comm = get_mesh()->get_comm();
    auto dim = get_problem()->get_dimension();
    PetscBool is_simplex = get_mesh()->is_simplex() ? PETSC_TRUE : PETSC_FALSE;
    for (auto & [_, info] : this->aux_fields)
        PETSC_CHECK(PetscFECreateLagrange(comm,
                                          dim,
                                          info.nc,
                                          is_simplex,
                                          info.k.value(),
                                          PETSC_DETERMINE,
                                          &this->aux_fe.at(info.id)));

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

std::vector<String>
FVProblemInterface::get_field_names() const
{
    CALL_STACK_MSG();
    std::vector<String> infos;
    infos.push_back(empty_name);
    return infos;
}

Expected<String, ErrorCode>
FVProblemInterface::get_field_name(FieldID fid) const
{
    CALL_STACK_MSG();
    if (fid == FieldID(0)) {
        if (this->fields.size() == 1)
            return this->fields.at(FieldID(0)).name;
        else
            return empty_name;
    }
    else
        return Unexpected(ErrorCode::NotFound);
}

Expected<Int, ErrorCode>
FVProblemInterface::get_field_num_components(FieldID fid) const
{
    CALL_STACK_MSG();
    if (fid == FieldID(0)) {
        Int n_comps = 0;
        for (auto & [_, info] : this->fields)
            n_comps += info.nc;
        return n_comps;
    }
    else
        return Unexpected(ErrorCode::NotFound);
}

Expected<FieldID, ErrorCode>
FVProblemInterface::get_field_id(String name) const
{
    CALL_STACK_MSG();
    return FieldID(0);
}

bool
FVProblemInterface::has_field_by_id(FieldID fid) const
{
    CALL_STACK_MSG();
    if (fid == FieldID(0))
        return true;
    else
        return false;
}

bool
FVProblemInterface::has_field_by_name(String name) const
{
    CALL_STACK_MSG();
    const auto & it = this->fields_by_name.find(name);
    return it != this->fields_by_name.end();
}

Expected<Order, ErrorCode>
FVProblemInterface::get_field_order(FieldID fid) const
{
    CALL_STACK_MSG();
    if (fid == FieldID(0))
        return Order(0);
    else
        throw NotImplementedException("Multiple-field problems are not implemented");
}

Expected<String, ErrorCode>
FVProblemInterface::get_field_component_name(FieldID fid, Int component) const
{
    CALL_STACK_MSG();
    if (fid == FieldID(0)) {
        const char * name;
        PETSC_CHECK(PetscFVGetComponentName(this->fvm, component, &name));
        return { name };
    }
    else
        throw NotImplementedException("Multiple-field problems are not implemented");
}

void
FVProblemInterface::set_field_component_name(FieldID fid, Int component, String name)
{
    CALL_STACK_MSG();
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end()) {
        if (it->second.nc > 1) {
            expect_true(component < it->second.nc &&
                            std::cmp_less(component, it->second.component_names.size()),
                        "Component index out of bounds");
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

std::vector<String>
FVProblemInterface::get_aux_field_names() const
{
    CALL_STACK_MSG();
    std::vector<String> names;
    names.reserve(this->aux_fields.size());
    for (const auto & [_, info] : this->aux_fields)
        names.push_back(info.name);
    return names;
}

Expected<String, ErrorCode>
FVProblemInterface::get_aux_field_name(FieldID fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.name;
    else
        return Unexpected(ErrorCode::NotFound);
}

Expected<Int, ErrorCode>
FVProblemInterface::get_aux_field_num_components(FieldID fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.nc;
    else
        return Unexpected(ErrorCode::NotFound);
}

Expected<FieldID, ErrorCode>
FVProblemInterface::get_aux_field_id(String name) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields_by_name.find(name);
    if (it != this->aux_fields_by_name.end())
        return it->second;
    else
        return Unexpected(ErrorCode::NotFound);
}

bool
FVProblemInterface::has_aux_field_by_id(FieldID fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    return it != this->aux_fields.end();
}

bool
FVProblemInterface::has_aux_field_by_name(String name) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields_by_name.find(name);
    return it != this->aux_fields_by_name.end();
}

Expected<Order, ErrorCode>
FVProblemInterface::get_aux_field_order(FieldID fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.k;
    else
        return Unexpected(ErrorCode::NotFound);
}

Expected<String, ErrorCode>
FVProblemInterface::get_aux_field_component_name(FieldID fid, Int component) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end()) {
        const FieldInfo & fi = it->second;
        if (fi.nc == 1)
            return { "" };
        else {
            expect_true(component < it->second.nc &&
                            std::cmp_less(component, it->second.component_names.size()),
                        "Component index out of bounds");
            return it->second.component_names.at(component);
        }
    }
    else
        return Unexpected(ErrorCode::NotFound);
}

void
FVProblemInterface::set_aux_field_component_name(FieldID fid, Int component, String name)
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end()) {
        if (it->second.nc > 1) {
            expect_true(component < it->second.nc &&
                            std::cmp_less(component, it->second.component_names.size()),
                        "Component index out of bounds");
            it->second.component_names[component] = name;
        }
        else
            throw Exception("Unable to set component name for single-component field");
    }
    else
        throw Exception("Auxiliary field with ID = '{}' does not exist.", fid);
}

void
FVProblemInterface::add_field(FieldID id, String name, Int nc, const Label & block)
{
    CALL_STACK_MSG();
    auto it = this->fields.find(id);
    if (it == this->fields.end()) {
        FieldInfo fi(name, id, nc, Order(0), block);
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (Int i = 0; i < nc; ++i)
                fi.component_names[i] = fmt::format("{:d}", i);
        }
        this->fields.emplace(id, fi);
        this->fields_by_name.emplace(name, id);
    }
    else
        throw Exception("Cannot add field '{}' with ID = {}. ID already exists.", name, id);
}

FieldID
FVProblemInterface::add_aux_field(String name, Int nc, Order k, const Label & block)
{
    CALL_STACK_MSG();
    auto keys = utils::map_keys(this->aux_fields);
    auto id = get_next_id(keys);
    set_aux_field(id, name, nc, k, block);
    return id;
}

void
FVProblemInterface::set_aux_field(FieldID id, String name, Int nc, Order k, const Label & block)
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
        this->aux_fields_by_name.emplace(name, id);
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
    if (get_mesh()->get_dimension() == 1_D)
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
    for (auto & [_, info] : this->fields)
        n_comps += info.nc;
    PETSC_CHECK(PetscFVSetNumComponents(this->fvm, n_comps));

    PETSC_CHECK(PetscFVSetSpatialDimension(this->fvm, get_mesh()->get_dimension()));

    for (std::size_t id = 0, c = 0; id < this->fields.size(); ++id) {
        const FieldInfo & fi = this->fields.at(FieldID(id));
        if (fi.nc == 1) {
            PETSC_CHECK(PetscFVSetComponentName(this->fvm, c, fi.name.c_str()));
        }
        else {
            for (Int i = 0; i < fi.nc; ++i) {
                String name = fmt::format("{}_{}", fi.name, fi.component_names[i]);
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
    for (auto & [id, fi] : this->aux_fields)
        PETSC_CHECK(
            DMSetField(dm_aux, fi.id.value(), fi.block, (PetscObject) this->aux_fe.at(fi.id)));
}

} // namespace godzilla
