#include "Shapeset3D.h"
#include "CallStack.h"

namespace godzilla {

Shapeset3D::Shapeset3D(EMode3D mode, uint n_components) : Shapeset(n_components), mode(mode) {}

Shapeset3D::~Shapeset3D() {}

} // namespace godzilla
