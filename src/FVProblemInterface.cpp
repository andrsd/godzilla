#include "Godzilla.h"
#include "CallStack.h"
#include "FVProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "Logger.h"
#include <assert.h>

namespace godzilla {

const std::string FVProblemInterface::empty_name("");

void
__compute_flux(PetscInt dim,
               PetscInt nf,
               const PetscReal x[],
               const PetscReal n[],
               const PetscScalar uL[],
               const PetscScalar uR[],
               PetscInt n_consts,
               const PetscScalar constants[],
               PetscScalar flux[],
               void * ctx)
{
    _F_;
    FVProblemInterface * fvpi = static_cast<FVProblemInterface *>(ctx);
    fvpi->compute_flux(dim, nf, x, n, uL, uR, n_consts, constants, flux);
}

FVProblemInterface::FVProblemInterface(Problem * problem, const Parameters & params) :
    DiscreteProblemInterface(problem, params),
    section(nullptr),
    fvm(nullptr),
    sln(nullptr)
{
    _F_;
}

FVProblemInterface::~FVProblemInterface()
{
    VecDestroy(&this->sln);
}

void
FVProblemInterface::init()
{
    DiscreteProblemInterface::init();

    DM dm = this->unstr_mesh->get_dm();
    PETSC_CHECK(DMGetLocalSection(dm, &this->section));
}

PetscInt
FVProblemInterface::get_num_fields() const
{
    _F_;
    // becuase there is one field with 'n' components which are the individual fields
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
FVProblemInterface::get_field_name(PetscInt fid) const
{
    _F_;
    if (fid == 0) {
        PetscInt nc;
        PETSC_CHECK(PetscFVGetNumComponents(this->fvm, &nc));
        if (nc == 1)
            return this->fields.at(0).name;
        else
            return empty_name;
    }
    else
        error("Field with ID = '%d' does not exist.", fid);
}

PetscInt
FVProblemInterface::get_field_num_components(PetscInt fid) const
{
    _F_;
    if (fid == 0) {
        PetscInt n_comps = 0;
        for (auto & it : this->fields)
            n_comps += it.second.nc;
        return n_comps;
    }
    else
        error("Field with ID = '%d' does not exist.", fid);
}

PetscInt
FVProblemInterface::get_field_id(const std::string & name) const
{
    _F_;
    return 0;
}

bool
FVProblemInterface::has_field_by_id(PetscInt fid) const
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

PetscInt
FVProblemInterface::get_field_order(PetscInt fid) const
{
    _F_;
    if (fid == 0)
        return 0;
    else
        error("Multiple-field problems are not implemented");
}

std::string
FVProblemInterface::get_field_component_name(PetscInt fid, PetscInt component) const
{
    _F_;
    if (fid == 0) {
        const char * name;
        PETSC_CHECK(PetscFVGetComponentName(this->fvm, component, &name));
        return std::string(name);
    }
    else
        error("Multiple-field problems are not implemented");
}

void
FVProblemInterface::set_field_component_name(PetscInt fid,
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
FVProblemInterface::get_field_dof(PetscInt point, PetscInt fid) const
{
    _F_;
    PetscInt offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->section, point, fid, &offset));
    return offset;
}

Vec
FVProblemInterface::get_solution_vector_local() const
{
    _F_;
    build_local_solution_vector(this->sln);
    return this->sln;
}

void
FVProblemInterface::add_field(PetscInt id, const std::string & name, PetscInt nc)
{
    _F_;
    auto it = this->fields.find(id);
    if (it == this->fields.end()) {
        FieldInfo fi = { name, id, nc, {} };
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
FVProblemInterface::create()
{
    _F_;
    const_cast<UnstructuredMesh *>(this->unstr_mesh)->construct_ghost_cells();
    DiscreteProblemInterface::create();
}

void
FVProblemInterface::allocate_objects()
{
    DM dm = this->unstr_mesh->get_dm();
    PETSC_CHECK(DMCreateLocalVector(dm, &this->sln));
}

void
FVProblemInterface::set_up_ds()
{
    _F_;
    const MPI_Comm & comm = this->unstr_mesh->get_comm();

    PETSC_CHECK(PetscFVCreate(comm, &this->fvm));
    PETSC_CHECK(PetscFVSetType(this->fvm, PETSCFVUPWIND));

    PetscInt n_comps = 0;
    for (auto & it : this->fields)
        n_comps += it.second.nc;
    PETSC_CHECK(PetscFVSetNumComponents(this->fvm, n_comps));

    PETSC_CHECK(PetscFVSetSpatialDimension(this->fvm, this->unstr_mesh->get_dimension()));

    for (PetscInt id = 0, c = 0; id < this->fields.size(); id++) {
        const FieldInfo & fi = this->fields[id];
        if (fi.nc == 1) {
            PETSC_CHECK(PetscFVSetComponentName(this->fvm, c, fi.name.c_str()));
        }
        else {
            for (PetscInt i = 0; i < fi.nc; i++) {
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
    PETSC_CHECK(PetscDSSetRiemannSolver(this->ds, 0, __compute_flux));
    PETSC_CHECK(PetscDSSetContext(this->ds, 0, this));
}

} // namespace godzilla
