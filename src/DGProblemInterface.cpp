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
    DiscreteProblemInterface(problem, params)
{
    CALL_STACK_MSG();
}

DGProblemInterface::~DGProblemInterface()
{
    CALL_STACK_MSG();
    for (auto & kv : this->aux_fe) {
        auto & fe = kv.second;
        PetscFEDestroy(&fe);
    }
}

void
DGProblemInterface::init()
{
    CALL_STACK_MSG();
    DiscreteProblemInterface::init();

    auto dm = get_unstr_mesh()->get_dm();
    DM cdm = dm;
    while (cdm) {
        set_up_auxiliary_dm(cdm);

        PETSC_CHECK(DMCopyDisc(dm, cdm));
        PETSC_CHECK(DMGetCoarseDM(cdm, &cdm));
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

Int
DGProblemInterface::get_field_order(Int fid) const
{
    CALL_STACK_MSG();
    if (fid == 0)
        return 0;
    else
        error("Multiple-field problems are not implemented");
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
            for (unsigned int i = 0; i < nc; i++)
                fi.component_names[i] = fmt::format("{:d}", i);
        }
        this->fields.emplace(id, fi);
        this->fields_by_name[name] = id;
    }
    else
        throw Exception("Cannot add field '{}' with ID = {}. ID already exists.", name, id);
}

Int
DGProblemInterface::add_aux_fe(const std::string & name, Int nc, Int k, const Label & block)
{
    CALL_STACK_MSG();
    std::vector<Int> keys = utils::map_keys(this->aux_fields);
    Int id = get_next_id(keys);
    set_aux_fe(id, name, nc, k, block);
    return id;
}

void
DGProblemInterface::set_aux_fe(Int id, const std::string & name, Int nc, Int k, const Label & block)
{
    CALL_STACK_MSG();
    auto it = this->aux_fields.find(id);
    if (it == this->aux_fields.end()) {
        auto dim = get_problem()->get_dimension();
        FieldInfo fi(name, id, nc, k, block);
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (unsigned int i = 0; i < nc; i++)
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
    auto unstr_mesh = get_unstr_mesh();
    auto ct = unstr_mesh->get_cell_type(c);
    auto n_nodes = unstr_mesh->get_num_cell_nodes(ct);
    return n_nodes;
}

Int
DGProblemInterface::get_field_dof(Int elem, Int local_node, Int comp, Int fid) const
{
    CALL_STACK_MSG();
    auto section = get_problem()->get_local_section();
    Int offset = section.get_field_offset(elem, fid);
    // FIXME: works only for order = 1
    offset += (comp * get_num_nodes_per_elem(elem)) + local_node;
    return offset;
}

Int
DGProblemInterface::get_aux_field_dof(Int elem, Int local_node, Int comp, Int fid) const
{
    CALL_STACK_MSG();
    Int offset = get_local_section_aux().get_field_offset(elem, fid);
    // FIXME: works only for order = 1
    offset += (comp * get_num_nodes_per_elem(elem)) + local_node;
    return offset;
}

void
DGProblemInterface::create()
{
    CALL_STACK_MSG();
    set_up_fields();
    DiscreteProblemInterface::create();
}

void
DGProblemInterface::set_up_ds()
{
    CALL_STACK_MSG();
    create_section();
}

void
DGProblemInterface::create_section()
{
    CALL_STACK_MSG();
    auto comm = get_problem()->get_comm();
    auto unstr_mesh = get_unstr_mesh();
    auto dm = unstr_mesh->get_dm();
    PETSC_CHECK(DMSetNumFields(dm, 1));
    Section section;
    section.create(comm);
    section.set_num_fields(this->fields.size());
    for (auto & it : this->fields) {
        auto & fi = it.second;
        section.set_num_field_components(fi.id, fi.nc);
    }
    auto cell_range = unstr_mesh->get_cell_range();
    section.set_chart(cell_range.first(), cell_range.last());
    for (Int c = cell_range.first(); c < cell_range.last(); c++) {
        auto n_nodes = get_num_nodes_per_elem(c);
        Int n_dofs = 0;
        for (auto & it : this->fields) {
            auto & fi = it.second;
            auto n_field_dofs = fi.nc * n_nodes; // FIXME: work for only order = 1
            section.set_field_dof(c, fi.id, n_field_dofs);
            n_dofs += n_field_dofs;
        }
        section.set_dof(c, n_dofs);
    }
    set_up_section_constraint_dofs(section);
    section.set_up();
    set_up_section_constraint_indicies(section);
    get_problem()->set_local_section(section);
}

void
DGProblemInterface::set_up_section_constraint_dofs(Section & section)
{
    CALL_STACK_MSG();
    auto unstr_mesh = get_unstr_mesh();

    auto depth_label = unstr_mesh->get_depth_label();
    Int dim = unstr_mesh->get_dimension();
    IndexSet all_facets = depth_label.get_stratum(dim - 1);

    for (auto & bnd : get_essential_bcs()) {
        Int n_ced_dofs = bnd->get_components().size();
        auto fid = bnd->get_field_id();

        auto & bnd_names = bnd->get_boundary();
        for (auto & name : bnd_names) {
            auto label = unstr_mesh->get_face_set_label(name);
            auto ids = label.get_values();
            IndexSet points = label.get_stratum(ids[0]);
            if (!points.empty()) {
                IndexSet facets = IndexSet::intersect(all_facets, points);
                for (Int i = 0; i < facets.get_local_size(); i++) {
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
    auto unstr_mesh = get_unstr_mesh();

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
            if (!points.empty()) {
                IndexSet facets = IndexSet::intersect(all_facets, points);
                for (Int i = 0; i < facets.get_local_size(); i++) {
                    auto facet = facets(i);
                    auto support = unstr_mesh->get_support(facet);
                    assert(support.size() == 1);
                    auto fconn = unstr_mesh->get_connectivity(facet);
                    auto econn = unstr_mesh->get_connectivity(support[0]);

                    auto cell_id = support[0];
                    auto n_nodes_per_elem = econn.size();
                    std::vector<Int> indices;
                    for (Int j = 0; j < fconn.size(); j++) {
                        auto local_node_idx = fe::get_local_vertex_index(econn, fconn[j]);
                        for (Int k = 0; k < components.size(); k++) {
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
    Section section_aux;
    section_aux.create(comm);
    section_aux.set_num_fields(this->aux_fields.size());
    for (auto & it : this->aux_fields) {
        auto & fi = it.second;
        section_aux.set_num_field_components(fi.id, fi.nc);
    }

    auto unstr_mesh = get_unstr_mesh();
    auto cell_range = unstr_mesh->get_cell_range();
    section_aux.set_chart(cell_range.first(), cell_range.last());
    for (Int c = cell_range.first(); c < cell_range.last(); c++) {
        auto n_nodes = get_num_nodes_per_elem(c);
        int n_dofs = 0;
        for (auto & it : this->aux_fields) {
            auto & fi = it.second;
            // FIXME: this works for order = 1
            Int n_field_dofs = fi.nc * n_nodes;
            section_aux.set_field_dof(c, fi.id, n_field_dofs);
            n_dofs += n_field_dofs;
        }
        section_aux.set_dof(c, n_dofs);
    }
    section_aux.set_up();
    PETSC_CHECK(DMSetLocalSection(get_dm_aux(), section_aux));
}

void
DGProblemInterface::set_up_assembly_data_aux()
{
    CALL_STACK_MSG();
    // do nothing
}

void
DGProblemInterface::add_boundary_essential(const std::string & name,
                                           const std::string & boundary,
                                           Int field,
                                           const std::vector<Int> & components,
                                           PetscFunc * fn,
                                           PetscFunc * fn_t,
                                           void * context)
{
    CALL_STACK_MSG();
    // do nothing
}

void
DGProblemInterface::add_boundary_natural(const std::string & name,
                                         const std::string & boundary,
                                         Int field,
                                         const std::vector<Int> & components,
                                         void * context)
{
    CALL_STACK_MSG();
    // do nothing
}

void
DGProblemInterface::add_boundary_natural_riemann(const std::string & name,
                                                 const std::string & boundary,
                                                 Int field,
                                                 const std::vector<Int> & components,
                                                 PetscNaturalRiemannBCFunc * fn,
                                                 PetscNaturalRiemannBCFunc * fn_t,
                                                 void * context)
{
    CALL_STACK_MSG();
    throw Exception("Natural Riemann BCs are not supported for DG problems");
}

} // namespace godzilla
