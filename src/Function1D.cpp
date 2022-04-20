#include "Function1D.h"
#include "CallStack.h"

namespace godzilla {

// the order of items must match values of EValueType
template <typename TYPE>
uint Function1D<TYPE>::idx2mask[][NUM_COMPONENTS] = { { FN_VAL_0 }, { FN_DX_0 }, { FN_DXX_0 } };

template <typename TYPE>
Function1D<TYPE>::Function1D()
{
    _F_;
    this->order = 0;
    this->cur_node = NULL;
    memset(this->quads, 0, sizeof(this->quads));
    this->cur_quad = 0;
    this->sub_idx = 0;
}

template <typename TYPE>
Function1D<TYPE>::~Function1D()
{
    _F_;
}

template <typename TYPE>
typename Function1D<TYPE>::Node *
Function1D<TYPE>::new_node(uint mask, uint num_points)
{
    _F_;
    // get the number of tables
    uint nt = 0, m = mask;
    if (this->num_components < 3)
        m &= FN_VAL_0 | FN_DX_0;
    while (m) {
        nt += m & 1;
        m >>= 1;
    }

    // allocate a node including its data part, init table pointers
    uint size = sizeof(Node) + sizeof(TYPE) * num_points * nt;
    Node * node = (Node *) malloc(size);
    node->mask = mask;
    node->size = size;
    memset(node->values, 0, sizeof(node->values));
    TYPE * data = node->data;
    for (uint j = 0; j < this->num_components; j++) {
        for (uint i = 0; i < NUM_VALUE_TYPES; i++)
            if (mask & idx2mask[i][j]) {
                node->values[j][i] = data;
                data += num_points;
            }
    }

    return node;
}

} // namespace godzilla
