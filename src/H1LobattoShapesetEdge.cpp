#include "H1LobattoShapesetEdge.h"
#include "CallStack.h"
#include "Lobatto.h"

namespace godzilla {

// clang-format off

static Real lobatto_f0(Real x) { return l0(x); }
static Real lobatto_f1(Real x) { return l1(x); }
static Real lobatto_f2(Real x) { return l2(x); }
static Real lobatto_f3(Real x) { return l3(x); }

static shape_fn_1d_t lobatto_edge_fn[] = {
    lobatto_f0, lobatto_f1, lobatto_f2, lobatto_f3
};

static Real lobatto_dx_f0(Real x) { return dl0(x); }
static Real lobatto_dx_f1(Real x) { return dl1(x); }
static Real lobatto_dx_f2(Real x) { return dl2(x); }
static Real lobatto_dx_f3(Real x) { return dl3(x); }

static shape_fn_1d_t lobatto_edge_dx[] = {
    lobatto_dx_f0, lobatto_dx_f1, lobatto_dx_f2, lobatto_dx_f3
};

static uint lobatto_edge_vertex_indices[] = { 0, 1 };

static uint lobatto_edge_bubble_indices_all_orders[] = { 2, 3 };

static uint * lobatto_edge_bubble_indices[] = {
    nullptr,
    nullptr,
    lobatto_edge_bubble_indices_all_orders,
    lobatto_edge_bubble_indices_all_orders
};

static uint lobatto_edge_bubble_count[] = { 0, 0, 1, 2 };

static uint lobatto_edge_index_to_order[] = { 1, 1, 2, 3 };

static shape_fn_1d_t * lobatto_edge_fn_table[] = { lobatto_edge_fn };
static shape_fn_1d_t * lobatto_edge_dx_table[] = { lobatto_edge_dx };

// clang-format on

H1LobattoShapesetEdge::H1LobattoShapesetEdge() : Shapeset1D(MODE_EDGE, 1)
{
    _F_;

    this->shape_table[FN] = lobatto_edge_fn_table;
    this->shape_table[DX] = lobatto_edge_dx_table;

    this->vertex_indices = lobatto_edge_vertex_indices;
    this->bubble_indices = lobatto_edge_bubble_indices;
}

H1LobattoShapesetEdge::~H1LobattoShapesetEdge()
{
    _F_;
}

uint
H1LobattoShapesetEdge::get_order(uint index) const
{
    _F_;
    return lobatto_edge_index_to_order[index];
}

} // namespace godzilla
