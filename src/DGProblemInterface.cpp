// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/DGProblemInterface.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/AuxiliaryField.h"
#include "godzilla/EssentialBC.h"
#include "godzilla/Logger.h"
#include "godzilla/Utils.h"
#include "godzilla/FEGeometry.h"
#include "godzilla/Exception.h"
#include <cassert>

namespace godzilla {

const std::string DGProblemInterface::empty_name;

DGProblemInterface::DGProblemInterface(Problem * problem, const Parameters & params) :
    DiscreteProblemInterface(problem, params),
    qorder(PETSC_DETERMINE)
{
    CALL_STACK_MSG();
}

DGProblemInterface::~DGProblemInterface()
{
    CALL_STACK_MSG();
    for (auto & [id, info] : this->fields) {
        PetscFEDestroy(&info.fe);
    }
    for (auto & kv : this->aux_fe) {
        auto & fe = kv.second;
        PetscFEDestroy(&fe);
    }
    this->section.destroy();
    this->section_aux.destroy();
}

void
DGProblemInterface::init()
{
    CALL_STACK_MSG();
    for (auto & it : this->fields)
        create_fe(it.second);
    DiscreteProblemInterface::init();

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
DGProblemInterface::get_num_fields() const
{
    CALL_STACK_MSG();
    return (Int) this->fields.size();
}

std::vector<std::string>
DGProblemInterface::get_field_names() const
{
    CALL_STACK_MSG();
    std::vector<std::string> infos;
    infos.reserve(this->fields.size());
    for (const auto & it : this->fields)
        infos.push_back(it.second.name);
    return infos;
}

const std::string &
DGProblemInterface::get_field_name(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.name;
    else
        throw Exception("Field with ID = '{}' does not exist.", fid);
}

Int
DGProblemInterface::get_field_num_components(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.nc;
    else
        throw Exception("Field with ID = '{}' does not exist.", fid);
}

Int
DGProblemInterface::get_field_id(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->fields_by_name.find(name);
    if (it != this->fields_by_name.end())
        return it->second;
    else
        throw Exception("Field '{}' does not exist. Typo?", name);
}

bool
DGProblemInterface::has_field_by_id(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->fields.find(fid);
    return it != this->fields.end();
}

bool
DGProblemInterface::has_field_by_name(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->fields_by_name.find(name);
    return it != this->fields_by_name.end();
}

PetscFE
DGProblemInterface::get_fe(Int fid) const
{
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end()) {
        const FieldInfo & fi = it->second;
        return fi.fe;
    }
    else
        throw Exception("Field with ID = '{}' does not exist.", fid);
}

Int
DGProblemInterface::get_field_order(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.k;
    else
        throw Exception("Field with ID = '{}' does not exist.", fid);
}

std::string
DGProblemInterface::get_field_component_name(Int fid, Int component) const
{
    CALL_STACK_MSG();
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end()) {
        const FieldInfo & fi = it->second;
        if (fi.nc == 1)
            return { "" };
        else {
            assert(component < it->second.nc && component < it->second.component_names.size());
            return it->second.component_names.at(component);
        }
    }
    else
        throw Exception("Field with ID = '{}' does not exist.", fid);
}

void
DGProblemInterface::set_field_component_name(Int fid, Int component, const std::string & name)
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
DGProblemInterface::get_num_aux_fields() const
{
    CALL_STACK_MSG();
    return (Int) this->aux_fields.size();
}

std::vector<std::string>
DGProblemInterface::get_aux_field_names() const
{
    CALL_STACK_MSG();
    std::vector<std::string> names;
    names.reserve(this->aux_fields.size());
    for (const auto & it : this->aux_fields)
        names.push_back(it.second.name);
    return names;
}

const std::string &
DGProblemInterface::get_aux_field_name(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.name;
    else
        throw Exception("Auxiliary field with ID = '{}' does not exist.", fid);
}

Int
DGProblemInterface::get_aux_field_num_components(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.nc;
    else
        throw Exception("Auxiliary field with ID = '{}' does not exist.", fid);
}

Int
DGProblemInterface::get_aux_field_id(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields_by_name.find(name);
    if (it != this->aux_fields_by_name.end())
        return it->second;
    else
        throw Exception("Auxiliary field '{}' does not exist. Typo?", name);
}

bool
DGProblemInterface::has_aux_field_by_id(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    return it != this->aux_fields.end();
}

bool
DGProblemInterface::has_aux_field_by_name(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields_by_name.find(name);
    return it != this->aux_fields_by_name.end();
}

Int
DGProblemInterface::get_aux_field_order(Int fid) const
{
    CALL_STACK_MSG();
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.k;
    else
        throw Exception("Auxiliary field with ID = '{}' does not exist.", fid);
}

std::string
DGProblemInterface::get_aux_field_component_name(Int fid, Int component) const
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
DGProblemInterface::set_aux_field_component_name(Int fid, Int component, const std::string & name)
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

Int
DGProblemInterface::add_field(const std::string & name, Int nc, Int k, const Label & block)
{
    CALL_STACK_MSG();
    std::vector<Int> keys = utils::map_keys(this->fields);
    Int id = get_next_id(keys);
    set_field(id, name, nc, k, block);
    return id;
}

void
DGProblemInterface::set_field(Int id, const std::string & name, Int nc, Int k, const Label & block)
{
    CALL_STACK_MSG();
    if (k != 1)
        throw Exception("DGProblemInterface works only with 1st order elements.");

    auto it = this->fields.find(id);
    if (it == this->fields.end()) {
        FieldInfo fi(name, id, nc, k, block);
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (unsigned int i = 0; i < nc; ++i)
                fi.component_names[i] = fmt::format("{:d}", i);
        }
        this->fields.emplace(id, fi);
        this->fields_by_name[name] = id;
    }
    else
        throw Exception("Cannot add field '{}' with ID = {}. ID already exists.", name, id);
}

Int
DGProblemInterface::add_aux_field(const std::string & name, Int nc, Int k, const Label & block)
{
    CALL_STACK_MSG();
    std::vector<Int> keys = utils::map_keys(this->aux_fields);
    Int id = get_next_id(keys);
    set_aux_field(id, name, nc, k, block);
    return id;
}

void
DGProblemInterface::set_aux_field(Int id,
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
    }
    else
        throw Exception("Cannot add auxiliary field '{}' with ID = {}. ID is already taken.",
                        name,
                        id);
}

Int
DGProblemInterface::get_num_nodes_per_elem(Int c) const
{
    auto unstr_mesh = get_mesh();
    auto ct = unstr_mesh->get_cell_type(c);
    auto n_nodes = UnstructuredMesh::get_num_cell_nodes(ct);
    return n_nodes;
}

Int
DGProblemInterface::get_field_dof(Int elem, Int local_node, Int fid) const
{
    CALL_STACK_MSG();
    auto offset = this->section.get_field_offset(elem, fid);
    // FIXME: works only for order = 1
    auto n_comps = get_field_num_components(fid);
    offset += n_comps * local_node;
    return offset;
}

Int
DGProblemInterface::get_aux_field_dof(Int elem, Int local_node, Int fid) const
{
    CALL_STACK_MSG();
    auto offset = get_local_section_aux().get_field_offset(elem, fid);
    // FIXME: works only for order = 1
    auto n_comps = get_aux_field_num_components(fid);
    offset += n_comps * local_node;
    return offset;
}

void
DGProblemInterface::create()
{
    CALL_STACK_MSG();
    set_up_fields();
    DiscreteProblemInterface::create();
    get_mesh()->localize_coordinates();
}

void
DGProblemInterface::set_up_ds()
{
    CALL_STACK_MSG();
    create_section();
    get_problem()->set_adjacency(0, true, false);
}

void
DGProblemInterface::create_section()
{
    CALL_STACK_MSG();
    auto comm = get_problem()->get_comm();
    auto unstr_mesh = get_mesh();
    auto dm = unstr_mesh->get_dm();
    PETSC_CHECK(DMSetNumFields(dm, 1));
    this->section.create(comm);
    this->section.set_num_fields(get_num_fields());
    for (auto & it : this->fields) {
        auto & fi = it.second;
        this->section.set_num_field_components(fi.id, fi.nc);
    }
    auto cell_range = unstr_mesh->get_cell_range();
    this->section.set_chart(cell_range.first(), cell_range.last());
    for (Int c = cell_range.first(); c < cell_range.last(); ++c) {
        auto n_nodes = get_num_nodes_per_elem(c);
        Int n_dofs = 0;
        for (auto & it : this->fields) {
            auto & fi = it.second;
            auto n_field_dofs = fi.nc * n_nodes; // FIXME: work for only order = 1
            this->section.set_field_dof(c, fi.id, n_field_dofs);
            n_dofs += n_field_dofs;
        }
        this->section.set_dof(c, n_dofs);
    }
    set_up_section_constraint_dofs(this->section);
    this->section.set_up();
    set_up_section_constraint_indicies(this->section);
    get_problem()->set_local_section(this->section);
}

void
DGProblemInterface::set_up_section_constraint_dofs(Section & section)
{
    CALL_STACK_MSG();
    auto unstr_mesh = get_mesh();

    auto depth_label = unstr_mesh->get_depth_label();
    auto dim = unstr_mesh->get_dimension();
    IndexSet all_facets = depth_label.get_stratum(dim - 1);

    for (auto & bnd : get_essential_bcs()) {
        auto n_ced_dofs = (Int) bnd->get_components().size();
        auto fid = bnd->get_field_id();

        auto & bnd_names = bnd->get_boundary();
        for (auto & name : bnd_names) {
            auto label = unstr_mesh->get_face_set_label(name);
            auto ids = label.get_values();
            IndexSet points = label.get_stratum(ids[0]);
            if (points) {
                IndexSet facets = IndexSet::intersect(all_facets, points);
                for (Int i = 0; i < facets.get_local_size(); ++i) {
                    auto facet = facets(i);
                    auto support = unstr_mesh->get_support(facet);
                    assert(support.size() == 1);
                    auto cell_id = support[0];
                    section.add_constraint_dof(cell_id, n_ced_dofs);
                    section.set_field_constraint_dof(cell_id, fid, n_ced_dofs);
                }
                points.destroy();
            }
        }
    }
}

void
DGProblemInterface::set_up_section_constraint_indicies(Section & section)
{
    CALL_STACK_MSG();
    auto unstr_mesh = get_mesh();

    auto depth_label = unstr_mesh->get_depth_label();
    Int dim = unstr_mesh->get_dimension();
    IndexSet all_facets = depth_label.get_stratum(dim - 1);

    for (auto & bnd : get_essential_bcs()) {
        auto components = bnd->get_components();
        auto fid = bnd->get_field_id();

        auto & bnd_names = bnd->get_boundary();
        for (auto & name : bnd_names) {
            auto label = unstr_mesh->get_face_set_label(name);
            auto ids = label.get_values();
            IndexSet points = label.get_stratum(ids[0]);
            if (points) {
                IndexSet facets = IndexSet::intersect(all_facets, points);
                for (Int i = 0; i < facets.get_local_size(); ++i) {
                    auto facet = facets(i);
                    auto support = unstr_mesh->get_support(facet);
                    assert(support.size() == 1);
                    auto fconn = unstr_mesh->get_connectivity(facet);
                    auto econn = unstr_mesh->get_connectivity(support[0]);

                    auto cell_id = support[0];
                    auto n_nodes_per_elem = (Int) econn.size();
                    std::vector<Int> indices;
                    for (std::size_t j = 0; j < fconn.size(); ++j) {
                        auto local_node_idx = fe::get_local_vertex_index(econn, fconn[j]);
                        for (std::size_t k = 0; k < components.size(); ++k) {
                            auto c = components[k];
                            indices.push_back((c * n_nodes_per_elem) + local_node_idx);
                        }
                    }
                    // TODO:
                    // section.set_constraint_indices(cell_id, indices);
                    section.set_field_constraint_indices(cell_id, fid, indices);
                }
                points.destroy();
            }
        }
    }
}

void
DGProblemInterface::create_aux_fields()
{
    CALL_STACK_MSG();
    auto comm = get_problem()->get_comm();
    this->section_aux.create(comm);
    this->section_aux.set_num_fields(get_num_aux_fields());
    for (auto & it : this->aux_fields) {
        auto & fi = it.second;
        this->section_aux.set_num_field_components(fi.id, fi.nc);
    }

    auto unstr_mesh = get_mesh();
    auto cell_range = unstr_mesh->get_cell_range();
    this->section_aux.set_chart(cell_range.first(), cell_range.last());
    for (Int c = cell_range.first(); c < cell_range.last(); ++c) {
        auto n_nodes = get_num_nodes_per_elem(c);
        Int n_dofs = 0;
        for (auto & it : this->aux_fields) {
            auto & fi = it.second;
            // FIXME: this works for order = 1
            auto n_field_dofs = fi.nc * n_nodes;
            this->section_aux.set_field_dof(c, fi.id, n_field_dofs);
            n_dofs += n_field_dofs;
        }
        this->section_aux.set_dof(c, n_dofs);
    }
    this->section_aux.set_up();
    PETSC_CHECK(DMSetLocalSection(get_dm_aux(), this->section_aux));
}

void
DGProblemInterface::set_up_weak_form()
{
}

void
DGProblemInterface::set_up_assembly_data_aux()
{
    CALL_STACK_MSG();
    // do nothing
}

void
DGProblemInterface::create_fe(FieldInfo & fi)
{
    CALL_STACK_MSG();
    auto comm = get_mesh()->get_comm();
    Int dim = get_problem()->get_dimension();
    PetscBool is_simplex = get_mesh()->is_simplex() ? PETSC_TRUE : PETSC_FALSE;
    PETSC_CHECK(PetscFECreateLagrange(comm, dim, fi.nc, is_simplex, fi.k, this->qorder, &fi.fe));
}

} // namespace godzilla
