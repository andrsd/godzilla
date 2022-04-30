#pragma once

#include "Space.h"

namespace godzilla {

/// H1 space
///
class H1Space : public Space {
public:
    H1Space(Mesh * mesh, Shapeset * ss);
    virtual ~H1Space();

    virtual void get_element_assembly_list(const Element * e, AssemblyList * al);

protected:
    virtual void calc_vertex_boundary_projection(Index vtx_idx);
    virtual void calc_edge_boundary_projection(const Element * elem, uint iedge);
    virtual void calc_face_boundary_projection(const Element * elem, uint iface);
    virtual void assign_dofs_internal();
    virtual uint get_vertex_ndofs();
    virtual uint get_edge_ndofs(uint order);
    virtual uint get_face_ndofs(uint order);
    virtual uint get_element_ndofs(uint order);
};

} // namespace godzilla
