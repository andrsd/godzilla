#include "Shapeset2D.h"
#include "CallStack.h"

namespace godzilla {

Shapeset2D::Shapeset2D(EMode2D mode, uint n_components)
{
    _F_;
    this->mode = mode;
    this->num_components = n_components;
}

Shapeset2D::~Shapeset2D()
{
    _F_;
}

} // namespace godzilla
