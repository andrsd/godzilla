#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/FVProblemInterface.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/AuxiliaryField.h"
#include "godzilla/Logger.h"
#include "godzilla/Utils.h"
#include <set>
#include <cassert>

namespace godzilla {

namespace {

void
compute_flux(Int dim,
             Int nf,
             const Real x[],
             const Real n[],
             const Scalar uL[],
             const Scalar uR[],
             Int n_consts,
             const Scalar constants[],
             Scalar flux[],
             void * ctx)
{
    _F_;
    auto * fvpi = static_cast<FVProblemInterface *>(ctx);
    fvpi->compute_flux(dim, nf, x, n, uL, uR, n_consts, constants, flux);
}

} // namespace

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
}

void
FVProblemInterface::init()
{
    _F_;
    DiscreteProblemInterface::init();

    auto comm = get_unstr_mesh()->get_comm();
    Int dim = get_problem()->get_dimension();
    PetscBool is_simplex = get_unstr_mesh()->is_simplex() ? PETSC_TRUE : PETSC_FALSE;
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

    auto dm = get_unstr_mesh()->get_dm();
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
        if (this->fields.size() == 1)
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
    const auto & it = this->fields_by_name.find(name);
    return it != this->fields_by_name.end();
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
    get_unstr_mesh()->construct_ghost_cells();
    set_up_fields();
    DiscreteProblemInterface::create();
}

void
FVProblemInterface::set_up_ds()
{
    _F_;
    auto comm = get_unstr_mesh()->get_comm();

    PETSC_CHECK(PetscFVCreate(comm, &this->fvm));
    PETSC_CHECK(PetscFVSetType(this->fvm, PETSCFVUPWIND));

    Int n_comps = 0;
    for (auto & it : this->fields)
        n_comps += it.second.nc;
    PETSC_CHECK(PetscFVSetNumComponents(this->fvm, n_comps));

    PETSC_CHECK(PetscFVSetSpatialDimension(this->fvm, get_unstr_mesh()->get_dimension()));

    for (Int id = 0, c = 0; id < this->fields.size(); id++) {
        const FieldInfo & fi = this->fields.at(id);
        if (fi.nc == 1) {
            PETSC_CHECK(PetscFVSetComponentName(this->fvm, c, fi.name.c_str()));
        }
        else {
            for (Int i = 0; i < fi.nc; i++) {
                std::string name = fmt::format("{}_{}", fi.name, fi.component_names[i]);
                PETSC_CHECK(PetscFVSetComponentName(this->fvm, c + i, name.c_str()));
            }
        }
        c += fi.nc;
    }

    auto dm = get_unstr_mesh()->get_dm();
    PETSC_CHECK(DMAddField(dm, nullptr, (PetscObject) this->fvm));
    create_ds();

    auto ds = get_ds();
    PETSC_CHECK(PetscDSSetRiemannSolver(ds, 0, godzilla::compute_flux));
    PETSC_CHECK(PetscDSSetContext(ds, 0, this));
}

void
FVProblemInterface::set_up_aux_fields()
{
    auto dm_aux = get_dm_aux();
    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(DMSetField(dm_aux, fi.id, fi.block, (PetscObject) this->aux_fe.at(fi.id)));
    }
}

void
FVProblemInterface::add_boundary_essential(const std::string & name,
                                           const std::string & boundary,
                                           Int field,
                                           const std::vector<Int> & components,
                                           PetscFunc * fn,
                                           PetscFunc * fn_t,
                                           void * context)
{
    _F_;
    error("Essential BCs are not supported for FV problems");
}

void
FVProblemInterface::add_boundary_natural(const std::string & name,
                                         const std::string & boundary,
                                         Int field,
                                         const std::vector<Int> & components,
                                         void * context)
{
    _F_;
    error("Natural BCs are not supported for FV problems");
}

Int
FVProblemInterface::get_next_id(const std::vector<Int> & ids) const
{
    _F_;
    std::set<Int> s;
    for (auto & id : ids)
        s.insert(id);
    for (Int id = 0; id < std::numeric_limits<Int>::max(); id++)
        if (s.find(id) == s.end())
            return id;
    return -1;
}

PetscErrorCode
FVProblemInterface::compute_flux(PetscInt dim,
                                 PetscInt nf,
                                 const PetscReal x[],
                                 const PetscReal n[],
                                 const PetscScalar uL[],
                                 const PetscScalar uR[],
                                 PetscInt n_consts,
                                 const PetscScalar constants[],
                                 PetscScalar flux[])
{
    _F_;
    return 0;
}

} // namespace godzilla
