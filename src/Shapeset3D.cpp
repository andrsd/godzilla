#include "Shapeset3D.h"
#include "CallStack.h"

namespace godzilla {

Shapeset3D::Shapeset3D(EMode3D mode, uint n_components)
{
    _F_;
    this->mode = mode;
    this->num_components = n_components;
}

Shapeset3D::~Shapeset3D()
{
    _F_;
}

} // namespace godzilla
