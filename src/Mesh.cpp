#include "Mesh.h"
#include "CallStack.h"

namespace godzilla {

Mesh::Mesh()
{
    _F_;
}

Mesh::~Mesh()
{
    _F_;
    free();
}

void
Mesh::free()
{
    _F_;
}

inline uint
Mesh::get_num_elements() const
{
    return 0;
}

} // namespace godzilla
