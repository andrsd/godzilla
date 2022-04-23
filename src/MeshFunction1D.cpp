#include "MeshFunction1D.h"
#include "CallStack.h"
#include "Error.h"

namespace godzilla {

MeshFunction1D::MeshFunction1D(const Mesh * mesh, uint num_components) :
    ScalarFunction1D(num_components)
{
    _F_;
    this->mesh = mesh;
    this->refmap = new RefMap1D(mesh);
    MEM_CHECK(this->refmap);
    this->element = NULL;
}

MeshFunction1D::~MeshFunction1D()
{
    _F_;
    delete refmap;
}

void
MeshFunction1D::set_active_element(const Element1D * e)
{
    _F_;
    this->element = e;
    this->refmap->set_active_element(e);
}

} // namespace godzilla
