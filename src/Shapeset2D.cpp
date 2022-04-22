#include "Shapeset2D.h"
#include "CallStack.h"

namespace godzilla {

Shapeset2D::Shapeset2D(EMode2D mode, uint n_components) : Shapeset(n_components), mode(mode) {}

Shapeset2D::~Shapeset2D() {}

uint *
Shapeset2D::get_face_indices(uint face, uint ori, uint order) const
{
    error("2D shapesets have no face functions");
    return nullptr;
}

} // namespace godzilla
