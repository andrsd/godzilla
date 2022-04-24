#include "Mesh.h"
#include "CallStack.h"
#include "Error.h"
#include "Vertex.h"
#include "Edge.h"
#include "Tri.h"
#include "Quad.h"
#include "Tetra.h"
#include "Hex.h"
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
    checkPetscError(ierr);
}

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

    create_elements();
    create_vertices();
}

void
Mesh::create_elements()
{
    _F_;
    PetscErrorCode ierr;
    PetscInt c_start, c_end;
    ierr = DMPlexGetHeightStratum(this->dm, 0, &c_start, &c_end);
    checkPetscError(ierr);
    for (PetscInt cell_id = c_start; cell_id < c_end; cell_id++) {
        PetscErrorCode ierr;

        DMPolytopeType cell_type;
        ierr = DMPlexGetCellType(this->dm, cell_id, &cell_type);
        checkPetscError(ierr);

        PetscInt sz;
        ierr = DMPlexGetConeSize(this->dm, cell_id, &sz);
        checkPetscError(ierr);

        const PetscInt * cone;
        ierr = DMPlexGetCone(this->dm, cell_id, &cone);
        checkPetscError(ierr);

        const Element * e = nullptr;
        switch (cell_type) {
        case DM_POLYTOPE_SEGMENT:
            e = new Edge(cell_id, cone[0], cone[1]);
            break;
        case DM_POLYTOPE_TRIANGLE:
            e = new Tri(cell_id, cone[0], cone[1], cone[2]);
            break;
        case DM_POLYTOPE_QUADRILATERAL:
            e = new Quad(cell_id, cone[0], cone[1], cone[2], cone[3]);
            break;
        case DM_POLYTOPE_TETRAHEDRON:
            e = new Tetra(cell_id, cone[0], cone[1], cone[2], cone[3]);
            break;
        case DM_POLYTOPE_HEXAHEDRON:
            e = new Hex(cell_id, cone[0], cone[1], cone[2], cone[3], cone[4], cone[5], cone[6], cone[7]);
            break;
        default:
            error("Polytope type ", cell_type, " not supported.");
            break;
        }

        this->elements[cell_id] = e;
    }

    // TODO: edges in 2D, faces in 3D
}

void
Mesh::create_vertices()
{
    _F_;
    PetscErrorCode ierr;
    DM cdm;
    ierr = DMGetCoordinateDM(this->dm, &cdm);
    checkPetscError(ierr);
    PetscInt cdim;
    ierr = DMGetCoordinateDim(this->dm, &cdim);
    checkPetscError(ierr);
    Vec coordinates;
    ierr = DMGetCoordinatesLocal(this->dm, &coordinates);
    checkPetscError(ierr);

    PetscInt v_start, v_end;
    ierr = DMPlexGetHeightStratum(this->dm, this->dim, &v_start, &v_end);
    checkPetscError(ierr);
    for (PetscInt vtx_id = v_start; vtx_id < v_end; vtx_id++) {
        PetscInt nc;
        PetscScalar * ccoords = nullptr;
        ierr = DMPlexVecGetClosure(cdm, nullptr, coordinates, vtx_id, &nc, &ccoords);
        checkPetscError(ierr);

        Vertex * vtx;
        switch (cdim) {
        case 1:
            vtx = new Vertex1D(vtx_id, ccoords[0]);
            break;
        case 2:
            vtx = new Vertex2D(vtx_id, ccoords[0], ccoords[1]);
            break;
        case 3:
            vtx = new Vertex3D(vtx_id, ccoords[0], ccoords[1], ccoords[2]);
            break;
        default:
            error("Dimension ", cdim, " is not supported.");
            break;
        }

        this->vertices[vtx_id] = vtx;

        ierr = DMPlexVecRestoreClosure(cdm, nullptr, coordinates, vtx_id, &nc, &ccoords);
        checkPetscError(ierr);
    }
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
