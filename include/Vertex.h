#pragma once

namespace godzilla {

class Vertex {
};

/// Represents a Vertex in 3D
class Vertex3D : public Vertex {
public:
    Vertex3D();
    Vertex3D(double x, double y = 0., double z = 0.);
    Vertex3D(const Vertex3D & o);
    virtual Vertex3D * copy();

protected:
    /// x-, y-, z-coordinates
    double x, y, z;
};

} // namespace godzilla
