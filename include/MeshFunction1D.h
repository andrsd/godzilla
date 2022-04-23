#pragma once

#include "Mesh.h"
#include "Function1D.h"
#include "RefMap1D.h"

namespace godzilla {

/// Represents a function defined on a mesh.
///
/// MeshFunction is a base class for all classes representing an arbitrary function
/// superimposed on a mesh (ie., domain). These include the Solution, ExactSolution
/// and Filter classes, which define the concrete behavior and the way the function
/// is (pre)calculated. Any such function can later be visualized.
class MeshFunction1D : public ScalarFunction1D {
public:
    MeshFunction1D(const Mesh * mesh, uint num_components);
    virtual ~MeshFunction1D();

    virtual void set_active_element(const Element1D * e);

    const Mesh *
    get_mesh() const
    {
        return mesh;
    }

    RefMap1D *
    get_refmap()
    {
        return refmap;
    }

    /// @return Order of the function on the active element
    virtual uint get_order() = 0;

protected:
    /// Mesh
    const Mesh * mesh;
    /// Reference mapping
    RefMap1D * refmap;
};

} // namespace godzilla
