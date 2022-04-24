#pragma once

#include "petsc.h"

namespace godzilla {

class Vertex {
public:
    Vertex(PetscInt id) : id(id) {}

    /// Vertex ID
    PetscInt id;
};

/// Represents a Vertex in 1D
class Vertex1D : public Vertex {
public:
    Vertex1D(PetscInt id);
    Vertex1D(PetscInt id, double x);
    Vertex1D(const Vertex1D & o);
    virtual Vertex1D * copy();

    /// x-coordinate
    double x;
};

/// Represents a Vertex in 2D
class Vertex2D : public Vertex {
public:
    Vertex2D(PetscInt id);
    Vertex2D(PetscInt id, double x, double y);
    Vertex2D(const Vertex2D & o);
    virtual Vertex2D * copy();

    /// x-, y-coordinates
    double x, y;
};

/// Represents a Vertex in 3D
class Vertex3D : public Vertex {
public:
    Vertex3D(PetscInt id);
    Vertex3D(PetscInt id, double x, double y, double z);
    Vertex3D(const Vertex3D & o);
    virtual Vertex3D * copy();

    /// x-, y-, z-coordinates
    double x, y, z;
};

} // namespace godzilla
