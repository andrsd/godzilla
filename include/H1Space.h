#pragma once

#include "Space.h"

namespace godzilla {

/// H1 space
///
class H1Space : public Space {
public:
    H1Space(Mesh * mesh, Shapeset * ss);
    virtual ~H1Space();

protected:
    virtual void assign_dofs_internal();
    virtual uint get_vertex_ndofs();
    virtual uint get_edge_ndofs(uint order);
    virtual uint get_face_ndofs(uint order);
    virtual uint get_element_ndofs(uint order);
};

} // namespace godzilla
