#include "Godzilla.h"
#include "FEProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "AuxiliaryField.h"
#include "NaturalBC.h"
#include "Types.h"
#include "App.h"
#include "Logger.h"
#include "PetscFEGodzilla.h"
#include "WeakForm.h"
#include "ResidualFunc.h"
#include "JacobianFunc.h"
#include "ValueFunctional.h"
#include "IndexSet.h"
#include "Utils.h"
#include "DependencyGraph.h"
#include <cassert>
#include <petsc/private/petscfeimpl.h>

namespace godzilla {

template <typename T>
void
add_functionals(DependencyGraph<const Functional *> & g,
                const std::map<std::string, const ValueFunctional *> & suppliers,
                const std::vector<T> & fnls)
{
    _F_;
    for (auto & f : fnls) {
        auto depends_on = f->get_dependent_values();
        for (auto & dep : depends_on) {
            auto jt = suppliers.find(dep);
            if (jt != suppliers.end())
                g.add_edge(f, jt->second);
            else
                error("Did not find any functional which would supply '{}'.", dep);
        }
    }
}

FEProblemInterface::AssemblyData::AssemblyData() :
    dim(-1),
    u(nullptr),
    u_t(nullptr),
    u_x(nullptr),
    xyz(dim),
    normals(dim),
    a(nullptr),
    a_x(nullptr),
    time(0.),
    u_t_shift(0.)
{
}

FEProblemInterface::FEProblemInterface(Problem * problem, const Parameters & params) :
    DiscreteProblemInterface(problem, params),
    DependencyEvaluator(),
    qorder(PETSC_DETERMINE),
    dm_aux(nullptr),
    section_aux(nullptr),
    a(nullptr),
    ds_aux(nullptr),
    wf(new WeakForm())
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

    this->a.destroy();
    DMDestroy(&this->dm_aux);

    this->sln.destroy();
}

void
FEProblemInterface::create()
{
    _F_;
    set_up_fields();
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
    DM cdm = dm;
    while (cdm) {
        set_up_auxiliary_dm(cdm);
        set_up_field_null_space(cdm);

        PETSC_CHECK(DMCopyDisc(dm, cdm));
        PETSC_CHECK(DMGetCoarseDM(cdm, &cdm));
    }

    set_up_weak_form();
    for (auto & bc : this->natural_bcs)
        bc->set_up_weak_form();

    sort_functionals();
}

void
FEProblemInterface::allocate_objects()
{
    this->sln = this->problem->create_local_vector();
}

Int
FEProblemInterface::get_num_fields() const
{
    _F_;
    return (Int) this->fields.size();
}

std::vector<std::string>
FEProblemInterface::get_field_names() const
{
    _F_;
    std::vector<std::string> infos;
    infos.reserve(this->fields.size());
    for (const auto & it : this->fields)
        infos.push_back(it.second.name);

    return infos;
}

const std::string &
FEProblemInterface::get_field_name(Int fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.name;
    else
        error("Field with ID = '{}' does not exist.", fid);
}

Int
FEProblemInterface::get_field_order(Int fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.k;
    else
        error("Field with ID = '{}' does not exist.", fid);
}

Int
FEProblemInterface::get_field_num_components(Int fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.nc;
    else
        error("Field with ID = '{}' does not exist.", fid);
}

Int
FEProblemInterface::get_field_id(const std::string & name) const
{
    _F_;
    const auto & it = this->fields_by_name.find(name);
    if (it != this->fields_by_name.end())
        return it->second;
    else
        error("Field '{}' does not exist. Typo?", name);
}

Int
FEProblemInterface::get_aux_field_dof(Int point, Int fid) const
{
    _F_;
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->section_aux, point, fid, &offset));
    return offset;
}

const Vector &
FEProblemInterface::get_solution_vector_local() const
{
    _F_;
    build_local_solution_vector(this->sln);
    return this->sln;
}

const Vector &
FEProblemInterface::get_aux_solution_vector_local() const
{
    return this->a;
}

WeakForm *
FEProblemInterface::get_weak_form() const
{
    _F_;
    return this->wf;
}

bool
FEProblemInterface::has_field_by_id(Int fid) const
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
FEProblemInterface::get_field_component_name(Int fid, Int component) const
{
    _F_;
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
        error("Field with ID = '{}' does not exist.", fid);
}

void
FEProblemInterface::set_field_component_name(Int fid, Int component, const std::string & name)
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
FEProblemInterface::get_num_aux_fields() const
{
    _F_;
    return (Int) this->aux_fields.size();
}

std::vector<std::string>
FEProblemInterface::get_aux_field_names() const
{
    _F_;
    std::vector<std::string> names;
    names.reserve(this->aux_fields.size());
    for (const auto & it : this->aux_fields)
        names.push_back(it.second.name);
    return names;
}

const std::string &
FEProblemInterface::get_aux_field_name(Int fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.name;
    else
        error("Auxiliary field with ID = '{}' does not exist.", fid);
}

Int
FEProblemInterface::get_aux_field_num_components(Int fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.nc;
    else
        error("Auxiliary field with ID = '{}' does not exist.", fid);
}

Int
FEProblemInterface::get_aux_field_id(const std::string & name) const
{
    _F_;
    const auto & it = this->aux_fields_by_name.find(name);
    if (it != this->aux_fields_by_name.end())
        return it->second;
    else
        error("Auxiliary field '{}' does not exist. Typo?", name);
}

bool
FEProblemInterface::has_aux_field_by_id(Int fid) const
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

Int
FEProblemInterface::get_aux_field_order(Int fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.k;
    else
        error("Auxiliary field with ID = '{}' does not exist.", fid);
}

std::string
FEProblemInterface::get_aux_field_component_name(Int fid, Int component) const
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
FEProblemInterface::set_aux_field_component_name(Int fid, Int component, const std::string & name)
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

bool
FEProblemInterface::has_aux(const std::string & name) const
{
    _F_;
    const auto & it = this->auxs_by_name.find(name);
    return it != this->auxs_by_name.end();
}

AuxiliaryField *
FEProblemInterface::get_aux(const std::string & name) const
{
    _F_;
    const auto & it = this->auxs_by_name.find(name);
    if (it != this->auxs_by_name.end())
        return it->second;
    else
        return nullptr;
}

Int
FEProblemInterface::add_fe(const std::string & name, Int nc, Int k)
{
    _F_;
    std::vector<Int> keys = utils::map_keys(this->fields);
    Int id = get_next_id(keys);
    set_fe(id, name, nc, k);
    return id;
}

void
FEProblemInterface::set_fe(Int id, const std::string & name, Int nc, Int k)
{
    _F_;
    auto it = this->fields.find(id);
    if (it == this->fields.end()) {
        FieldInfo fi(name, id, nc, k, this->asmbl.dim);
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (unsigned int i = 0; i < nc; i++)
                fi.component_names[i] = fmt::format("{:d}", i);
        }
        this->fields.emplace(id, fi);
        this->fields_by_name[name] = id;
    }
    else
        error("Cannot add field '{}' with ID = {}. ID already exists.", name, id);
}

Int
FEProblemInterface::add_aux_fe(const std::string & name, Int nc, Int k)
{
    _F_;
    std::vector<Int> keys = utils::map_keys(this->aux_fields);
    Int id = get_next_id(keys);
    set_aux_fe(id, name, nc, k);
    return id;
}

void
FEProblemInterface::set_aux_fe(Int id, const std::string & name, Int nc, Int k)
{
    _F_;
    auto it = this->aux_fields.find(id);
    if (it == this->aux_fields.end()) {
        FieldInfo fi(name, id, nc, k, this->asmbl.dim);
        this->aux_fields.emplace(id, fi);
        this->aux_fields_by_name[name] = id;
    }
    else
        error("Cannot add auxiliary field '{}' with ID = {}. ID is already taken.", name, id);
}

void
FEProblemInterface::add_auxiliary_field(AuxiliaryField * aux)
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

void
FEProblemInterface::set_up_field_null_space(DM dm)
{
}

void
FEProblemInterface::create_fe(FieldInfo & fi)
{
    _F_;
    const MPI_Comm & comm = this->unstr_mesh->get_comm();
    Int dim = this->problem->get_dimension();
    PetscBool is_simplex = this->unstr_mesh->is_simplex() ? PETSC_TRUE : PETSC_FALSE;
    PETSC_CHECK(internal::create_lagrange_petscfe(comm,
                                                  dim,
                                                  fi.nc,
                                                  is_simplex,
                                                  fi.k,
                                                  this->qorder,
                                                  &fi.fe));
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
    for (auto & it : this->fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(PetscDSSetContext(this->ds, fi.id, this));
    }

    set_up_assembly_data();
}

void
FEProblemInterface::set_up_assembly_data()
{
    _F_;
    this->asmbl.dim = this->problem->get_dimension();
    PETSC_CHECK(
        PetscDSGetEvaluationArrays(this->ds, &this->asmbl.u, &this->asmbl.u_t, &this->asmbl.u_x));
    Int *u_offset, *u_offset_x;
    PETSC_CHECK(PetscDSGetComponentOffsets(this->ds, &u_offset));
    PETSC_CHECK(PetscDSGetComponentDerivativeOffsets(this->ds, &u_offset_x));
    for (auto & it : this->fields) {
        FieldInfo & fi = it.second;
        fi.values.set(this->asmbl.u + u_offset[fi.id]);
        fi.derivs.set(this->asmbl.u_x + u_offset_x[fi.id]);
        fi.dots.set(this->asmbl.u_t + u_offset[fi.id]);
    }
    Real * coord;
    PETSC_CHECK(PetscDSGetWorkspace(this->ds, &coord, nullptr, nullptr, nullptr, nullptr));
    this->asmbl.xyz.set(coord);
}

void
FEProblemInterface::set_up_quadrature()
{
    _F_;
    assert(!this->fields.empty());
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
FEProblemInterface::compute_label_aux_fields(DM dm,
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
FEProblemInterface::compute_aux_fields()
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
FEProblemInterface::set_up_auxiliary_dm(DM dm)
{
    _F_;
    if (this->aux_fields.empty())
        return;

    PETSC_CHECK(DMClone(dm, &this->dm_aux));

    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(DMSetField(this->dm_aux, fi.id, fi.block, (PetscObject) fi.fe));
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

        set_up_assembly_data_aux();
    }
}

void
FEProblemInterface::set_up_assembly_data_aux()
{
    _F_;
    PETSC_CHECK(
        PetscDSGetEvaluationArrays(this->ds_aux, &this->asmbl.a, nullptr, &this->asmbl.a_x));
    Int *a_offset, *a_offset_x;
    PETSC_CHECK(PetscDSGetComponentOffsets(this->ds_aux, &a_offset));
    PETSC_CHECK(PetscDSGetComponentDerivativeOffsets(this->ds_aux, &a_offset_x));

    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        fi.values.set(this->asmbl.a + a_offset[fi.id]);
        fi.derivs.set(this->asmbl.a_x + a_offset_x[fi.id]);
    }
}

const Int &
FEProblemInterface::get_spatial_dimension() const
{
    _F_;
    return this->asmbl.dim;
}

const FieldValue &
FEProblemInterface::get_field_value(const std::string & field_name) const
{
    _F_;
    if (has_field_by_name(field_name)) {
        Int fid = get_field_id(field_name);
        return this->fields.at(fid).values;
    }
    else if (has_aux_field_by_name(field_name)) {
        Int fid = get_aux_field_id(field_name);
        return this->aux_fields.at(fid).values;
    }
    else
        error("Field '{}' does not exist. Typo?", field_name);
}

const FieldGradient &
FEProblemInterface::get_field_gradient(const std::string & field_name) const
{
    _F_;
    if (has_field_by_name(field_name)) {
        Int fid = get_field_id(field_name);
        return this->fields.at(fid).derivs;
    }
    else if (has_aux_field_by_name(field_name)) {
        Int fid = get_aux_field_id(field_name);
        return this->aux_fields.at(fid).derivs;
    }
    else
        error("Field '{}' does not exist. Typo?", field_name);
}

const FieldValue &
FEProblemInterface::get_field_dot(const std::string & field_name) const
{
    _F_;
    if (has_field_by_name(field_name)) {
        Int fid = get_field_id(field_name);
        return this->fields.at(fid).dots;
    }
    else if (has_aux_field_by_name(field_name)) {
        Int fid = get_aux_field_id(field_name);
        return this->aux_fields.at(fid).dots;
    }
    else
        error("Field '{}' does not exist. Typo?", field_name);
}

const Real &
FEProblemInterface::get_time_shift() const
{
    _F_;
    return this->asmbl.u_t_shift;
}

const Real &
FEProblemInterface::get_time() const
{
    _F_;
    return this->asmbl.time;
}

const Normal &
FEProblemInterface::get_normal() const
{
    _F_;
    return this->asmbl.normals;
}

const Point &
FEProblemInterface::get_xyz() const
{
    _F_;
    return this->asmbl.xyz;
}

void
FEProblemInterface::sort_residual_functionals(
    const std::map<std::string, const ValueFunctional *> & suppliers)
{
    _F_;
    auto graph = build_dependecy_graph(suppliers);
    this->sorted_res_functionals.clear();
    auto res_keys = this->wf->get_residual_keys();
    for (auto & k : res_keys) {
        for (Int f = 0; f < get_num_fields(); f++) {
            Label lbl(k.label);
            auto f0_fnls = this->wf->get(PETSC_WF_F0, lbl, k.value, f, k.part);
            auto f1_fnls = this->wf->get(PETSC_WF_F1, lbl, k.value, f, k.part);

            add_functionals<ResidualFunc *>(graph, suppliers, f0_fnls);
            add_functionals<ResidualFunc *>(graph, suppliers, f1_fnls);

            std::vector<const Functional *> fnls;
            fnls.insert(fnls.end(), f0_fnls.begin(), f0_fnls.end());
            fnls.insert(fnls.end(), f1_fnls.begin(), f1_fnls.end());
            auto sv = graph.bfs(fnls);

            PetscFormKey pwfk = k;
            pwfk.field = f;
            // bfs gives back a sorted vector, but in reverse order, so
            // we reverse the vector here to get the order of evaluation
            for (auto it = sv.rbegin(); it != sv.rend(); it++) {
                auto ofnl = dynamic_cast<const ValueFunctional *>(*it);
                if (ofnl)
                    this->sorted_res_functionals[pwfk].push_back(ofnl);
            }
        }
    }
}

void
FEProblemInterface::sort_jacobian_functionals(
    const std::map<std::string, const ValueFunctional *> & suppliers)
{
    _F_;
    auto graph = build_dependecy_graph(suppliers);
    this->sorted_jac_functionals.clear();
    auto jac_keys = this->wf->get_jacobian_keys();
    for (auto & k : jac_keys) {
        for (Int f = 0; f < get_num_fields(); f++) {
            for (Int g = 0; g < get_num_fields(); g++) {
                Label lbl(k.label);
                auto g0_fnls = this->wf->get(PETSC_WF_G0, lbl, k.value, f, g, k.part);
                auto g1_fnls = this->wf->get(PETSC_WF_G1, lbl, k.value, f, g, k.part);
                auto g2_fnls = this->wf->get(PETSC_WF_G2, lbl, k.value, f, g, k.part);
                auto g3_fnls = this->wf->get(PETSC_WF_G3, lbl, k.value, f, g, k.part);

                add_functionals<JacobianFunc *>(graph, suppliers, g0_fnls);
                add_functionals<JacobianFunc *>(graph, suppliers, g1_fnls);
                add_functionals<JacobianFunc *>(graph, suppliers, g2_fnls);
                add_functionals<JacobianFunc *>(graph, suppliers, g3_fnls);

                std::vector<const Functional *> fnls;
                fnls.insert(fnls.end(), g0_fnls.begin(), g0_fnls.end());
                fnls.insert(fnls.end(), g1_fnls.begin(), g1_fnls.end());
                fnls.insert(fnls.end(), g2_fnls.begin(), g2_fnls.end());
                fnls.insert(fnls.end(), g3_fnls.begin(), g3_fnls.end());
                auto sv = graph.bfs(fnls);

                PetscFormKey pwfk = k;
                pwfk.field = this->wf->get_jac_key(f, g);
                // bfs gives back a sorted vector, but in reverse order, so
                // we reverse the vector here to get the order of evaluation
                for (auto it = sv.rbegin(); it != sv.rend(); it++) {
                    auto ofnl = dynamic_cast<const ValueFunctional *>(*it);
                    if (ofnl)
                        this->sorted_jac_functionals[pwfk].push_back(ofnl);
                }
            }
        }
    }
}

void
FEProblemInterface::sort_functionals()
{
    _F_;
    auto suppliers = get_suppliers();
    sort_residual_functionals(suppliers);
    sort_jacobian_functionals(suppliers);
}

PetscErrorCode
FEProblemInterface::integrate_residual(PetscDS ds,
                                       PetscFormKey key,
                                       Int n_elems,
                                       PetscFEGeom * cell_geom,
                                       const Scalar coefficients[],
                                       const Scalar coefficients_t[],
                                       PetscDS ds_aux,
                                       const Scalar coefficients_aux[],
                                       Real t,
                                       Scalar elem_vec[])
{
    _F_;
    Int field = key.field;
    Label lbl(key.label);
    const auto & f0_res_fns = this->wf->get(PETSC_WF_F0, lbl, key.value, field, key.part);
    const auto & f1_res_fns = this->wf->get(PETSC_WF_F1, lbl, key.value, field, key.part);
    if (f0_res_fns.empty() && f1_res_fns.empty())
        return 0;

    PetscFE & fe = this->fields.at(field).fe;

    PETSC_CHECK(PetscFEGetSpatialDimension(fe, &this->asmbl.dim));
    this->asmbl.time = t;

    Scalar *basis_real, *basis_der_real;
    PETSC_CHECK(PetscDSGetWorkspace(ds, nullptr, &basis_real, &basis_der_real, nullptr, nullptr));
    Scalar *f0, *f1;
    PETSC_CHECK(PetscDSGetWeakFormArrays(ds, &f0, &f1, nullptr, nullptr, nullptr, nullptr));
    Int f_offset;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field, &f_offset));
    PetscTabulation * T;
    PETSC_CHECK(PetscDSGetTabulation(ds, &T));
    Int tot_dim = 0;
    PETSC_CHECK(PetscDSGetTotalDimension(ds, &tot_dim));

    Int tot_dim_aux = 0;
    PetscTabulation * T_aux = nullptr;
    if (ds_aux) {
        PETSC_CHECK(PetscDSGetTotalDimension(ds_aux, &tot_dim_aux));
        PETSC_CHECK(PetscDSGetTabulation(ds_aux, &T_aux));
        PetscCheckFalse(
            T[0]->Np != T_aux[0]->Np,
            PETSC_COMM_SELF,
            PETSC_ERR_ARG_WRONG,
            "Number of tabulation points %D != %D number of auxiliary tabulation points",
            T[0]->Np,
            T_aux[0]->Np);
    }

    // FIXME: quad should be a member variable
    PetscQuadrature quad;
    PETSC_CHECK(PetscFEGetQuadrature(fe, &quad));
    Int q_dim, q_n_comp, q_n_pts;
    const Real *q_points, *q_weights;
    PETSC_CHECK(PetscQuadratureGetData(quad, &q_dim, &q_n_comp, &q_n_pts, &q_points, &q_weights));
    PetscCheckFalse(q_n_comp != 1,
                    PETSC_COMM_SELF,
                    PETSC_ERR_SUP,
                    "Only supports scalar quadrature, not %D components",
                    q_n_comp);

    Int dim_embed = cell_geom->dimEmbed;
    PetscCheckFalse(cell_geom->dim != q_dim,
                    PETSC_COMM_SELF,
                    PETSC_ERR_ARG_INCOMP,
                    "FEGeom dim %D != %D quadrature dim",
                    cell_geom->dim,
                    q_dim);

    Int n_fields = get_num_fields();
    Int n_fields_aux = get_num_aux_fields();
    Int c_offset = 0;
    Int c_offset_aux = 0;
    for (Int e = 0; e < n_elems; ++e) {
        PetscFEGeom fe_geom;

        fe_geom.v = this->asmbl.xyz.get(); /* workspace */

        PETSC_CHECK(PetscArrayzero(f0, q_n_pts * T[field]->Nc));
        PETSC_CHECK(PetscArrayzero(f1, q_n_pts * T[field]->Nc * dim_embed));

        for (Int q = 0; q < q_n_pts; ++q) {
            PETSC_CHECK(
                PetscFEGeomGetPoint(cell_geom, e, q, &q_points[q * cell_geom->dim], &fe_geom));
            this->asmbl.xyz.set(fe_geom.v);
            Real w = fe_geom.detJ[0] * q_weights[q];

            PETSC_CHECK(evaluate_field_jets(ds,
                                            n_fields,
                                            0,
                                            q,
                                            T,
                                            &fe_geom,
                                            &coefficients[c_offset],
                                            coefficients_t ? &coefficients_t[c_offset] : nullptr,
                                            this->asmbl.u,
                                            this->asmbl.u_x,
                                            coefficients_t ? this->asmbl.u_t : nullptr));
            if (ds_aux)
                PETSC_CHECK(evaluate_field_jets(ds_aux,
                                                n_fields_aux,
                                                0,
                                                q,
                                                T_aux,
                                                &fe_geom,
                                                &coefficients_aux[c_offset_aux],
                                                nullptr,
                                                this->asmbl.a,
                                                this->asmbl.a_x,
                                                nullptr));
            for (auto & f : this->sorted_res_functionals[key])
                f->evaluate();
            for (auto & func : f0_res_fns)
                func->evaluate(&f0[q * T[field]->Nc]);
            for (Int c = 0; c < T[field]->Nc; ++c)
                f0[q * T[field]->Nc + c] *= w;
            for (auto & func : f1_res_fns)
                func->evaluate(&f1[q * T[field]->Nc * this->asmbl.dim]);
            for (Int c = 0; c < T[field]->Nc; ++c)
                for (Int d = 0; d < this->asmbl.dim; ++d)
                    f1[(q * T[field]->Nc + c) * this->asmbl.dim + d] *= w;
        }

        PETSC_CHECK(update_element_vec(fe,
                                       T[field],
                                       0,
                                       basis_real,
                                       basis_der_real,
                                       e,
                                       cell_geom,
                                       f0,
                                       f1,
                                       &elem_vec[c_offset + f_offset]));

        c_offset += tot_dim;
        c_offset_aux += tot_dim_aux;
    }

    return 0;
}

PetscErrorCode
FEProblemInterface::integrate_bnd_residual(PetscDS ds,
                                           PetscFormKey key,
                                           Int n_elems,
                                           PetscFEGeom * face_geom,
                                           const Scalar coefficients[],
                                           const Scalar coefficients_t[],
                                           PetscDS ds_aux,
                                           const Scalar coefficients_aux[],
                                           Real t,
                                           Scalar elem_vec[])
{
    _F_;
    Int field = key.field;
    Label lbl(key.label);
    const auto & f0_res_fns = this->wf->get(PETSC_WF_BDF0, lbl, key.value, field, key.part);
    const auto & f1_res_fns = this->wf->get(PETSC_WF_BDF1, lbl, key.value, field, key.part);
    if (f0_res_fns.empty() && f1_res_fns.empty())
        return 0;

    PetscFE & fe = this->fields.at(field).fe;

    PETSC_CHECK(PetscFEGetSpatialDimension(fe, &this->asmbl.dim));
    this->asmbl.time = t;

    Scalar *basis_real, *basis_der_real;
    PETSC_CHECK(PetscDSGetWorkspace(ds, nullptr, &basis_real, &basis_der_real, nullptr, nullptr));
    Scalar *f0, *f1;
    PETSC_CHECK(PetscDSGetWeakFormArrays(ds, &f0, &f1, nullptr, nullptr, nullptr, nullptr));
    Int f_offset;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field, &f_offset));
    Int tot_dim = 0;
    PETSC_CHECK(PetscDSGetTotalDimension(ds, &tot_dim));
    PetscTabulation * T_face;
    PETSC_CHECK(PetscDSGetFaceTabulation(ds, &T_face));

    Int dim_aux = 0;
    Int tot_dim_aux = 0;
    PetscTabulation * T_face_aux = nullptr;
    PetscBool aux_on_bnd = PETSC_FALSE;
    if (ds_aux) {
        PETSC_CHECK(PetscDSGetSpatialDimension(ds_aux, &dim_aux));
        PETSC_CHECK(PetscDSGetTotalDimension(ds_aux, &tot_dim_aux));

        aux_on_bnd = dim_aux < this->asmbl.dim ? PETSC_TRUE : PETSC_FALSE;
        if (aux_on_bnd)
            PETSC_CHECK(PetscDSGetTabulation(ds_aux, &T_face_aux));
        else
            PETSC_CHECK(PetscDSGetFaceTabulation(ds_aux, &T_face_aux));

        PetscCheckFalse(
            T_face[0]->Np != T_face_aux[0]->Np,
            PETSC_COMM_SELF,
            PETSC_ERR_ARG_WRONG,
            "Number of tabulation points %D != %D number of auxiliary tabulation points",
            T_face[0]->Np,
            T_face_aux[0]->Np);
    }

    Int n_comp_i = T_face[field]->Nc;

    // FIXME: quad should be a member variable
    PetscQuadrature quad;
    PETSC_CHECK(PetscFEGetFaceQuadrature(fe, &quad));
    Int q_dim, q_n_comp, q_n_pts;
    const Real *q_points, *q_weights;
    PETSC_CHECK(PetscQuadratureGetData(quad, &q_dim, &q_n_comp, &q_n_pts, &q_points, &q_weights));
    PetscCheckFalse(q_n_comp != 1,
                    PETSC_COMM_SELF,
                    PETSC_ERR_SUP,
                    "Only supports scalar quadrature, not %D components",
                    q_n_comp);

    Int dim_embed = face_geom->dimEmbed;
    /* TODO FIX THIS */
    face_geom->dim = this->asmbl.dim - 1;

    PetscCheckFalse(face_geom->dim != q_dim,
                    PETSC_COMM_SELF,
                    PETSC_ERR_ARG_INCOMP,
                    "FEGeom dim %D != %D quadrature dim",
                    face_geom->dim,
                    q_dim);

    Int n_fields = get_num_fields();
    Int n_fields_aux = get_num_aux_fields();
    Int c_offset = 0;
    Int c_offset_aux = 0;
    for (Int e = 0; e < n_elems; ++e) {
        PetscFEGeom fe_geom, cell_geom;
        const Int face = face_geom->face[e][0];

        fe_geom.v = this->asmbl.xyz.get(); /* Workspace */
        PETSC_CHECK(PetscArrayzero(f0, q_n_pts * n_comp_i));
        PETSC_CHECK(PetscArrayzero(f1, q_n_pts * n_comp_i * dim_embed));
        for (Int q = 0; q < q_n_pts; ++q) {
            PETSC_CHECK(
                PetscFEGeomGetPoint(face_geom, e, q, &q_points[q * face_geom->dim], &fe_geom));
            this->asmbl.xyz.set(fe_geom.v);
            PETSC_CHECK(PetscFEGeomGetCellPoint(face_geom, e, q, &cell_geom));
            Real w = fe_geom.detJ[0] * q_weights[q];
            this->asmbl.normals.set(fe_geom.n);
            PETSC_CHECK(evaluate_field_jets(ds,
                                            n_fields,
                                            face,
                                            q,
                                            T_face,
                                            &cell_geom,
                                            &coefficients[c_offset],
                                            coefficients_t ? &coefficients_t[c_offset] : nullptr,
                                            this->asmbl.u,
                                            this->asmbl.u_x,
                                            coefficients_t ? this->asmbl.u_t : nullptr));
            if (ds_aux)
                PETSC_CHECK(evaluate_field_jets(ds_aux,
                                                n_fields_aux,
                                                aux_on_bnd ? 0 : face,
                                                q,
                                                T_face_aux,
                                                &cell_geom,
                                                &coefficients_aux[c_offset_aux],
                                                nullptr,
                                                this->asmbl.a,
                                                this->asmbl.a_x,
                                                nullptr));
            for (auto & f : this->sorted_res_functionals[key])
                f->evaluate();
            for (auto & func : f0_res_fns)
                func->evaluate(&f0[q * n_comp_i]);
            for (Int c = 0; c < n_comp_i; ++c)
                f0[q * n_comp_i + c] *= w;
            for (auto & func : f1_res_fns)
                func->evaluate(&f1[q * n_comp_i * this->asmbl.dim]);
            for (Int c = 0; c < n_comp_i; ++c)
                for (Int d = 0; d < this->asmbl.dim; ++d)
                    f1[(q * n_comp_i + c) * this->asmbl.dim + d] *= w;
        }
        PETSC_CHECK(update_element_vec(fe,
                                       T_face[field],
                                       face,
                                       basis_real,
                                       basis_der_real,
                                       e,
                                       face_geom,
                                       f0,
                                       f1,
                                       &elem_vec[c_offset + f_offset]));
        c_offset += tot_dim;
        c_offset_aux += tot_dim_aux;
    }

    return 0;
}

PetscErrorCode
FEProblemInterface::integrate_jacobian(PetscDS ds,
                                       PetscFEJacobianType jtype,
                                       PetscFormKey key,
                                       Int n_elems,
                                       PetscFEGeom * cell_geom,
                                       const Scalar coefficients[],
                                       const Scalar coefficients_t[],
                                       PetscDS ds_aux,
                                       const Scalar coefficients_aux[],
                                       Real t,
                                       Real u_tshift,
                                       Scalar elem_mat[])
{
    _F_;
    Int n_fields = get_num_fields();
    Int n_fields_aux = get_num_aux_fields();

    Int field_i = key.field / n_fields;
    Int field_j = key.field % n_fields;

    PetscWeakFormKind kind0, kind1, kind2, kind3;
    switch (jtype) {
    case PETSCFE_JACOBIAN_DYN:
        kind0 = PETSC_WF_GT0;
        kind1 = PETSC_WF_GT1;
        kind2 = PETSC_WF_GT2;
        kind3 = PETSC_WF_GT3;
        break;
    case PETSCFE_JACOBIAN_PRE:
        kind0 = PETSC_WF_GP0;
        kind1 = PETSC_WF_GP1;
        kind2 = PETSC_WF_GP2;
        kind3 = PETSC_WF_GP3;
        break;
    case PETSCFE_JACOBIAN:
        kind0 = PETSC_WF_G0;
        kind1 = PETSC_WF_G1;
        kind2 = PETSC_WF_G2;
        kind3 = PETSC_WF_G3;
        break;
    }

    Label lbl(key.label);
    const auto & g0_jac_fns = this->wf->get(kind0, lbl, key.value, field_i, field_j, key.part);
    const auto & g1_jac_fns = this->wf->get(kind1, lbl, key.value, field_i, field_j, key.part);
    const auto & g2_jac_fns = this->wf->get(kind2, lbl, key.value, field_i, field_j, key.part);
    const auto & g3_jac_fns = this->wf->get(kind3, lbl, key.value, field_i, field_j, key.part);
    if (g0_jac_fns.empty() && g1_jac_fns.empty() && g2_jac_fns.empty() && g3_jac_fns.empty())
        return 0;

    PetscFE & fe_i = this->fields.at(field_i).fe;
    PetscFE & fe_j = this->fields.at(field_j).fe;

    PETSC_CHECK(PetscFEGetSpatialDimension(fe_i, &this->asmbl.dim));
    this->asmbl.time = t;
    this->asmbl.u_t_shift = u_tshift;

    Scalar *basis_real, *basis_der_real, *test_real, *test_der_real;
    PETSC_CHECK(
        PetscDSGetWorkspace(ds, nullptr, &basis_real, &basis_der_real, &test_real, &test_der_real));
    Scalar *g0, *g1, *g2, *g3;
    PETSC_CHECK(PetscDSGetWeakFormArrays(ds, nullptr, nullptr, &g0, &g1, &g2, &g3));
    Int offset_i;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field_i, &offset_i));
    Int offset_j;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field_j, &offset_j));
    Int tot_dim;
    PETSC_CHECK(PetscDSGetTotalDimension(ds, &tot_dim));
    PetscTabulation * T;
    PETSC_CHECK(PetscDSGetTabulation(ds, &T));
    Int n_comp_i = T[field_i]->Nc;
    Int n_comp_j = T[field_j]->Nc;

    PetscTabulation * T_aux = nullptr;
    Int tot_dim_aux = 0;
    if (ds_aux) {
        PETSC_CHECK(PetscDSGetTotalDimension(ds_aux, &tot_dim_aux));
        PETSC_CHECK(PetscDSGetTabulation(ds_aux, &T_aux));
        PetscCheckFalse(
            T[0]->Np != T_aux[0]->Np,
            PETSC_COMM_SELF,
            PETSC_ERR_ARG_WRONG,
            "Number of tabulation points %D != %D number of auxiliary tabulation points",
            T[0]->Np,
            T_aux[0]->Np);
    }

    Int n_pts = cell_geom->numPoints;
    Int dim_embed = cell_geom->dimEmbed;
    bool is_affine = cell_geom->isAffine;

    // Initialize here in case the function is not defined
    PETSC_CHECK(PetscArrayzero(g0, n_comp_i * n_comp_j));
    PETSC_CHECK(PetscArrayzero(g1, n_comp_i * n_comp_j * dim_embed));
    PETSC_CHECK(PetscArrayzero(g2, n_comp_i * n_comp_j * dim_embed));
    PETSC_CHECK(PetscArrayzero(g3, n_comp_i * n_comp_j * dim_embed * dim_embed));

    PetscQuadrature quad;
    PETSC_CHECK(PetscFEGetQuadrature(fe_i, &quad));
    Int q_n_comp, q_n_points;
    const Real *q_points, *q_weights;
    PETSC_CHECK(
        PetscQuadratureGetData(quad, nullptr, &q_n_comp, &q_n_points, &q_points, &q_weights));
    PetscCheckFalse(q_n_comp != 1,
                    PETSC_COMM_SELF,
                    PETSC_ERR_SUP,
                    "Only supports scalar quadrature, not %D components",
                    q_n_comp);

    // Offset into elem_mat[] for element e
    Int e_offset = 0;
    // Offset into coefficients[] for element e
    Int c_offset = 0;
    // Offset into coefficientsAux[] for element e
    Int c_offset_aux = 0;
    for (Int e = 0; e < n_elems; ++e) {
        PetscFEGeom fe_geom;

        fe_geom.dim = cell_geom->dim;
        fe_geom.dimEmbed = cell_geom->dimEmbed;
        if (is_affine) {
            fe_geom.v = this->asmbl.xyz.get();
            fe_geom.xi = cell_geom->xi;
            fe_geom.J = &cell_geom->J[e * n_pts * dim_embed * dim_embed];
            fe_geom.invJ = &cell_geom->invJ[e * n_pts * dim_embed * dim_embed];
            fe_geom.detJ = &cell_geom->detJ[e * n_pts];
        }
        for (Int q = 0; q < q_n_points; ++q) {
            if (is_affine) {
                CoordinatesRefToReal(dim_embed,
                                     this->asmbl.dim,
                                     fe_geom.xi,
                                     &cell_geom->v[e * n_pts * dim_embed],
                                     fe_geom.J,
                                     &q_points[q * this->asmbl.dim],
                                     this->asmbl.xyz.get());
            }
            else {
                this->asmbl.xyz.set(&cell_geom->v[(e * n_pts + q) * dim_embed]);
                fe_geom.v = this->asmbl.xyz.get();
                fe_geom.J = &cell_geom->J[(e * n_pts + q) * dim_embed * dim_embed];
                fe_geom.invJ = &cell_geom->invJ[(e * n_pts + q) * dim_embed * dim_embed];
                fe_geom.detJ = &cell_geom->detJ[e * n_pts + q];
            }
            Real w = fe_geom.detJ[0] * q_weights[q];
            if (coefficients)
                PETSC_CHECK(
                    evaluate_field_jets(ds,
                                        n_fields,
                                        0,
                                        q,
                                        T,
                                        &fe_geom,
                                        &coefficients[c_offset],
                                        coefficients_t ? &coefficients_t[c_offset] : nullptr,
                                        this->asmbl.u,
                                        this->asmbl.u_x,
                                        coefficients_t ? this->asmbl.u_t : nullptr));
            if (ds_aux)
                PETSC_CHECK(evaluate_field_jets(ds_aux,
                                                n_fields_aux,
                                                0,
                                                q,
                                                T_aux,
                                                &fe_geom,
                                                &coefficients_aux[c_offset_aux],
                                                nullptr,
                                                this->asmbl.a,
                                                this->asmbl.a_x,
                                                nullptr));
            for (auto & f : this->sorted_jac_functionals[key])
                f->evaluate();
            if (!g0_jac_fns.empty()) {
                PETSC_CHECK(PetscArrayzero(g0, n_comp_i * n_comp_j));
                for (auto & func : g0_jac_fns)
                    func->evaluate(g0);
                for (Int c = 0; c < n_comp_i * n_comp_j; ++c)
                    g0[c] *= w;
            }
            if (!g1_jac_fns.empty()) {
                PETSC_CHECK(PetscArrayzero(g1, n_comp_i * n_comp_j * dim_embed));
                for (auto & func : g1_jac_fns)
                    func->evaluate(g1);
                for (Int c = 0; c < n_comp_i * n_comp_j * this->asmbl.dim; ++c)
                    g1[c] *= w;
            }
            if (!g2_jac_fns.empty()) {
                PETSC_CHECK(PetscArrayzero(g2, n_comp_i * n_comp_j * dim_embed));
                for (auto & func : g2_jac_fns)
                    func->evaluate(g2);
                for (Int c = 0; c < n_comp_i * n_comp_j * this->asmbl.dim; ++c)
                    g2[c] *= w;
            }
            if (!g3_jac_fns.empty()) {
                PETSC_CHECK(PetscArrayzero(g3, n_comp_i * n_comp_j * dim_embed * dim_embed));
                for (auto & func : g3_jac_fns)
                    func->evaluate(g3);
                for (Int c = 0; c < n_comp_i * n_comp_j * this->asmbl.dim * this->asmbl.dim; ++c)
                    g3[c] *= w;
            }

            PETSC_CHECK(update_element_mat(fe_i,
                                           fe_j,
                                           0,
                                           q,
                                           T[field_i],
                                           basis_real,
                                           basis_der_real,
                                           T[field_j],
                                           test_real,
                                           test_der_real,
                                           &fe_geom,
                                           g0,
                                           g1,
                                           g2,
                                           g3,
                                           e_offset,
                                           tot_dim,
                                           offset_i,
                                           offset_j,
                                           elem_mat));
        }
        c_offset += tot_dim;
        c_offset_aux += tot_dim_aux;
        e_offset += PetscSqr(tot_dim);
    }

    return 0;
}

PetscErrorCode
FEProblemInterface::integrate_bnd_jacobian(PetscDS ds,
                                           PetscFormKey key,
                                           Int n_elems,
                                           PetscFEGeom * face_geom,
                                           const Scalar coefficients[],
                                           const Scalar coefficients_t[],
                                           PetscDS ds_aux,
                                           const Scalar coefficients_aux[],
                                           Real t,
                                           Real u_tshift,
                                           Scalar elem_mat[])
{
    _F_;
    Int n_fields = get_num_fields();
    Int n_fields_aux = get_num_aux_fields();

    Int field_i = key.field / n_fields;
    Int field_j = key.field % n_fields;

    Label lbl(key.label);
    const auto & g0_jac_fns =
        this->wf->get(PETSC_WF_BDG0, lbl, key.value, field_i, field_j, key.part);
    const auto & g1_jac_fns =
        this->wf->get(PETSC_WF_BDG1, lbl, key.value, field_i, field_j, key.part);
    const auto & g2_jac_fns =
        this->wf->get(PETSC_WF_BDG2, lbl, key.value, field_i, field_j, key.part);
    const auto & g3_jac_fns =
        this->wf->get(PETSC_WF_BDG3, lbl, key.value, field_i, field_j, key.part);
    if (g0_jac_fns.empty() && g1_jac_fns.empty() && g2_jac_fns.empty() && g3_jac_fns.empty())
        return 0;

    PetscFE & fe_i = this->fields.at(field_i).fe;
    PetscFE & fe_j = this->fields.at(field_j).fe;

    PETSC_CHECK(PetscFEGetSpatialDimension(fe_i, &this->asmbl.dim));
    this->asmbl.time = t;
    this->asmbl.u_t_shift = u_tshift;

    Scalar *basis_real, *basis_der_real, *test_real, *test_der_real;
    PETSC_CHECK(
        PetscDSGetWorkspace(ds, nullptr, &basis_real, &basis_der_real, &test_real, &test_der_real));
    Scalar *g0, *g1, *g2, *g3;
    PETSC_CHECK(PetscDSGetWeakFormArrays(ds, nullptr, nullptr, &g0, &g1, &g2, &g3));
    Int offset_i;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field_i, &offset_i));
    Int offset_j;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field_j, &offset_j));
    Int tot_dim;
    PETSC_CHECK(PetscDSGetTotalDimension(ds, &tot_dim));
    PetscTabulation * T;
    PETSC_CHECK(PetscDSGetFaceTabulation(ds, &T));
    Int n_comp_i = T[field_i]->Nc;
    Int n_comp_j = T[field_j]->Nc;

    PetscTabulation * T_aux = nullptr;
    Int tot_dim_aux = 0;
    if (ds_aux) {
        PETSC_CHECK(PetscDSGetTotalDimension(ds_aux, &tot_dim_aux));
        PETSC_CHECK(PetscDSGetFaceTabulation(ds_aux, &T_aux));
    }

    Int n_pts = face_geom->numPoints;
    Int dim_embed = face_geom->dimEmbed;
    bool is_affine = face_geom->isAffine;

    // Initialize here in case the function is not defined
    PETSC_CHECK(PetscArrayzero(g0, n_comp_i * n_comp_j));
    PETSC_CHECK(PetscArrayzero(g1, n_comp_i * n_comp_j * dim_embed));
    PETSC_CHECK(PetscArrayzero(g2, n_comp_i * n_comp_j * dim_embed));
    PETSC_CHECK(PetscArrayzero(g3, n_comp_i * n_comp_j * dim_embed * dim_embed));

    PetscQuadrature quad;
    PETSC_CHECK(PetscFEGetFaceQuadrature(fe_i, &quad));
    Int q_n_comp, q_n_points;
    const Real *q_points, *q_weights;
    PETSC_CHECK(
        PetscQuadratureGetData(quad, nullptr, &q_n_comp, &q_n_points, &q_points, &q_weights));
    PetscCheckFalse(q_n_comp != 1,
                    PETSC_COMM_SELF,
                    PETSC_ERR_SUP,
                    "Only supports scalar quadrature, not %D components",
                    q_n_comp);

    // Offset into elem_mat[] for element e
    Int e_offset = 0;
    // Offset into coefficients[] for element e
    Int c_offset = 0;
    // Offset into coefficients_aux[] for element e
    Int c_offset_aux = 0;
    for (Int e = 0; e < n_elems; ++e) {
        PetscFEGeom fe_geom;
        const Int face = face_geom->face[e][0];
        fe_geom.n = nullptr;
        fe_geom.v = nullptr;
        fe_geom.J = nullptr;
        fe_geom.detJ = nullptr;
        fe_geom.dim = face_geom->dim;
        fe_geom.dimEmbed = face_geom->dimEmbed;

        PetscFEGeom cell_geom;
        cell_geom.dim = face_geom->dim;
        cell_geom.dimEmbed = face_geom->dimEmbed;
        if (is_affine) {
            fe_geom.v = this->asmbl.xyz.get();
            fe_geom.xi = face_geom->xi;
            fe_geom.J = &face_geom->J[e * n_pts * dim_embed * dim_embed];
            fe_geom.invJ = &face_geom->invJ[e * n_pts * dim_embed * dim_embed];
            fe_geom.detJ = &face_geom->detJ[e * n_pts];
            fe_geom.n = &face_geom->n[e * n_pts * dim_embed];

            cell_geom.J = &face_geom->suppJ[0][e * n_pts * dim_embed * dim_embed];
            cell_geom.invJ = &face_geom->suppInvJ[0][e * n_pts * dim_embed * dim_embed];
            cell_geom.detJ = &face_geom->suppDetJ[0][e * n_pts];
        }
        for (Int q = 0; q < q_n_points; ++q) {
            if (is_affine) {
                CoordinatesRefToReal(dim_embed,
                                     this->asmbl.dim - 1,
                                     fe_geom.xi,
                                     &face_geom->v[e * n_pts * dim_embed],
                                     fe_geom.J,
                                     &q_points[q * (this->asmbl.dim - 1)],
                                     this->asmbl.xyz.get());
            }
            else {
                this->asmbl.xyz.set(&face_geom->v[(e * n_pts + q) * dim_embed]);
                fe_geom.v = this->asmbl.xyz.get();
                fe_geom.J = &face_geom->J[(e * n_pts + q) * dim_embed * dim_embed];
                fe_geom.invJ = &face_geom->invJ[(e * n_pts + q) * dim_embed * dim_embed];
                fe_geom.detJ = &face_geom->detJ[e * n_pts + q];
                fe_geom.n = &face_geom->n[(e * n_pts + q) * dim_embed];

                cell_geom.J = &face_geom->suppJ[0][(e * n_pts + q) * dim_embed * dim_embed];
                cell_geom.invJ = &face_geom->suppInvJ[0][(e * n_pts + q) * dim_embed * dim_embed];
                cell_geom.detJ = &face_geom->suppDetJ[0][e * n_pts + q];
            }
            this->asmbl.normals.set(fe_geom.n);
            Real w = fe_geom.detJ[0] * q_weights[q];
            if (coefficients)
                PETSC_CHECK(
                    evaluate_field_jets(ds,
                                        n_fields,
                                        face,
                                        q,
                                        T,
                                        &cell_geom,
                                        &coefficients[c_offset],
                                        coefficients_t ? &coefficients_t[c_offset] : nullptr,
                                        this->asmbl.u,
                                        this->asmbl.u_x,
                                        coefficients_t ? this->asmbl.u_t : nullptr));
            if (ds_aux)
                PETSC_CHECK(evaluate_field_jets(ds_aux,
                                                n_fields_aux,
                                                face,
                                                q,
                                                T_aux,
                                                &cell_geom,
                                                &coefficients_aux[c_offset_aux],
                                                nullptr,
                                                this->asmbl.a,
                                                this->asmbl.a_x,
                                                nullptr));

            for (auto & f : this->sorted_jac_functionals[key])
                f->evaluate();
            if (!g0_jac_fns.empty()) {
                PETSC_CHECK(PetscArrayzero(g0, n_comp_i * n_comp_j));
                for (auto & func : g0_jac_fns)
                    func->evaluate(g0);
                for (Int c = 0; c < n_comp_i * n_comp_j; ++c)
                    g0[c] *= w;
            }
            if (!g1_jac_fns.empty()) {
                PETSC_CHECK(PetscArrayzero(g1, n_comp_i * n_comp_j * dim_embed));
                for (auto & func : g1_jac_fns)
                    func->evaluate(g1);
                for (Int c = 0; c < n_comp_i * n_comp_j * this->asmbl.dim; ++c)
                    g1[c] *= w;
            }
            if (!g2_jac_fns.empty()) {
                PETSC_CHECK(PetscArrayzero(g2, n_comp_i * n_comp_j * dim_embed));
                for (auto & func : g2_jac_fns)
                    func->evaluate(g2);
                for (Int c = 0; c < n_comp_i * n_comp_j * this->asmbl.dim; ++c)
                    g2[c] *= w;
            }
            if (!g3_jac_fns.empty()) {
                PETSC_CHECK(PetscArrayzero(g3, n_comp_i * n_comp_j * dim_embed * dim_embed));
                for (auto & func : g3_jac_fns)
                    func->evaluate(g3);
                for (Int c = 0; c < n_comp_i * n_comp_j * this->asmbl.dim * this->asmbl.dim; ++c)
                    g3[c] *= w;
            }

            PETSC_CHECK(update_element_mat(fe_i,
                                           fe_j,
                                           face,
                                           q,
                                           T[field_i],
                                           basis_real,
                                           basis_der_real,
                                           T[field_j],
                                           test_real,
                                           test_der_real,
                                           &cell_geom,
                                           g0,
                                           g1,
                                           g2,
                                           g3,
                                           e_offset,
                                           tot_dim,
                                           offset_i,
                                           offset_j,
                                           elem_mat));
        }
        c_offset += tot_dim;
        c_offset_aux += tot_dim_aux;
        e_offset += PetscSqr(tot_dim);
    }

    return 0;
}

// This is a copy of petsc/fe.c, PetscFEUpdateElementVec_Internal
PetscErrorCode
FEProblemInterface::update_element_vec(PetscFE fe,
                                       PetscTabulation tab,
                                       Int r,
                                       Scalar tmp_basis[],
                                       Scalar tmp_basis_der[],
                                       Int e,
                                       PetscFEGeom * fe_geom,
                                       Scalar f0[],
                                       Scalar f1[],
                                       Scalar elem_vec[])
{
    _F_;
    PetscFEGeom pgeom;
    const Int dEt = tab->cdim;
    const Int dE = fe_geom->dimEmbed;
    const Int Nq = tab->Np;
    const Int Nb = tab->Nb;
    const Int Nc = tab->Nc;
    const Real * basis = &tab->T[0][r * Nq * Nb * Nc];
    const Real * basis_der = &tab->T[1][r * Nq * Nb * Nc * dEt];

    for (Int q = 0; q < Nq; ++q) {
        for (Int b = 0; b < Nb; ++b) {
            for (Int c = 0; c < Nc; ++c) {
                const Int bcidx = b * Nc + c;

                tmp_basis[bcidx] = basis[q * Nb * Nc + bcidx];
                for (Int d = 0; d < dEt; ++d)
                    tmp_basis_der[bcidx * dE + d] = basis_der[q * Nb * Nc * dEt + bcidx * dEt + d];
                for (Int d = dEt; d < dE; ++d)
                    tmp_basis_der[bcidx * dE + d] = 0.0;
            }
        }
        PETSC_CHECK(PetscFEGeomGetCellPoint(fe_geom, e, q, &pgeom));
        PETSC_CHECK(PetscFEPushforward(fe, &pgeom, Nb, tmp_basis));
        PETSC_CHECK(PetscFEPushforwardGradient(fe, &pgeom, Nb, tmp_basis_der));
        for (Int b = 0; b < Nb; ++b) {
            for (Int c = 0; c < Nc; ++c) {
                const Int bcidx = b * Nc + c;
                const Int qcidx = q * Nc + c;

                elem_vec[b] += tmp_basis[bcidx] * f0[qcidx];
                for (Int d = 0; d < dE; ++d)
                    elem_vec[b] += tmp_basis_der[bcidx * dE + d] * f1[qcidx * dE + d];
            }
        }
    }
    return 0;
}

// This is a copy of petsc/fe.c, PetscFEUpdateElementMat_Internal
PetscErrorCode
FEProblemInterface::update_element_mat(PetscFE fe_i,
                                       PetscFE fe_j,
                                       Int r,
                                       Int q,
                                       PetscTabulation tab_i,
                                       Scalar tmp_basis_i[],
                                       Scalar tmp_basis_der_i[],
                                       PetscTabulation tab_j,
                                       Scalar tmp_basis_j[],
                                       Scalar tmp_basis_der_j[],
                                       PetscFEGeom * fe_geom,
                                       const Scalar g0[],
                                       const Scalar g1[],
                                       const Scalar g2[],
                                       const Scalar g3[],
                                       Int e_offset,
                                       Int tot_dim,
                                       Int offset_i,
                                       Int offset_j,
                                       Scalar elem_mat[])
{
    _F_;
    const Int dE = tab_i->cdim;
    const Int NqI = tab_i->Np;
    const Int NbI = tab_i->Nb;
    const Int n_comp_i = tab_i->Nc;
    const Real * basis_i = &tab_i->T[0][(r * NqI + q) * NbI * n_comp_i];
    const Real * basis_der_i = &tab_i->T[1][(r * NqI + q) * NbI * n_comp_i * dE];
    const Int NqJ = tab_j->Np;
    const Int NbJ = tab_j->Nb;
    const Int n_comp_j = tab_j->Nc;
    const Real * basis_j = &tab_j->T[0][(r * NqJ + q) * NbJ * n_comp_j];
    const Real * basis_der_j = &tab_j->T[1][(r * NqJ + q) * NbJ * n_comp_j * dE];

    for (Int f = 0; f < NbI; ++f) {
        for (Int fc = 0; fc < n_comp_i; ++fc) {
            // Test function basis index
            const Int fidx = f * n_comp_i + fc;

            tmp_basis_i[fidx] = basis_i[fidx];
            for (Int df = 0; df < dE; ++df)
                tmp_basis_der_i[fidx * dE + df] = basis_der_i[fidx * dE + df];
        }
    }
    PETSC_CHECK(PetscFEPushforward(fe_i, fe_geom, NbI, tmp_basis_i));
    PETSC_CHECK(PetscFEPushforwardGradient(fe_i, fe_geom, NbI, tmp_basis_der_i));
    for (Int g = 0; g < NbJ; ++g) {
        for (Int gc = 0; gc < n_comp_j; ++gc) {
            // Trial function basis index
            const Int gidx = g * n_comp_j + gc;

            tmp_basis_j[gidx] = basis_j[gidx];
            for (Int dg = 0; dg < dE; ++dg)
                tmp_basis_der_j[gidx * dE + dg] = basis_der_j[gidx * dE + dg];
        }
    }
    PETSC_CHECK(PetscFEPushforward(fe_j, fe_geom, NbJ, tmp_basis_j));
    PETSC_CHECK(PetscFEPushforwardGradient(fe_j, fe_geom, NbJ, tmp_basis_der_j));
    for (Int f = 0; f < NbI; ++f) {
        for (Int fc = 0; fc < n_comp_i; ++fc) {
            // Test function basis index
            const Int fidx = f * n_comp_i + fc;
            // Element matrix row
            const Int i = offset_i + f;
            for (Int g = 0; g < NbJ; ++g) {
                for (Int gc = 0; gc < n_comp_j; ++gc) {
                    // Trial function basis index
                    const Int gidx = g * n_comp_j + gc;
                    // Element matrix column
                    const Int j = offset_j + g;
                    const Int f_off = e_offset + i * tot_dim + j;

                    elem_mat[f_off] +=
                        tmp_basis_i[fidx] * g0[fc * n_comp_j + gc] * tmp_basis_j[gidx];
                    for (Int df = 0; df < dE; ++df) {
                        elem_mat[f_off] += tmp_basis_i[fidx] * g1[(fc * n_comp_j + gc) * dE + df] *
                                           tmp_basis_der_j[gidx * dE + df];
                        elem_mat[f_off] += tmp_basis_der_i[fidx * dE + df] *
                                           g2[(fc * n_comp_j + gc) * dE + df] * tmp_basis_j[gidx];
                        for (Int dg = 0; dg < dE; ++dg) {
                            elem_mat[f_off] += tmp_basis_der_i[fidx * dE + df] *
                                               g3[((fc * n_comp_j + gc) * dE + df) * dE + dg] *
                                               tmp_basis_der_j[gidx * dE + dg];
                        }
                    }
                }
            }
        }
    }
    return (0);
}

// This is a copy of petsc/fe.c, PetscFEEvaluateFieldJets_Internal
PetscErrorCode
FEProblemInterface::evaluate_field_jets(PetscDS ds,
                                        Int nf,
                                        Int r,
                                        Int q,
                                        PetscTabulation tab[],
                                        PetscFEGeom * fe_geom,
                                        const Scalar coefficients[],
                                        const Scalar coefficients_t[],
                                        Scalar u[],
                                        Scalar u_x[],
                                        Scalar u_t[])
{
    _F_;
    Int d_offset = 0, f_offset = 0;

    for (Int f = 0; f < nf; ++f) {
        PetscFE fe;
        const Int k = ds->jetDegree[f];
        const Int cdim = tab[f]->cdim;
        const Int n_q = tab[f]->Np;
        const Int n_bf = tab[f]->Nb;
        const Int n_cf = tab[f]->Nc;
        const Real * Bq = &tab[f]->T[0][(r * n_q + q) * n_bf * n_cf];
        const Real * Dq = &tab[f]->T[1][(r * n_q + q) * n_bf * n_cf * cdim];
        const Real * Hq =
            k > 1 ? &tab[f]->T[2][(r * n_q + q) * n_bf * n_cf * cdim * cdim] : nullptr;
        Int h_offset = 0;

        PETSC_CHECK(PetscDSGetDiscretization(ds, f, (PetscObject *) &fe));
        for (Int c = 0; c < n_cf; ++c)
            u[f_offset + c] = 0.0;
        for (Int d = 0; d < cdim * n_cf; ++d)
            u_x[f_offset * cdim + d] = 0.0;
        for (Int b = 0; b < n_bf; ++b) {
            for (Int c = 0; c < n_cf; ++c) {
                const Int cidx = b * n_cf + c;

                u[f_offset + c] += Bq[cidx] * coefficients[d_offset + b];
                for (Int d = 0; d < cdim; ++d)
                    u_x[(f_offset + c) * cdim + d] +=
                        Dq[cidx * cdim + d] * coefficients[d_offset + b];
            }
        }
        if (k > 1) {
            for (Int g = 0; g < nf; ++g)
                h_offset += tab[g]->Nc * cdim;
            for (Int d = 0; d < cdim * cdim * n_cf; ++d)
                u_x[h_offset + f_offset * cdim * cdim + d] = 0.0;
            for (Int b = 0; b < n_bf; ++b) {
                for (Int c = 0; c < n_cf; ++c) {
                    const Int cidx = b * n_cf + c;

                    for (Int d = 0; d < cdim * cdim; ++d)
                        u_x[h_offset + (f_offset + c) * cdim * cdim + d] +=
                            Hq[cidx * cdim * cdim + d] * coefficients[d_offset + b];
                }
            }
            PETSC_CHECK(
                PetscFEPushforwardHessian(fe, fe_geom, 1, &u_x[h_offset + f_offset * cdim * cdim]));
        }
        PETSC_CHECK(PetscFEPushforward(fe, fe_geom, 1, &u[f_offset]));
        PETSC_CHECK(PetscFEPushforwardGradient(fe, fe_geom, 1, &u_x[f_offset * cdim]));
        if (coefficients_t) {
            for (Int c = 0; c < n_cf; ++c)
                u_t[f_offset + c] = 0.0;
            for (Int b = 0; b < n_bf; ++b) {
                for (Int c = 0; c < n_cf; ++c) {
                    const Int cidx = b * n_cf + c;

                    u_t[f_offset + c] += Bq[cidx] * coefficients_t[d_offset + b];
                }
            }
            PETSC_CHECK(PetscFEPushforward(fe, fe_geom, 1, &u_t[f_offset]));
        }
        f_offset += n_cf;
        d_offset += n_bf;
    }
    return 0;
}

Int
FEProblemInterface::get_next_id(const std::vector<Int> & ids) const
{
    std::set<Int> s;
    for (auto & id : ids)
        s.insert(id);
    for (Int id = 0; id < std::numeric_limits<Int>::max(); id++)
        if (s.find(id) == s.end())
            return id;
    return -1;
}

void
FEProblemInterface::add_boundary_natural_riemann(const std::string & name,
                                                 const Label & label,
                                                 const std::vector<Int> & ids,
                                                 Int field,
                                                 const std::vector<Int> & components,
                                                 PetscNaturalRiemannBCFunc * fn,
                                                 PetscNaturalRiemannBCFunc * fn_t,
                                                 void * context) const
{
    _F_;
    error("Natural Riemann BCs are not supported for FE problems");
}

} // namespace godzilla
