#include "Mesh.h"
#include "CallStack.h"
#include "Error.h"
#include "petscdm.h"

namespace godzilla {

Mesh::Mesh(const MPI_Comm & comm) : comm(comm)
{
    _F_;
    DMPlexCreate(get_comm(), &this->dm);
}

Mesh::~Mesh()
{
    _F_;
    free();
    DMDestroy(&this->dm);
}

const MPI_Comm &
Mesh::get_comm() const
{
    _F_;
    return this->comm;
}

void
Mesh::free()
{
    _F_;
}

const uint &
Mesh::get_dimension() const
{
    _F_;
    return this->dim;
}

void
Mesh::set_dimension(const uint & dim)
{
    _F_;
    PetscErrorCode ierr;
    ierr = DMSetDimension(this->dm, dim);
    checkPetscError(ierr);
    this->dim = dim;
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

    ierr = DMPlexSetChart(this->dm, 0, this->vertices.count() + this->elements.count());
    checkPetscError(ierr);
    ierr = DMCreateLabel(this->dm, "celltype");
    checkPetscError(ierr);

    // here, we assume contiguous numberintg starting from 0
    for (Index idx = this->elements.first(); idx != INVALID_IDX; idx = this->elements.next(idx)) {
        const Element * elem = this->elements[idx];
        ierr = DMPlexSetConeSize(this->dm, idx, elem->get_num_vertices());
        checkPetscError(ierr);

        DMPolytopeType et = elem->get_potytope_type();
        ierr = DMPlexSetCellType(this->dm, idx, et);
        checkPetscError(ierr);
    }
    uint num_elems = this->elements.count();
    for (Index idx = this->vertices.first(); idx != INVALID_IDX; idx = this->vertices.next(idx)) {
        ierr = DMPlexSetCellType(this->dm, num_elems + idx, DM_POLYTOPE_POINT);
        checkPetscError(ierr);
    }
    ierr = DMSetUp(this->dm);
    checkPetscError(ierr);

    for (Index idx = this->elements.first(); idx != INVALID_IDX; idx = this->elements.next(idx)) {
        const Element * elem = this->elements[idx];
        const Index * vtcs = elem->get_vertices();
        uint nv = elem->get_num_vertices();
        PetscInt cone[nv];
        for (uint i = 0; i < nv; i++)
            cone[i] = vtcs[i] + num_elems;

        DMPolytopeType cell_type;
        ierr = DMPlexGetCellType(this->dm, idx, &cell_type);
        checkPetscError(ierr);
        ierr = DMPlexInvertCell(cell_type, cone);
        checkPetscError(ierr);
        ierr = DMPlexSetCone(this->dm, idx, cone);
        checkPetscError(ierr);
    }

    ierr = DMPlexSymmetrize(this->dm);
    checkPetscError(ierr);
    ierr = DMPlexStratify(this->dm);
    checkPetscError(ierr);

    DM idm;
    ierr = DMPlexInterpolate(this->dm, &idm);
    checkPetscError(ierr);
    ierr = DMDestroy(&this->dm);
    this->dm = idm;
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
    this->boundaries.add(bnd);
}

} // namespace godzilla
