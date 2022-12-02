#include "Godzilla.h"
#include "LineMesh.h"
#include "CallStack.h"
#include "petscdm.h"
#include "petscdmplex.h"
#include <array>

namespace godzilla {

REGISTER_OBJECT(LineMesh);

Parameters
LineMesh::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    params.add_param<PetscReal>("xmin", 0., "Minimum in the x direction");
    params.add_param<PetscReal>("xmax", 1., "Maximum in the x direction");
    params.add_required_param<PetscInt>("nx", "Number of mesh points in the x direction");
    return params;
}

LineMesh::LineMesh(const Parameters & parameters) :
    UnstructuredMesh(parameters),
    xmin(get_param<PetscReal>("xmin")),
    xmax(get_param<PetscReal>("xmax")),
    nx(get_param<PetscInt>("nx")),
    interpolate(PETSC_TRUE)
{
    _F_;
    if (this->xmax <= this->xmin)
        log_error("Parameter 'xmax' must be larger than 'xmin'.");
}

PetscReal
LineMesh::get_x_min() const
{
    _F_;
    return this->xmin;
}

PetscReal
LineMesh::get_x_max() const
{
    _F_;
    return this->xmax;
}

PetscInt
LineMesh::get_nx() const
{
    _F_;
    return this->nx;
}

void
LineMesh::create_dm()
{
    _F_;
    std::array<PetscReal, 1> lower = { this->xmin };
    std::array<PetscReal, 1> upper = { this->xmax };
    std::array<PetscInt, 1> faces = { this->nx };
    std::array<DMBoundaryType, 1> periodicity = { DM_BOUNDARY_GHOSTED };

    PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                    1,
                                    PETSC_TRUE,
                                    faces.data(),
                                    lower.data(),
                                    upper.data(),
                                    periodicity.data(),
                                    this->interpolate,
                                    &this->dm));

    // create user-friendly names for sides
    std::map<PetscInt, std::string> face_set_names;
    face_set_names[1] = "left";
    face_set_names[2] = "right";
    create_face_set_labels(face_set_names);
}

} // namespace godzilla
