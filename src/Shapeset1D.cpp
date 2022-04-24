#include "Shapeset1D.h"
#include "CallStack.h"

namespace godzilla {

Shapeset1D::Shapeset1D(EMode1D mode, uint n_components) : Shapeset(n_components), mode(mode) {}

Shapeset1D::~Shapeset1D() {}

uint *
Shapeset1D::get_edge_indices(uint edge, uint ori, uint order) const
{
    error("1D shapesets have no edge functions");
    return nullptr;
}

uint *
Shapeset1D::get_face_indices(uint face, uint ori, uint order) const
{
    error("1D shapesets have no face functions");
    return nullptr;
}

} // namespace godzilla
