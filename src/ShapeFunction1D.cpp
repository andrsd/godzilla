#include "ShapeFunction1D.h"
#include "CallStack.h"

namespace godzilla {

ShapeFunction1D::ShapeFunction1D() : RealFunction1D()
{
    _F_;
    this->shapeset = NULL;
    this->num_components = 0;
}

ShapeFunction1D::ShapeFunction1D(Shapeset1D * shapeset) : RealFunction1D()
{
    _F_;
    set_shapeset(shapeset);
}

ShapeFunction1D::~ShapeFunction1D()
{
    _F_;
    free();
}

void
ShapeFunction1D::set_active_shape(uint index)
{
    _F_;
    free_cur_node();
    this->index = index;
    this->order = this->shapeset->get_order(index);
}

void
ShapeFunction1D::set_active_element(Element1D * e)
{
    _F_;
    free_cur_node();
    this->element = e;
}

void
ShapeFunction1D::free()
{
    _F_;
    free_cur_node();
}

void
ShapeFunction1D::set_shapeset(Shapeset1D * ss)
{
    _F_;
    free_cur_node();
    this->shapeset = ss;
    this->num_components = ss->get_num_components();
    assert(this->num_components == 1);
}

void
ShapeFunction1D::precalculate(const uint np, const QPoint1D * pt, uint mask)
{
    _F_;
    uint oldmask = (cur_node != NULL) ? cur_node->mask : 0;
    uint newmask = mask | oldmask;
    Node * node = new_node(newmask, np);

    // precalculate all required tables
    for (uint ic = 0; ic < num_components; ic++) {
        for (uint j = 0; j < NUM_VALUE_TYPES; j++) {
            if (newmask & idx2mask[j][ic]) {
                shapeset->get_values(j, index, np, pt, ic, node->values[ic][j]);
            }
        }
    }

    // remove the old node and attach the new one
    replace_cur_node(node);
}

} // namespace godzilla
