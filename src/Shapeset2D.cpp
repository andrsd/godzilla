#include "Shapeset2D.h"
#include "CallStack.h"

namespace godzilla {

Shapeset2D::Shapeset2D(EMode2D mode, uint n_components)
{
    this->mode = mode;
    this->num_components = n_components;
}

Shapeset2D::~Shapeset2D()
{
}

} // namespace godzilla
