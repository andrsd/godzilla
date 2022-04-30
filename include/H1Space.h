#pragma once

#include "Space.h"

namespace godzilla {

/// H1 space
///
class H1Space : public Space {
public:
    H1Space(Mesh * mesh, Shapeset * ss);
    virtual ~H1Space();

    void set_bc_values(Scalar (*fn)(uint marker, double x, double y, double z));

    virtual void get_element_assembly_list(const Element * e, AssemblyList * al);

protected:
    virtual void calc_vertex_boundary_projection(const Element * elem, uint ivertex);
    virtual void calc_edge_boundary_projection(const Element * elem, uint iedge);
    virtual void calc_face_boundary_projection(const Element * elem, uint iface);
    virtual void assign_dofs_internal();
    virtual uint get_vertex_ndofs();
    virtual uint get_edge_ndofs(uint order);
    virtual uint get_face_ndofs(uint order);
    virtual uint get_element_ndofs(uint order);

    /// Callback for obtaining values of essential boundary conditions
    Scalar (*bc_value_callback_by_coord)(uint marker, double x, double y, double z);
};

} // namespace godzilla
