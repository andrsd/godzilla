#include "RefMapShapeset1D.h"
#include "CallStack.h"
#include "Lobatto.h"
#include <assert.h>

namespace godzilla {

// clang-format off

static Real lobatto_f0(Real x) { return l0(x); }
static Real lobatto_f1(Real x) { return l1(x); }

static shape_fn_1d_t lobatto_edge_fn[] = {
    lobatto_f0, lobatto_f1
};

static Real lobatto_dx_f0(Real x) { return dl0(x); }
static Real lobatto_dx_f1(Real x) { return dl1(x); }

static shape_fn_1d_t lobatto_edge_dx[] = {
    lobatto_dx_f0, lobatto_dx_f1
};

static uint lobatto_edge_vertex_indices[] = { 0, 1 };

static uint lobatto_edge_index_to_order[] = { 1, 1 };

static shape_fn_1d_t * lobatto_edge_fn_table[] = { lobatto_edge_fn };
static shape_fn_1d_t * lobatto_edge_dx_table[] = { lobatto_edge_dx };

// clang-format on

RefMapShapesetEdge::RefMapShapesetEdge() : Shapeset1D(MODE_EDGE, 1)
{
    this->shape_table[FN] = lobatto_edge_fn_table;
    this->shape_table[DX] = lobatto_edge_dx_table;
    this->vertex_indices = lobatto_edge_vertex_indices;
}

RefMapShapesetEdge::~RefMapShapesetEdge()
{
}

uint
RefMapShapesetEdge::get_order(uint index) const
{
    _F_;
    assert((0 <= index) && (index <= 1));
    return lobatto_edge_index_to_order[index];
}

static RefMapShapesetEdge ss_edge;

const RefMapShapesetEdge *
RefMapShapesetEdge::get()
{
    return &ss_edge;
}

} // namespace godzilla
