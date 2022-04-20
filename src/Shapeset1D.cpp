#include "Shapeset1D.h"
#include "CallStack.h"

namespace godzilla {

Shapeset1D::Shapeset1D(EMode1D mode, uint n_components) :
    mode(mode),
    num_components(n_components)
{
}

Shapeset1D::~Shapeset1D()
{
}

} // namespace godzilla
