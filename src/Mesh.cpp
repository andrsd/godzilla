#include "Mesh.h"
#include "CallStack.h"
#include "Error.h"
#include "petscdm.h"

namespace godzilla {

InputParameters
Mesh::validParams()
{
    InputParameters params = Object::validParams();
    params.set<std::string>("_name") = "mesh";
    return params;
}

Mesh::Mesh(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    dm(nullptr),
    dim(-1),
    partition_overlap(0)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscPartitionerCreate(comm(), &this->partitioner);
    checkPetscError(ierr);
}

Mesh::~Mesh()
{
    _F_;
    free();

    PetscErrorCode ierr;
    if (this->dm) {
        ierr = DMDestroy(&this->dm);
        checkPetscError(ierr);
    }
    ierr = PetscPartitionerDestroy(&this->partitioner);
    checkPetscError(ierr);}

DM
Mesh::get_dm() const
{
    _F_;
    return this->dm;
}

void
Mesh::free()
{
    _F_;
}

const int &
Mesh::get_dimension() const
{
    _F_;
    return this->dim;
}

void
Mesh::set_partitioner_type(const std::string & type)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscPartitionerSetType(this->partitioner, type.c_str());
    checkPetscError(ierr);
}

void
Mesh::set_partition_overlap(int overlap)
{
    _F_;
    this->partition_overlap = overlap;
}

uint
Mesh::get_num_elements() const
{
    _F_;
    return this->elements.count();
}

void
Mesh::create()
{
    _F_;
    PetscErrorCode ierr;

    create_dm();
    ierr = DMSetUp(this->dm);
    checkPetscError(ierr);
    ierr = DMGetDimension(this->dm, &this->dim);
    checkPetscError(ierr);
    distribute();
}

uint
Mesh::get_cone_size(const Index & id) const
{
    _F_;
    PetscErrorCode ierr;
    PetscInt sz;
    ierr = DMPlexGetConeSize(this->dm, id, &sz);
    checkPetscError(ierr);
    return sz;
}

void
Mesh::get_cone(const Index & id, const PetscInt * cone[]) const
{
    _F_;
    PetscErrorCode ierr;
    ierr = DMPlexGetCone(this->dm, id, cone);
    checkPetscError(ierr);
}

DMPolytopeType
Mesh::get_cell_type(PetscInt cell) const
{
    _F_;
    PetscErrorCode ierr;
    DMPolytopeType cell_type;
    ierr = DMPlexGetCellType(this->dm, cell, &cell_type);
    checkPetscError(ierr);
    return cell_type;
}

void
Mesh::set_vertex(const Index & id, const Vertex * vertex)
{
    _F_;
    this->vertices[id] = vertex;
}

const Vertex *
Mesh::get_vertex(const Index & id) const
{
    return this->vertices[id];
}

void
Mesh::set_element(const Index & id, const Element * elem)
{
    _F_;
    this->elements[id] = elem;
}

const Element *
Mesh::get_element(const Index & id) const
{
    _F_;
    return this->elements[id];
}

void
Mesh::set_boundary(const Index & eid, const uint & local_side, const uint & marker)
{
    _F_;
    SideBoundary * bnd = new SideBoundary(eid, local_side, marker);
    this->side_boundaries.add(bnd);
}

const SideBoundary *
Mesh::get_side_boundary(const Index & idx) const
{
    _F_;
    return this->side_boundaries[idx];
}

Index
Mesh::get_vertex_id(const Element * e, uint vertex) const
{
    _F_;
    return e->get_vertex(vertex);
}

Index
Mesh::get_edge_id(const Element * e, uint edge) const
{
    _F_;
    error("Not implemented.");
    return 0;
}

Index
Mesh::get_face_id(const Element * e, uint face) const
{
    _F_;
    error("Not implemented.");
    return 0;
}

void
Mesh::distribute()
{
    _F_;
    PetscErrorCode ierr;

    ierr = PetscPartitionerSetUp(this->partitioner);
    checkPetscError(ierr);

    ierr = DMPlexSetPartitioner(this->dm, this->partitioner);
    checkPetscError(ierr);

    DM dm_dist = nullptr;
    ierr = DMPlexDistribute(this->dm, this->partition_overlap, NULL, &dm_dist);
    checkPetscError(ierr);
    if (dm_dist) {
        DMDestroy(&this->dm);
        this->dm = dm_dist;
    }
}

} // namespace godzilla
