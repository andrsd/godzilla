#pragma once

#include "Common.h"

namespace godzilla {

/// Base class for shapesets
///
class Shapeset {
public:
    Shapeset(uint n_components);
    virtual ~Shapeset();

    uint
    get_num_components() const
    {
        return num_components;
    }

    // @return index of a vertex shape function for a vertex
    // @param [in] vertex - index of the vertex
    virtual uint get_vertex_index(uint vertex) const = 0;

    virtual uint get_num_edge_fns(uint order) const = 0;

    /// @return indices of edge shape functions
    /// @param [in] edge - edge number (local)
    /// @param [in] ori - orientation of the edge (0 or 1)
    /// @param [in] order - order on the edge
    virtual uint * get_edge_indices(uint edge, uint ori, uint order) const = 0;

    /// @return indices of face shape functions
    /// @param [in] face - face number (local)
    /// @param [in] ori - orientation of the face
    /// @param [in] order - order on the face
    virtual uint * get_face_indices(uint face, uint ori, uint order) const = 0;

    virtual uint get_num_face_fns(uint order) const = 0;

    /// @return indices of bubble functions
    /// @param order - order of the bubble function
    virtual uint * get_bubble_indices(uint order) const = 0;

    virtual uint get_num_bubble_fns(uint order) const = 0;

protected:
    /// Number of components
    uint num_components;
};

} // namespace godzilla
