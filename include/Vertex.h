#pragma once

namespace godzilla {

class Vertex {
};

/// Represents a Vertex in 1D
class Vertex1D : public Vertex {
public:
    Vertex1D();
    Vertex1D(double x);
    Vertex1D(const Vertex1D & o);
    virtual Vertex1D * copy();

    /// x-coordinate
    double x;
};

/// Represents a Vertex in 2D
class Vertex2D : public Vertex {
public:
    Vertex2D();
    Vertex2D(double x, double y);
    Vertex2D(const Vertex2D & o);
    virtual Vertex2D * copy();

    /// x-, y-coordinates
    double x, y;
};

/// Represents a Vertex in 3D
class Vertex3D : public Vertex {
public:
    Vertex3D();
    Vertex3D(double x, double y, double z);
    Vertex3D(const Vertex3D & o);
    virtual Vertex3D * copy();

    /// x-, y-, z-coordinates
    double x, y, z;
};

} // namespace godzilla
