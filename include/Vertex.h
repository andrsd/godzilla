#pragma once

namespace godzilla {

/// Represents a vertex in 3D
class Vertex {
public:
    Vertex();
    Vertex(double x, double y = 0., double z = 0.);
    Vertex(const Vertex & o);
    virtual Vertex * copy();

protected:
    /// x-, y-, z-coordinates
    double x, y, z;
};

} // namespace godzilla
