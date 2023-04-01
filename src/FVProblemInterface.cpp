#include "Godzilla.h"
#include "CallStack.h"
#include "FVProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "Logger.h"
#include <cassert>

namespace godzilla {

const std::string FVProblemInterface::empty_name;

void
__compute_flux(Int dim,
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

FVProblemInterface::FVProblemInterface(Problem * problem, const Parameters & params) :
    DiscreteProblemInterface(problem, params),
    fvm(nullptr)
{
    _F_;
}

FVProblemInterface::~FVProblemInterface()
{
    this->sln.destroy();
}

void
FVProblemInterface::init()
{
    DiscreteProblemInterface::init();
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
    error("Not implemented");
}

std::vector<std::string>
FVProblemInterface::get_aux_field_names() const
{
    return {};
}

const std::string &
FVProblemInterface::get_aux_field_name(Int fid) const
{
    error("Not implemented");
}

Int
FVProblemInterface::get_aux_field_num_components(Int fid) const
{
    error("Not implemented");
}

Int
FVProblemInterface::get_aux_field_id(const std::string & name) const
{
    error("Not implemented");
}

bool
FVProblemInterface::has_aux_field_by_id(Int fid) const
{
    error("Not implemented");
}

bool
FVProblemInterface::has_aux_field_by_name(const std::string & name) const
{
    error("Not implemented");
}

Int
FVProblemInterface::get_aux_field_order(Int fid) const
{
    error("Not implemented");
}

std::string
FVProblemInterface::get_aux_field_component_name(Int fid, Int component) const
{
    error("Not implemented");
}

void
FVProblemInterface::set_aux_field_component_name(Int fid, Int component, const std::string & name)
{
    error("Not implemented");
}

Int
FVProblemInterface::get_aux_field_dof(Int point, Int fid) const
{
    error("Not implemented");
}

const Vector &
FVProblemInterface::get_solution_vector_local() const
{
    _F_;
    build_local_solution_vector(this->sln);
    return this->sln;
}

const Vector &
FVProblemInterface::get_aux_solution_vector_local(DMLabel region) const
{
    return this->a;
}

void
FVProblemInterface::add_field(Int id, const std::string & name, Int nc)
{
    _F_;
    auto it = this->fields.find(id);
    if (it == this->fields.end()) {
        FieldInfo fi = { name, id, nc, {} };
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (Int i = 0; i < nc; i++)
                fi.component_names[i] = fmt::format("{:d}", i);
        }
        this->fields[id] = fi;
        this->fields_by_name[name] = id;
    }
    else
        error("Cannot add field '{}' with ID = {}. ID already exists.", name, id);
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
        const FieldInfo & fi = this->fields[id];
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
    PETSC_CHECK(PetscDSSetRiemannSolver(this->ds, 0, __compute_flux));
    PETSC_CHECK(PetscDSSetContext(this->ds, 0, this));
}

void
FVProblemInterface::add_boundary_essential(const std::string & name,
                                           DMLabel label,
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
                                         DMLabel label,
                                         const std::vector<Int> & ids,
                                         Int field,
                                         const std::vector<Int> & components,
                                         void * context) const
{
    _F_;
    error("Natural BCs are not supported for FV problems");
}

} // namespace godzilla
