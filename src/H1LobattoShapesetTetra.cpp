#include "H1LobattoShapesetTetra.h"
#include "CallStack.h"
#include "Lobatto.h"

namespace godzilla {

#define lambda0(x, y, z) (((y) + 1) / 2)
#define lambda1(x, y, z) (-(1 + (x) + (y) + (z)) / 2)
#define lambda2(x, y, z) (((x) + 1) / 2)
#define lambda3(x, y, z) (((z) + 1) / 2)

/// X derivatives of affine coordinates

#define lambda0dx(x, y, z) (0.0)
#define lambda1dx(x, y, z) (-1.0 / 2.0)
#define lambda2dx(x, y, z) (1.0 / 2.0)
#define lambda3dx(x, y, z) (0.0)

/// Y derivatives of affine coordinates

#define lambda0dy(x, y, z) (1.0 / 2.0)
#define lambda1dy(x, y, z) (-1.0 / 2.0)
#define lambda2dy(x, y, z) (0.0)
#define lambda3dy(x, y, z) (0.0)

/// Z derivatives of affine coordinates

#define lambda0dz(x, y, z) (0.0)
#define lambda1dz(x, y, z) (-1.0 / 2.0)
#define lambda2dz(x, y, z) (0.0)
#define lambda3dz(x, y, z) (1.0 / 2.0)

// DEGREE 1

// Vertex shape functions, degree 1

static Real
lobatto_f0(Real x, Real y, Real z)
{
    return lambda1(x, y, z);
}

static Real
lobatto_f1(Real x, Real y, Real z)
{
    return lambda2(x, y, z);
}

static Real
lobatto_f2(Real x, Real y, Real z)
{
    return lambda0(x, y, z);
}

static Real
lobatto_f3(Real x, Real y, Real z)
{
    return lambda3(x, y, z);
}

// DEGREE 2

// Edge shape functions, degree 2

// edge 0
static Real
lobatto_f4(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda2(x, y, z) * phi0(lambda1(x, y, z) - lambda2(x, y, z));
}

// edge 1
static Real
lobatto_f5(Real x, Real y, Real z)
{
    return lambda2(x, y, z) * lambda0(x, y, z) * phi0(lambda2(x, y, z) - lambda0(x, y, z));
}

// edge 2
static Real
lobatto_f6(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda0(x, y, z) * phi0(lambda1(x, y, z) - lambda0(x, y, z));
}

// edge 3
static Real
lobatto_f7(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda3(x, y, z) * phi0(lambda1(x, y, z) - lambda3(x, y, z));
}

// edge 4
static Real
lobatto_f8(Real x, Real y, Real z)
{
    return lambda2(x, y, z) * lambda3(x, y, z) * phi0(lambda2(x, y, z) - lambda3(x, y, z));
}

// edge 5
static Real
lobatto_f9(Real x, Real y, Real z)
{
    return lambda0(x, y, z) * lambda3(x, y, z) * phi0(lambda0(x, y, z) - lambda3(x, y, z));
}

// DEGREE 3

// Edge shape functions, degree 3

// edge 0
static Real
lobatto_f10_0(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda2(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z));
}

static Real
lobatto_f10_1(Real x, Real y, Real z)
{
    return -lambda1(x, y, z) * lambda2(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z));
}

// edge 1
static Real
lobatto_f11_0(Real x, Real y, Real z)
{
    return lambda2(x, y, z) * lambda0(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z));
}

static Real
lobatto_f11_1(Real x, Real y, Real z)
{
    return -lambda2(x, y, z) * lambda0(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z));
}

// edge 2
static Real
lobatto_f12_0(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda0(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z));
}

static Real
lobatto_f12_1(Real x, Real y, Real z)
{
    return -lambda1(x, y, z) * lambda0(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z));
}

// edge 3
static Real
lobatto_f13_0(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda3(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z));
}

static Real
lobatto_f13_1(Real x, Real y, Real z)
{
    return -lambda1(x, y, z) * lambda3(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z));
}

// edge 4
static Real
lobatto_f14_0(Real x, Real y, Real z)
{
    return lambda2(x, y, z) * lambda3(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z));
}

static Real
lobatto_f14_1(Real x, Real y, Real z)
{
    return -lambda2(x, y, z) * lambda3(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z));
}

// edge 5
static Real
lobatto_f15_0(Real x, Real y, Real z)
{
    return lambda0(x, y, z) * lambda3(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z));
}

static Real
lobatto_f15_1(Real x, Real y, Real z)
{
    return -lambda0(x, y, z) * lambda3(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z));
}

// Face shape functions, degree 3

// face 0
static Real
lobatto_f16_0(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
           phi0(lambda2(x, y, z) - lambda1(x, y, z)) * phi0(lambda1(x, y, z) - lambda3(x, y, z));
}

static Real
lobatto_f16_1(Real x, Real y, Real z)
{
    return lambda2(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
           phi0(lambda3(x, y, z) - lambda2(x, y, z)) * phi0(lambda2(x, y, z) - lambda1(x, y, z));
}

static Real
lobatto_f16_2(Real x, Real y, Real z)
{
    return lambda3(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
           phi0(lambda1(x, y, z) - lambda3(x, y, z)) * phi0(lambda3(x, y, z) - lambda2(x, y, z));
}

static Real
lobatto_f16_3(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
           phi0(lambda3(x, y, z) - lambda1(x, y, z)) * phi0(lambda1(x, y, z) - lambda2(x, y, z));
}

static Real
lobatto_f16_4(Real x, Real y, Real z)
{
    return lambda2(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
           phi0(lambda1(x, y, z) - lambda2(x, y, z)) * phi0(lambda2(x, y, z) - lambda3(x, y, z));
}

static Real
lobatto_f16_5(Real x, Real y, Real z)
{
    return lambda3(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
           phi0(lambda2(x, y, z) - lambda3(x, y, z)) * phi0(lambda3(x, y, z) - lambda1(x, y, z));
}

// face 1
static Real
lobatto_f17_0(Real x, Real y, Real z)
{
    return lambda2(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
           phi0(lambda0(x, y, z) - lambda2(x, y, z)) * phi0(lambda2(x, y, z) - lambda3(x, y, z));
}

static Real
lobatto_f17_1(Real x, Real y, Real z)
{
    return lambda0(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
           phi0(lambda3(x, y, z) - lambda0(x, y, z)) * phi0(lambda0(x, y, z) - lambda2(x, y, z));
}

static Real
lobatto_f17_2(Real x, Real y, Real z)
{
    return lambda3(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
           phi0(lambda2(x, y, z) - lambda3(x, y, z)) * phi0(lambda3(x, y, z) - lambda0(x, y, z));
}

static Real
lobatto_f17_3(Real x, Real y, Real z)
{
    return lambda2(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
           phi0(lambda3(x, y, z) - lambda2(x, y, z)) * phi0(lambda2(x, y, z) - lambda0(x, y, z));
}

static Real
lobatto_f17_4(Real x, Real y, Real z)
{
    return lambda0(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
           phi0(lambda2(x, y, z) - lambda0(x, y, z)) * phi0(lambda0(x, y, z) - lambda3(x, y, z));
}

static Real
lobatto_f17_5(Real x, Real y, Real z)
{
    return lambda3(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
           phi0(lambda0(x, y, z) - lambda3(x, y, z)) * phi0(lambda3(x, y, z) - lambda2(x, y, z));
}

// face 2
static Real
lobatto_f18_0(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
           phi0(lambda0(x, y, z) - lambda1(x, y, z)) * phi0(lambda1(x, y, z) - lambda3(x, y, z));
}

static Real
lobatto_f18_1(Real x, Real y, Real z)
{
    return lambda0(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
           phi0(lambda3(x, y, z) - lambda0(x, y, z)) * phi0(lambda0(x, y, z) - lambda1(x, y, z));
}

static Real
lobatto_f18_2(Real x, Real y, Real z)
{
    return lambda3(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
           phi0(lambda1(x, y, z) - lambda3(x, y, z)) * phi0(lambda3(x, y, z) - lambda0(x, y, z));
}

static Real
lobatto_f18_3(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
           phi0(lambda3(x, y, z) - lambda1(x, y, z)) * phi0(lambda1(x, y, z) - lambda0(x, y, z));
}

static Real
lobatto_f18_4(Real x, Real y, Real z)
{
    return lambda0(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
           phi0(lambda1(x, y, z) - lambda0(x, y, z)) * phi0(lambda0(x, y, z) - lambda3(x, y, z));
}

static Real
lobatto_f18_5(Real x, Real y, Real z)
{
    return lambda3(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
           phi0(lambda0(x, y, z) - lambda3(x, y, z)) * phi0(lambda3(x, y, z) - lambda1(x, y, z));
}

// face 3
static Real
lobatto_f19_0(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
           phi0(lambda2(x, y, z) - lambda1(x, y, z)) * phi0(lambda1(x, y, z) - lambda0(x, y, z));
}

static Real
lobatto_f19_1(Real x, Real y, Real z)
{
    return lambda2(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
           phi0(lambda0(x, y, z) - lambda2(x, y, z)) * phi0(lambda2(x, y, z) - lambda1(x, y, z));
}

static Real
lobatto_f19_2(Real x, Real y, Real z)
{
    return lambda0(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
           phi0(lambda1(x, y, z) - lambda0(x, y, z)) * phi0(lambda0(x, y, z) - lambda2(x, y, z));
}

static Real
lobatto_f19_3(Real x, Real y, Real z)
{
    return lambda1(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
           phi0(lambda0(x, y, z) - lambda1(x, y, z)) * phi0(lambda1(x, y, z) - lambda2(x, y, z));
}

static Real
lobatto_f19_4(Real x, Real y, Real z)
{
    return lambda2(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
           phi0(lambda1(x, y, z) - lambda2(x, y, z)) * phi0(lambda2(x, y, z) - lambda0(x, y, z));
}

static Real
lobatto_f19_5(Real x, Real y, Real z)
{
    return lambda0(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
           phi0(lambda2(x, y, z) - lambda0(x, y, z)) * phi0(lambda0(x, y, z) - lambda1(x, y, z));
}

static shape_fn_3d_t lobatto_tetra_fn[] = {
    lobatto_f0,    lobatto_f1,    lobatto_f2,    lobatto_f3,    lobatto_f4,    lobatto_f5,
    lobatto_f6,    lobatto_f7,    lobatto_f8,    lobatto_f9,    lobatto_f10_0, lobatto_f10_1,
    lobatto_f11_0, lobatto_f11_1, lobatto_f12_0, lobatto_f12_1, lobatto_f13_0, lobatto_f13_1,
    lobatto_f14_0, lobatto_f14_1, lobatto_f15_0, lobatto_f15_1, lobatto_f16_0, lobatto_f16_1,
    lobatto_f16_2, lobatto_f16_3, lobatto_f16_4, lobatto_f16_5, lobatto_f17_0, lobatto_f17_1,
    lobatto_f17_2, lobatto_f17_3, lobatto_f17_4, lobatto_f17_5, lobatto_f18_0, lobatto_f18_1,
    lobatto_f18_2, lobatto_f18_3, lobatto_f18_4, lobatto_f18_5, lobatto_f19_0, lobatto_f19_1,
    lobatto_f19_2, lobatto_f19_3, lobatto_f19_4, lobatto_f19_5
};

// vertices //

static uint lobatto_tetra_vertex_indices[] = { 0, 1, 2, 3 };

// edges //

static uint lobatto_tetra_edge_indices_0_0[] = { 4, 10 };
static uint lobatto_tetra_edge_indices_0_1[] = { 4, 11 };

static uint lobatto_tetra_edge_indices_1_0[] = { 5, 12 };
static uint lobatto_tetra_edge_indices_1_1[] = { 5, 13 };

static uint lobatto_tetra_edge_indices_2_0[] = { 6, 14 };
static uint lobatto_tetra_edge_indices_2_1[] = { 6, 15 };

static uint lobatto_tetra_edge_indices_3_0[] = { 7, 16 };
static uint lobatto_tetra_edge_indices_3_1[] = { 7, 17 };

static uint lobatto_tetra_edge_indices_4_0[] = { 8, 18 };
static uint lobatto_tetra_edge_indices_4_1[] = { 8, 19 };

static uint lobatto_tetra_edge_indices_5_0[] = { 9, 20 };
static uint lobatto_tetra_edge_indices_5_1[] = { 9, 21 };

static uint * lobatto_tetra_edge_indices_0[] = { lobatto_tetra_edge_indices_0_0,
                                                 lobatto_tetra_edge_indices_0_1 };
static uint * lobatto_tetra_edge_indices_1[] = { lobatto_tetra_edge_indices_1_0,
                                                 lobatto_tetra_edge_indices_1_1 };
static uint * lobatto_tetra_edge_indices_2[] = { lobatto_tetra_edge_indices_2_0,
                                                 lobatto_tetra_edge_indices_2_1 };
static uint * lobatto_tetra_edge_indices_3[] = { lobatto_tetra_edge_indices_3_0,
                                                 lobatto_tetra_edge_indices_3_1 };
static uint * lobatto_tetra_edge_indices_4[] = { lobatto_tetra_edge_indices_4_0,
                                                 lobatto_tetra_edge_indices_4_1 };
static uint * lobatto_tetra_edge_indices_5[] = { lobatto_tetra_edge_indices_5_0,
                                                 lobatto_tetra_edge_indices_5_1 };

static uint ** lobatto_tetra_edge_indices[] = {
    lobatto_tetra_edge_indices_0, lobatto_tetra_edge_indices_1, lobatto_tetra_edge_indices_2,
    lobatto_tetra_edge_indices_3, lobatto_tetra_edge_indices_4, lobatto_tetra_edge_indices_5
};

static uint lobatto_tetra_edge_count[] = { 0, 0, 1, 1 };

// faces //

static uint lobatto_tetra_face_indices_0_0[] = { 22 };
static uint lobatto_tetra_face_indices_0_1[] = { 23 };
static uint lobatto_tetra_face_indices_0_2[] = { 24 };
static uint lobatto_tetra_face_indices_0_3[] = { 25 };
static uint lobatto_tetra_face_indices_0_4[] = { 26 };
static uint lobatto_tetra_face_indices_0_5[] = { 27 };

static uint lobatto_tetra_face_indices_1_0[] = { 28 };
static uint lobatto_tetra_face_indices_1_1[] = { 29 };
static uint lobatto_tetra_face_indices_1_2[] = { 30 };
static uint lobatto_tetra_face_indices_1_3[] = { 31 };
static uint lobatto_tetra_face_indices_1_4[] = { 32 };
static uint lobatto_tetra_face_indices_1_5[] = { 33 };

static uint lobatto_tetra_face_indices_2_0[] = { 34 };
static uint lobatto_tetra_face_indices_2_1[] = { 35 };
static uint lobatto_tetra_face_indices_2_2[] = { 36 };
static uint lobatto_tetra_face_indices_2_3[] = { 37 };
static uint lobatto_tetra_face_indices_2_4[] = { 38 };
static uint lobatto_tetra_face_indices_2_5[] = { 39 };

static uint lobatto_tetra_face_indices_3_0[] = { 40 };
static uint lobatto_tetra_face_indices_3_1[] = { 41 };
static uint lobatto_tetra_face_indices_3_2[] = { 42 };
static uint lobatto_tetra_face_indices_3_3[] = { 43 };
static uint lobatto_tetra_face_indices_3_4[] = { 44 };
static uint lobatto_tetra_face_indices_3_5[] = { 45 };

static uint * lobatto_tetra_face_indices_0[] = {
    lobatto_tetra_face_indices_0_0, lobatto_tetra_face_indices_0_1, lobatto_tetra_face_indices_0_2,
    lobatto_tetra_face_indices_0_3, lobatto_tetra_face_indices_0_4, lobatto_tetra_face_indices_0_5
};
static uint * lobatto_tetra_face_indices_1[] = {
    lobatto_tetra_face_indices_1_0, lobatto_tetra_face_indices_1_1, lobatto_tetra_face_indices_1_2,
    lobatto_tetra_face_indices_1_3, lobatto_tetra_face_indices_1_4, lobatto_tetra_face_indices_1_5
};
static uint * lobatto_tetra_face_indices_2[] = {
    lobatto_tetra_face_indices_2_0, lobatto_tetra_face_indices_2_1, lobatto_tetra_face_indices_2_2,
    lobatto_tetra_face_indices_2_3, lobatto_tetra_face_indices_2_4, lobatto_tetra_face_indices_2_5
};
static uint * lobatto_tetra_face_indices_3[] = {
    lobatto_tetra_face_indices_3_0, lobatto_tetra_face_indices_3_1, lobatto_tetra_face_indices_3_2,
    lobatto_tetra_face_indices_3_3, lobatto_tetra_face_indices_3_4, lobatto_tetra_face_indices_3_5
};

static uint ** lobatto_tetra_face_indices[] = {
    lobatto_tetra_face_indices_0,
    lobatto_tetra_face_indices_1,
    lobatto_tetra_face_indices_2,
    lobatto_tetra_face_indices_3,
};

static uint lobatto_tetra_face_count[] = { 0, 0, 0, 1 };

// bubbles //

static uint * lobatto_tetra_bubble_indices[] = { nullptr, nullptr, nullptr, nullptr };

static uint lobatto_tetra_bubble_count[] = { 0, 0, 0, 0 };

static uint lobatto_tetra_index_to_order[] = {
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
};

// Derivatives of the shape functions (dx)

// DEGREE 1

// Vertex shape functions, degree 1

static Real
lobatto_dx_f0(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z);
}

static Real
lobatto_dx_f1(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z);
}

static Real
lobatto_dx_f2(Real x, Real y, Real z)
{
    return lambda0dx(x, y, z);
}

static Real
lobatto_dx_f3(Real x, Real y, Real z)
{
    return lambda3dx(x, y, z);
}

// DEGREE 2

// Edge shape functions, degree 2

// edge 0
static Real
lobatto_dx_f4(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda2(x, y, z) * phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2dx(x, y, z) * phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dx(x, y, z) - lambda2dx(x, y, z));
}

// edge 1
static Real
lobatto_dx_f5(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z) * lambda0(x, y, z) * phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0dx(x, y, z) * phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dx(x, y, z) - lambda0dx(x, y, z));
}

// edge 2
static Real
lobatto_dx_f6(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda0(x, y, z) * phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0dx(x, y, z) * phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dx(x, y, z) - lambda0dx(x, y, z));
}

// edge 3
static Real
lobatto_dx_f7(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda3(x, y, z) * phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3dx(x, y, z) * phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dx(x, y, z) - lambda3dx(x, y, z));
}

// edge 4
static Real
lobatto_dx_f8(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z) * lambda3(x, y, z) * phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3dx(x, y, z) * phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dx(x, y, z) - lambda3dx(x, y, z));
}

// edge 5
static Real
lobatto_dx_f9(Real x, Real y, Real z)
{
    return lambda0dx(x, y, z) * lambda3(x, y, z) * phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3dx(x, y, z) * phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dx(x, y, z) - lambda3dx(x, y, z));
}

// DEGREE 3

// Edge shape functions, degree 3

// edge 0
static Real
lobatto_dx_f10_0(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda2(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2dx(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * phi1dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dx(x, y, z) - lambda2dx(x, y, z));
}

static Real
lobatto_dx_f10_1(Real x, Real y, Real z)
{
    return -(lambda1dx(x, y, z) * lambda2(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
             lambda1(x, y, z) * lambda2dx(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
             lambda1(x, y, z) * lambda2(x, y, z) * phi1dx(lambda1(x, y, z) - lambda2(x, y, z)) *
                 (lambda1dx(x, y, z) - lambda2dx(x, y, z)));
}

// edge 1
static Real
lobatto_dx_f11_0(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z) * lambda0(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0dx(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * phi1dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dx(x, y, z) - lambda0dx(x, y, z));
}

static Real
lobatto_dx_f11_1(Real x, Real y, Real z)
{
    return -(lambda2dx(x, y, z) * lambda0(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
             lambda2(x, y, z) * lambda0dx(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
             lambda2(x, y, z) * lambda0(x, y, z) * phi1dx(lambda2(x, y, z) - lambda0(x, y, z)) *
                 (lambda2dx(x, y, z) - lambda0dx(x, y, z)));
}

// edge 2
static Real
lobatto_dx_f12_0(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda0(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0dx(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * phi1dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dx(x, y, z) - lambda0dx(x, y, z));
}

static Real
lobatto_dx_f12_1(Real x, Real y, Real z)
{
    return -(lambda1dx(x, y, z) * lambda0(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
             lambda1(x, y, z) * lambda0dx(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
             lambda1(x, y, z) * lambda0(x, y, z) * phi1dx(lambda1(x, y, z) - lambda0(x, y, z)) *
                 (lambda1dx(x, y, z) - lambda0dx(x, y, z)));
}

// edge 3
static Real
lobatto_dx_f13_0(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda3(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3dx(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * phi1dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dx(x, y, z) - lambda3dx(x, y, z));
}

static Real
lobatto_dx_f13_1(Real x, Real y, Real z)
{
    return -(lambda1dx(x, y, z) * lambda3(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
             lambda1(x, y, z) * lambda3dx(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
             lambda1(x, y, z) * lambda3(x, y, z) * phi1dx(lambda1(x, y, z) - lambda3(x, y, z)) *
                 (lambda1dx(x, y, z) - lambda3dx(x, y, z)));
}

// edge 4
static Real
lobatto_dx_f14_0(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z) * lambda3(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3dx(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * phi1dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dx(x, y, z) - lambda3dx(x, y, z));
}

static Real
lobatto_dx_f14_1(Real x, Real y, Real z)
{
    return -(lambda2dx(x, y, z) * lambda3(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
             lambda2(x, y, z) * lambda3dx(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
             lambda2(x, y, z) * lambda3(x, y, z) * phi1dx(lambda2(x, y, z) - lambda3(x, y, z)) *
                 (lambda2dx(x, y, z) - lambda3dx(x, y, z)));
}

// edge 5
static Real
lobatto_dx_f15_0(Real x, Real y, Real z)
{
    return lambda0dx(x, y, z) * lambda3(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3dx(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * phi1dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dx(x, y, z) - lambda3dx(x, y, z));
}

static Real
lobatto_dx_f15_1(Real x, Real y, Real z)
{
    return -(lambda0dx(x, y, z) * lambda3(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
             lambda0(x, y, z) * lambda3dx(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
             lambda0(x, y, z) * lambda3(x, y, z) * phi1dx(lambda0(x, y, z) - lambda3(x, y, z)) *
                 (lambda0dx(x, y, z) - lambda3dx(x, y, z)));
}

// Face shape functions, degree 3

// face 0
static Real
lobatto_dx_f16_0(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2dx(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda3dx(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dx(x, y, z) - lambda1dx(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dx(x, y, z) - lambda3dx(x, y, z));
}

static Real
lobatto_dx_f16_1(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3dx(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda1dx(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dx(x, y, z) - lambda2dx(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dx(x, y, z) - lambda1dx(x, y, z));
}

static Real
lobatto_dx_f16_2(Real x, Real y, Real z)
{
    return lambda3dx(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1dx(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda2dx(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dx(x, y, z) - lambda3dx(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dx(x, y, z) - lambda2dx(x, y, z));
}

static Real
lobatto_dx_f16_3(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3dx(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda2dx(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dx(x, y, z) - lambda1dx(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dx(x, y, z) - lambda2dx(x, y, z));
}

static Real
lobatto_dx_f16_4(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1dx(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda3dx(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dx(x, y, z) - lambda2dx(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dx(x, y, z) - lambda3dx(x, y, z));
}

static Real
lobatto_dx_f16_5(Real x, Real y, Real z)
{
    return lambda3dx(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2dx(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda1dx(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dx(x, y, z) - lambda3dx(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dx(x, y, z) - lambda1dx(x, y, z));
}

// face 1
static Real
lobatto_dx_f17_0(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0dx(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda3dx(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dx(x, y, z) - lambda2dx(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dx(x, y, z) - lambda3dx(x, y, z));
}

static Real
lobatto_dx_f17_1(Real x, Real y, Real z)
{
    return lambda0dx(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3dx(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda2dx(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dx(x, y, z) - lambda0dx(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dx(x, y, z) - lambda2dx(x, y, z));
}

static Real
lobatto_dx_f17_2(Real x, Real y, Real z)
{
    return lambda3dx(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2dx(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda0dx(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dx(x, y, z) - lambda3dx(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dx(x, y, z) - lambda0dx(x, y, z));
}

static Real
lobatto_dx_f17_3(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3dx(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda0dx(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dx(x, y, z) - lambda2dx(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dx(x, y, z) - lambda0dx(x, y, z));
}

static Real
lobatto_dx_f17_4(Real x, Real y, Real z)
{
    return lambda0dx(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2dx(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda3dx(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dx(x, y, z) - lambda0dx(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dx(x, y, z) - lambda3dx(x, y, z));
}

static Real
lobatto_dx_f17_5(Real x, Real y, Real z)
{
    return lambda3dx(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0dx(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda2dx(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dx(x, y, z) - lambda3dx(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dx(x, y, z) - lambda2dx(x, y, z));
}

// face 2
static Real
lobatto_dx_f18_0(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0dx(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda3dx(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dx(x, y, z) - lambda1dx(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dx(x, y, z) - lambda3dx(x, y, z));
}

static Real
lobatto_dx_f18_1(Real x, Real y, Real z)
{
    return lambda0dx(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3dx(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda1dx(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dx(x, y, z) - lambda0dx(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dx(x, y, z) - lambda1dx(x, y, z));
}

static Real
lobatto_dx_f18_2(Real x, Real y, Real z)
{
    return lambda3dx(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1dx(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda0dx(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dx(x, y, z) - lambda3dx(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dx(x, y, z) - lambda0dx(x, y, z));
}

static Real
lobatto_dx_f18_3(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3dx(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda0dx(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dx(x, y, z) - lambda1dx(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dx(x, y, z) - lambda0dx(x, y, z));
}

static Real
lobatto_dx_f18_4(Real x, Real y, Real z)
{
    return lambda0dx(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1dx(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda3dx(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dx(x, y, z) - lambda0dx(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dx(x, y, z) - lambda3dx(x, y, z));
}

static Real
lobatto_dx_f18_5(Real x, Real y, Real z)
{
    return lambda3dx(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0dx(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda1dx(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dx(x, y, z) - lambda3dx(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dx(x, y, z) - lambda1dx(x, y, z));
}

// face 3
static Real
lobatto_dx_f19_0(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2dx(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda0dx(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dx(x, y, z) - lambda1dx(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dx(x, y, z) - lambda0dx(x, y, z));
}

static Real
lobatto_dx_f19_1(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0dx(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda1dx(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dx(x, y, z) - lambda2dx(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dx(x, y, z) - lambda1dx(x, y, z));
}

static Real
lobatto_dx_f19_2(Real x, Real y, Real z)
{
    return lambda0dx(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1dx(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda2dx(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dx(x, y, z) - lambda0dx(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dx(x, y, z) - lambda2dx(x, y, z));
}

static Real
lobatto_dx_f19_3(Real x, Real y, Real z)
{
    return lambda1dx(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0dx(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda2dx(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dx(x, y, z) - lambda1dx(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dx(x, y, z) - lambda2dx(x, y, z));
}

static Real
lobatto_dx_f19_4(Real x, Real y, Real z)
{
    return lambda2dx(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1dx(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda0dx(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dx(x, y, z) - lambda2dx(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dx(x, y, z) - lambda0dx(x, y, z));
}

static Real
lobatto_dx_f19_5(Real x, Real y, Real z)
{
    return lambda0dx(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2dx(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda1dx(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dx(x, y, z) - lambda0dx(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dx(x, y, z) - lambda1dx(x, y, z));
}

static shape_fn_3d_t lobatto_tetra_dx[] = {
    lobatto_dx_f0,    lobatto_dx_f1,    lobatto_dx_f2,    lobatto_dx_f3,    lobatto_dx_f4,
    lobatto_dx_f5,    lobatto_dx_f6,    lobatto_dx_f7,    lobatto_dx_f8,    lobatto_dx_f9,
    lobatto_dx_f10_0, lobatto_dx_f10_1, lobatto_dx_f11_0, lobatto_dx_f11_1, lobatto_dx_f12_0,
    lobatto_dx_f12_1, lobatto_dx_f13_0, lobatto_dx_f13_1, lobatto_dx_f14_0, lobatto_dx_f14_1,
    lobatto_dx_f15_0, lobatto_dx_f15_1, lobatto_dx_f16_0, lobatto_dx_f16_1, lobatto_dx_f16_2,
    lobatto_dx_f16_3, lobatto_dx_f16_4, lobatto_dx_f16_5, lobatto_dx_f17_0, lobatto_dx_f17_1,
    lobatto_dx_f17_2, lobatto_dx_f17_3, lobatto_dx_f17_4, lobatto_dx_f17_5, lobatto_dx_f18_0,
    lobatto_dx_f18_1, lobatto_dx_f18_2, lobatto_dx_f18_3, lobatto_dx_f18_4, lobatto_dx_f18_5,
    lobatto_dx_f19_0, lobatto_dx_f19_1, lobatto_dx_f19_2, lobatto_dx_f19_3, lobatto_dx_f19_4,
    lobatto_dx_f19_5
};

// Derivatives of the shape functions (dy)

// DEGREE 1

// Vertex shape functions, degree 1

static Real
lobatto_dy_f0(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z);
}

static Real
lobatto_dy_f1(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z);
}

static Real
lobatto_dy_f2(Real x, Real y, Real z)
{
    return lambda0dy(x, y, z);
}

static Real
lobatto_dy_f3(Real x, Real y, Real z)
{
    return lambda3dy(x, y, z);
}

// DEGREE 2

// Edge shape functions, degree 2

// edge 0
static Real
lobatto_dy_f4(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda2(x, y, z) * phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2dy(x, y, z) * phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dy(x, y, z) - lambda2dy(x, y, z));
}

// edge 1
static Real
lobatto_dy_f5(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z) * lambda0(x, y, z) * phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0dy(x, y, z) * phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dy(x, y, z) - lambda0dy(x, y, z));
}

// edge 2
static Real
lobatto_dy_f6(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda0(x, y, z) * phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0dy(x, y, z) * phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dy(x, y, z) - lambda0dy(x, y, z));
}

// edge 3
static Real
lobatto_dy_f7(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda3(x, y, z) * phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3dy(x, y, z) * phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dy(x, y, z) - lambda3dy(x, y, z));
}

// edge 4
static Real
lobatto_dy_f8(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z) * lambda3(x, y, z) * phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3dy(x, y, z) * phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dy(x, y, z) - lambda3dy(x, y, z));
}

// edge 5
static Real
lobatto_dy_f9(Real x, Real y, Real z)
{
    return lambda0dy(x, y, z) * lambda3(x, y, z) * phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3dy(x, y, z) * phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dy(x, y, z) - lambda3dy(x, y, z));
}

// DEGREE 4

// Edge shape functions, degree 3

// edge 0
static Real
lobatto_dy_f10_0(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda2(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2dy(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * phi1dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dy(x, y, z) - lambda2dy(x, y, z));
}

static Real
lobatto_dy_f10_1(Real x, Real y, Real z)
{
    return -(lambda1dy(x, y, z) * lambda2(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
             lambda1(x, y, z) * lambda2dy(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
             lambda1(x, y, z) * lambda2(x, y, z) * phi1dx(lambda1(x, y, z) - lambda2(x, y, z)) *
                 (lambda1dy(x, y, z) - lambda2dy(x, y, z)));
}

// edge 1
static Real
lobatto_dy_f11_0(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z) * lambda0(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0dy(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * phi1dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dy(x, y, z) - lambda0dy(x, y, z));
}

static Real
lobatto_dy_f11_1(Real x, Real y, Real z)
{
    return -(lambda2dy(x, y, z) * lambda0(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
             lambda2(x, y, z) * lambda0dy(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
             lambda2(x, y, z) * lambda0(x, y, z) * phi1dx(lambda2(x, y, z) - lambda0(x, y, z)) *
                 (lambda2dy(x, y, z) - lambda0dy(x, y, z)));
}

// edge 2
static Real
lobatto_dy_f12_0(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda0(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0dy(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * phi1dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dy(x, y, z) - lambda0dy(x, y, z));
}

static Real
lobatto_dy_f12_1(Real x, Real y, Real z)
{
    return -(lambda1dy(x, y, z) * lambda0(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
             lambda1(x, y, z) * lambda0dy(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
             lambda1(x, y, z) * lambda0(x, y, z) * phi1dx(lambda1(x, y, z) - lambda0(x, y, z)) *
                 (lambda1dy(x, y, z) - lambda0dy(x, y, z)));
}

// edge 3
static Real
lobatto_dy_f13_0(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda3(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3dy(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * phi1dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dy(x, y, z) - lambda3dy(x, y, z));
}

static Real
lobatto_dy_f13_1(Real x, Real y, Real z)
{
    return -(lambda1dy(x, y, z) * lambda3(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
             lambda1(x, y, z) * lambda3dy(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
             lambda1(x, y, z) * lambda3(x, y, z) * phi1dx(lambda1(x, y, z) - lambda3(x, y, z)) *
                 (lambda1dy(x, y, z) - lambda3dy(x, y, z)));
}

// edge 4
static Real
lobatto_dy_f14_0(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z) * lambda3(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3dy(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * phi1dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dy(x, y, z) - lambda3dy(x, y, z));
}

static Real
lobatto_dy_f14_1(Real x, Real y, Real z)
{
    return -(lambda2dy(x, y, z) * lambda3(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
             lambda2(x, y, z) * lambda3dy(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
             lambda2(x, y, z) * lambda3(x, y, z) * phi1dx(lambda2(x, y, z) - lambda3(x, y, z)) *
                 (lambda2dy(x, y, z) - lambda3dy(x, y, z)));
}

// edge 5
static Real
lobatto_dy_f15_0(Real x, Real y, Real z)
{
    return lambda0dy(x, y, z) * lambda3(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3dy(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * phi1dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dy(x, y, z) - lambda3dy(x, y, z));
}

static Real
lobatto_dy_f15_1(Real x, Real y, Real z)
{
    return -(lambda0dy(x, y, z) * lambda3(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
             lambda0(x, y, z) * lambda3dy(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
             lambda0(x, y, z) * lambda3(x, y, z) * phi1dx(lambda0(x, y, z) - lambda3(x, y, z)) *
                 (lambda0dy(x, y, z) - lambda3dy(x, y, z)));
}

// Face shape functions, degree 3

// face 0
static Real
lobatto_dy_f16_0(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2dy(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda3dy(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dy(x, y, z) - lambda1dy(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dy(x, y, z) - lambda3dy(x, y, z));
}

static Real
lobatto_dy_f16_1(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3dy(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda1dy(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dy(x, y, z) - lambda2dy(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dy(x, y, z) - lambda1dy(x, y, z));
}

static Real
lobatto_dy_f16_2(Real x, Real y, Real z)
{
    return lambda3dy(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1dy(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda2dy(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dy(x, y, z) - lambda3dy(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dy(x, y, z) - lambda2dy(x, y, z));
}

static Real
lobatto_dy_f16_3(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3dy(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda2dy(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dy(x, y, z) - lambda1dy(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dy(x, y, z) - lambda2dy(x, y, z));
}

static Real
lobatto_dy_f16_4(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1dy(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda3dy(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dy(x, y, z) - lambda2dy(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dy(x, y, z) - lambda3dy(x, y, z));
}

static Real
lobatto_dy_f16_5(Real x, Real y, Real z)
{
    return lambda3dy(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2dy(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda1dy(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dy(x, y, z) - lambda3dy(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dy(x, y, z) - lambda1dy(x, y, z));
}

// face 1
static Real
lobatto_dy_f17_0(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0dy(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda3dy(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dy(x, y, z) - lambda2dy(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dy(x, y, z) - lambda3dy(x, y, z));
}

static Real
lobatto_dy_f17_1(Real x, Real y, Real z)
{
    return lambda0dy(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3dy(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda2dy(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dy(x, y, z) - lambda0dy(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dy(x, y, z) - lambda2dy(x, y, z));
}

static Real
lobatto_dy_f17_2(Real x, Real y, Real z)
{
    return lambda3dy(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2dy(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda0dy(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dy(x, y, z) - lambda3dy(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dy(x, y, z) - lambda0dy(x, y, z));
}

static Real
lobatto_dy_f17_3(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3dy(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda0dy(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dy(x, y, z) - lambda2dy(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dy(x, y, z) - lambda0dy(x, y, z));
}

static Real
lobatto_dy_f17_4(Real x, Real y, Real z)
{
    return lambda0dy(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2dy(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda3dy(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dy(x, y, z) - lambda0dy(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dy(x, y, z) - lambda3dy(x, y, z));
}

static Real
lobatto_dy_f17_5(Real x, Real y, Real z)
{
    return lambda3dy(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0dy(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda2dy(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dy(x, y, z) - lambda3dy(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dy(x, y, z) - lambda2dy(x, y, z));
}

// face 2
static Real
lobatto_dy_f18_0(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0dy(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda3dy(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dy(x, y, z) - lambda1dy(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dy(x, y, z) - lambda3dy(x, y, z));
}

static Real
lobatto_dy_f18_1(Real x, Real y, Real z)
{
    return lambda0dy(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3dy(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda1dy(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dy(x, y, z) - lambda0dy(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dy(x, y, z) - lambda1dy(x, y, z));
}

static Real
lobatto_dy_f18_2(Real x, Real y, Real z)
{
    return lambda3dy(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1dy(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda0dy(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dy(x, y, z) - lambda3dy(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dy(x, y, z) - lambda0dy(x, y, z));
}

static Real
lobatto_dy_f18_3(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3dy(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda0dy(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dy(x, y, z) - lambda1dy(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dy(x, y, z) - lambda0dy(x, y, z));
}

static Real
lobatto_dy_f18_4(Real x, Real y, Real z)
{
    return lambda0dy(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1dy(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda3dy(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dy(x, y, z) - lambda0dy(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dy(x, y, z) - lambda3dy(x, y, z));
}

static Real
lobatto_dy_f18_5(Real x, Real y, Real z)
{
    return lambda3dy(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0dy(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda1dy(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dy(x, y, z) - lambda3dy(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dy(x, y, z) - lambda1dy(x, y, z));
}

// face 3
static Real
lobatto_dy_f19_0(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2dy(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda0dy(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dy(x, y, z) - lambda1dy(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dy(x, y, z) - lambda0dy(x, y, z));
}

static Real
lobatto_dy_f19_1(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0dy(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda1dy(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dy(x, y, z) - lambda2dy(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dy(x, y, z) - lambda1dy(x, y, z));
}

static Real
lobatto_dy_f19_2(Real x, Real y, Real z)
{
    return lambda0dy(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1dy(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda2dy(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dy(x, y, z) - lambda0dy(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dy(x, y, z) - lambda2dy(x, y, z));
}

static Real
lobatto_dy_f19_3(Real x, Real y, Real z)
{
    return lambda1dy(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0dy(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda2dy(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dy(x, y, z) - lambda1dy(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dy(x, y, z) - lambda2dy(x, y, z));
}

static Real
lobatto_dy_f19_4(Real x, Real y, Real z)
{
    return lambda2dy(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1dy(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda0dy(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dy(x, y, z) - lambda2dy(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dy(x, y, z) - lambda0dy(x, y, z));
}

static Real
lobatto_dy_f19_5(Real x, Real y, Real z)
{
    return lambda0dy(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2dy(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda1dy(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dy(x, y, z) - lambda0dy(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dy(x, y, z) - lambda1dy(x, y, z));
}

static shape_fn_3d_t lobatto_tetra_dy[] = {
    lobatto_dy_f0,    lobatto_dy_f1,    lobatto_dy_f2,    lobatto_dy_f3,    lobatto_dy_f4,
    lobatto_dy_f5,    lobatto_dy_f6,    lobatto_dy_f7,    lobatto_dy_f8,    lobatto_dy_f9,
    lobatto_dy_f10_0, lobatto_dy_f10_1, lobatto_dy_f11_0, lobatto_dy_f11_1, lobatto_dy_f12_0,
    lobatto_dy_f12_1, lobatto_dy_f13_0, lobatto_dy_f13_1, lobatto_dy_f14_0, lobatto_dy_f14_1,
    lobatto_dy_f15_0, lobatto_dy_f15_1, lobatto_dy_f16_0, lobatto_dy_f16_1, lobatto_dy_f16_2,
    lobatto_dy_f16_3, lobatto_dy_f16_4, lobatto_dy_f16_5, lobatto_dy_f17_0, lobatto_dy_f17_1,
    lobatto_dy_f17_2, lobatto_dy_f17_3, lobatto_dy_f17_4, lobatto_dy_f17_5, lobatto_dy_f18_0,
    lobatto_dy_f18_1, lobatto_dy_f18_2, lobatto_dy_f18_3, lobatto_dy_f18_4, lobatto_dy_f18_5,
    lobatto_dy_f19_0, lobatto_dy_f19_1, lobatto_dy_f19_2, lobatto_dy_f19_3, lobatto_dy_f19_4,
    lobatto_dy_f19_5
};

// Derivatives of the shape functions (dz)

// DEGREE 1

// Vertex shape functions, degree 1

static Real
lobatto_dz_f0(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z);
}

static Real
lobatto_dz_f1(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z);
}

static Real
lobatto_dz_f2(Real x, Real y, Real z)
{
    return lambda0dz(x, y, z);
}

static Real
lobatto_dz_f3(Real x, Real y, Real z)
{
    return lambda3dz(x, y, z);
}

// DEGREE 2

// Edge shape functions, degree 2

// edge 0
static Real
lobatto_dz_f4(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda2(x, y, z) * phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2dz(x, y, z) * phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dz(x, y, z) - lambda2dz(x, y, z));
}

// edge 1
static Real
lobatto_dz_f5(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z) * lambda0(x, y, z) * phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0dz(x, y, z) * phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dz(x, y, z) - lambda0dz(x, y, z));
}

// edge 2
static Real
lobatto_dz_f6(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda0(x, y, z) * phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0dz(x, y, z) * phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dz(x, y, z) - lambda0dz(x, y, z));
}

// edge 3
static Real
lobatto_dz_f7(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda3(x, y, z) * phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3dz(x, y, z) * phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dz(x, y, z) - lambda3dz(x, y, z));
}

// edge 4
static Real
lobatto_dz_f8(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z) * lambda3(x, y, z) * phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3dz(x, y, z) * phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dz(x, y, z) - lambda3dz(x, y, z));
}

// edge 5
static Real
lobatto_dz_f9(Real x, Real y, Real z)
{
    return lambda0dz(x, y, z) * lambda3(x, y, z) * phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3dz(x, y, z) * phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dz(x, y, z) - lambda3dz(x, y, z));
}

// DEGREE 3

// Edge shape functions, degree 3

// edge 0
static Real
lobatto_dz_f10_0(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda2(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2dz(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * phi1dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dz(x, y, z) - lambda2dz(x, y, z));
}

static Real
lobatto_dz_f10_1(Real x, Real y, Real z)
{
    return -(lambda1dz(x, y, z) * lambda2(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
             lambda1(x, y, z) * lambda2dz(x, y, z) * phi1(lambda1(x, y, z) - lambda2(x, y, z)) +
             lambda1(x, y, z) * lambda2(x, y, z) * phi1dx(lambda1(x, y, z) - lambda2(x, y, z)) *
                 (lambda1dz(x, y, z) - lambda2dz(x, y, z)));
}

// edge 1
static Real
lobatto_dz_f11_0(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z) * lambda0(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0dz(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * phi1dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dz(x, y, z) - lambda0dz(x, y, z));
}

static Real
lobatto_dz_f11_1(Real x, Real y, Real z)
{
    return -(lambda2dz(x, y, z) * lambda0(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
             lambda2(x, y, z) * lambda0dz(x, y, z) * phi1(lambda2(x, y, z) - lambda0(x, y, z)) +
             lambda2(x, y, z) * lambda0(x, y, z) * phi1dx(lambda2(x, y, z) - lambda0(x, y, z)) *
                 (lambda2dz(x, y, z) - lambda0dz(x, y, z)));
}

// edge 2
static Real
lobatto_dz_f12_0(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda0(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0dz(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * phi1dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dz(x, y, z) - lambda0dz(x, y, z));
}

static Real
lobatto_dz_f12_1(Real x, Real y, Real z)
{
    return -(lambda1dz(x, y, z) * lambda0(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
             lambda1(x, y, z) * lambda0dz(x, y, z) * phi1(lambda1(x, y, z) - lambda0(x, y, z)) +
             lambda1(x, y, z) * lambda0(x, y, z) * phi1dx(lambda1(x, y, z) - lambda0(x, y, z)) *
                 (lambda1dz(x, y, z) - lambda0dz(x, y, z)));
}

// edge 3
static Real
lobatto_dz_f13_0(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda3(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3dz(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * phi1dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dz(x, y, z) - lambda3dz(x, y, z));
}

static Real
lobatto_dz_f13_1(Real x, Real y, Real z)
{
    return -(lambda1dz(x, y, z) * lambda3(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
             lambda1(x, y, z) * lambda3dz(x, y, z) * phi1(lambda1(x, y, z) - lambda3(x, y, z)) +
             lambda1(x, y, z) * lambda3(x, y, z) * phi1dx(lambda1(x, y, z) - lambda3(x, y, z)) *
                 (lambda1dz(x, y, z) - lambda3dz(x, y, z)));
}

// edge 4
static Real
lobatto_dz_f14_0(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z) * lambda3(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3dz(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * phi1dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dz(x, y, z) - lambda3dz(x, y, z));
}

static Real
lobatto_dz_f14_1(Real x, Real y, Real z)
{
    return -(lambda2dz(x, y, z) * lambda3(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
             lambda2(x, y, z) * lambda3dz(x, y, z) * phi1(lambda2(x, y, z) - lambda3(x, y, z)) +
             lambda2(x, y, z) * lambda3(x, y, z) * phi1dx(lambda2(x, y, z) - lambda3(x, y, z)) *
                 (lambda2dz(x, y, z) - lambda3dz(x, y, z)));
}

// edge 5
static Real
lobatto_dz_f15_0(Real x, Real y, Real z)
{
    return lambda0dz(x, y, z) * lambda3(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3dz(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * phi1dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dz(x, y, z) - lambda3dz(x, y, z));
}

static Real
lobatto_dz_f15_1(Real x, Real y, Real z)
{
    return -(lambda0dz(x, y, z) * lambda3(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
             lambda0(x, y, z) * lambda3dz(x, y, z) * phi1(lambda0(x, y, z) - lambda3(x, y, z)) +
             lambda0(x, y, z) * lambda3(x, y, z) * phi1dx(lambda0(x, y, z) - lambda3(x, y, z)) *
                 (lambda0dz(x, y, z) - lambda3dz(x, y, z)));
}

// Face shape functions, degree 3

// face 0
static Real
lobatto_dz_f16_0(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2dz(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda3dz(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dz(x, y, z) - lambda1dz(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dz(x, y, z) - lambda3dz(x, y, z));
}

static Real
lobatto_dz_f16_1(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3dz(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda1dz(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dz(x, y, z) - lambda2dz(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dz(x, y, z) - lambda1dz(x, y, z));
}

static Real
lobatto_dz_f16_2(Real x, Real y, Real z)
{
    return lambda3dz(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1dz(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda2dz(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dz(x, y, z) - lambda3dz(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dz(x, y, z) - lambda2dz(x, y, z));
}

static Real
lobatto_dz_f16_3(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3dz(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda2dz(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dz(x, y, z) - lambda1dz(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dz(x, y, z) - lambda2dz(x, y, z));
}

static Real
lobatto_dz_f16_4(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1dz(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda3dz(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dz(x, y, z) - lambda2dz(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dz(x, y, z) - lambda3dz(x, y, z));
}

static Real
lobatto_dz_f16_5(Real x, Real y, Real z)
{
    return lambda3dz(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2dz(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda1dz(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dz(x, y, z) - lambda3dz(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dz(x, y, z) - lambda1dz(x, y, z));
}

// face 1
static Real
lobatto_dz_f17_0(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0dz(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda3dz(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dz(x, y, z) - lambda2dz(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dz(x, y, z) - lambda3dz(x, y, z));
}

static Real
lobatto_dz_f17_1(Real x, Real y, Real z)
{
    return lambda0dz(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3dz(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda2dz(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dz(x, y, z) - lambda0dz(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda2(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dz(x, y, z) - lambda2dz(x, y, z));
}

static Real
lobatto_dz_f17_2(Real x, Real y, Real z)
{
    return lambda3dz(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2dz(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda0dz(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda3(x, y, z)) *
               (lambda2dz(x, y, z) - lambda3dz(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dz(x, y, z) - lambda0dz(x, y, z));
}

static Real
lobatto_dz_f17_3(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3dz(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda0dz(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dz(x, y, z) - lambda2dz(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dz(x, y, z) - lambda0dz(x, y, z));
}

static Real
lobatto_dz_f17_4(Real x, Real y, Real z)
{
    return lambda0dz(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2dz(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda3dz(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dz(x, y, z) - lambda0dz(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda3(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dz(x, y, z) - lambda3dz(x, y, z));
}

static Real
lobatto_dz_f17_5(Real x, Real y, Real z)
{
    return lambda3dz(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0dz(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda2dz(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dz(x, y, z) - lambda3dz(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda2(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda2(x, y, z)) *
               (lambda3dz(x, y, z) - lambda2dz(x, y, z));
}

// face 2
static Real
lobatto_dz_f18_0(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0dz(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda3dz(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dz(x, y, z) - lambda1dz(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda3(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dz(x, y, z) - lambda3dz(x, y, z));
}

static Real
lobatto_dz_f18_1(Real x, Real y, Real z)
{
    return lambda0dz(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3dz(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda1dz(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dz(x, y, z) - lambda0dz(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda3(x, y, z) * lambda1(x, y, z) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dz(x, y, z) - lambda1dz(x, y, z));
}

static Real
lobatto_dz_f18_2(Real x, Real y, Real z)
{
    return lambda3dz(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1dz(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda0dz(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda3(x, y, z)) *
               (lambda1dz(x, y, z) - lambda3dz(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda0(x, y, z)) +
           lambda3(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda0(x, y, z)) *
               (lambda3dz(x, y, z) - lambda0dz(x, y, z));
}

static Real
lobatto_dz_f18_3(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3dz(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda0dz(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dz(x, y, z) - lambda1dz(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda3(x, y, z) * lambda0(x, y, z) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dz(x, y, z) - lambda0dz(x, y, z));
}

static Real
lobatto_dz_f18_4(Real x, Real y, Real z)
{
    return lambda0dz(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1dz(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda3dz(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dz(x, y, z) - lambda0dz(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda3(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dz(x, y, z) - lambda3dz(x, y, z));
}

static Real
lobatto_dz_f18_5(Real x, Real y, Real z)
{
    return lambda3dz(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0dz(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda1dz(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda3(x, y, z)) *
               (lambda0dz(x, y, z) - lambda3dz(x, y, z)) *
               phi0(lambda3(x, y, z) - lambda1(x, y, z)) +
           lambda3(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda3(x, y, z)) *
               phi0dx(lambda3(x, y, z) - lambda1(x, y, z)) *
               (lambda3dz(x, y, z) - lambda1dz(x, y, z));
}

// face 3
static Real
lobatto_dz_f19_0(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2dz(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda0dz(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dz(x, y, z) - lambda1dz(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) +
           lambda1(x, y, z) * lambda2(x, y, z) * lambda0(x, y, z) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dz(x, y, z) - lambda0dz(x, y, z));
}

static Real
lobatto_dz_f19_1(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0dz(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda1dz(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dz(x, y, z) - lambda2dz(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda1(x, y, z)) +
           lambda2(x, y, z) * lambda0(x, y, z) * lambda1(x, y, z) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda1(x, y, z)) *
               (lambda2dz(x, y, z) - lambda1dz(x, y, z));
}

static Real
lobatto_dz_f19_2(Real x, Real y, Real z)
{
    return lambda0dz(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1dz(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda2dz(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda0(x, y, z)) *
               (lambda1dz(x, y, z) - lambda0dz(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda2(x, y, z)) +
           lambda0(x, y, z) * lambda1(x, y, z) * lambda2(x, y, z) *
               phi0(lambda1(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda2(x, y, z)) *
               (lambda0dz(x, y, z) - lambda2dz(x, y, z));
}

static Real
lobatto_dz_f19_3(Real x, Real y, Real z)
{
    return lambda1dz(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0dz(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda2dz(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dz(x, y, z) - lambda1dz(x, y, z)) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) +
           lambda1(x, y, z) * lambda0(x, y, z) * lambda2(x, y, z) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dz(x, y, z) - lambda2dz(x, y, z));
}

static Real
lobatto_dz_f19_4(Real x, Real y, Real z)
{
    return lambda2dz(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1dz(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda0dz(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0dx(lambda1(x, y, z) - lambda2(x, y, z)) *
               (lambda1dz(x, y, z) - lambda2dz(x, y, z)) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) +
           lambda2(x, y, z) * lambda1(x, y, z) * lambda0(x, y, z) *
               phi0(lambda1(x, y, z) - lambda2(x, y, z)) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dz(x, y, z) - lambda0dz(x, y, z));
}

static Real
lobatto_dz_f19_5(Real x, Real y, Real z)
{
    return lambda0dz(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2dz(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda1dz(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0dx(lambda2(x, y, z) - lambda0(x, y, z)) *
               (lambda2dz(x, y, z) - lambda0dz(x, y, z)) *
               phi0(lambda0(x, y, z) - lambda1(x, y, z)) +
           lambda0(x, y, z) * lambda2(x, y, z) * lambda1(x, y, z) *
               phi0(lambda2(x, y, z) - lambda0(x, y, z)) *
               phi0dx(lambda0(x, y, z) - lambda1(x, y, z)) *
               (lambda0dz(x, y, z) - lambda1dz(x, y, z));
}

static shape_fn_3d_t lobatto_tetra_dz[] = {
    lobatto_dz_f0,    lobatto_dz_f1,    lobatto_dz_f2,    lobatto_dz_f3,    lobatto_dz_f4,
    lobatto_dz_f5,    lobatto_dz_f6,    lobatto_dz_f7,    lobatto_dz_f8,    lobatto_dz_f9,
    lobatto_dz_f10_0, lobatto_dz_f10_1, lobatto_dz_f11_0, lobatto_dz_f11_1, lobatto_dz_f12_0,
    lobatto_dz_f12_1, lobatto_dz_f13_0, lobatto_dz_f13_1, lobatto_dz_f14_0, lobatto_dz_f14_1,
    lobatto_dz_f15_0, lobatto_dz_f15_1, lobatto_dz_f16_0, lobatto_dz_f16_1, lobatto_dz_f16_2,
    lobatto_dz_f16_3, lobatto_dz_f16_4, lobatto_dz_f16_5, lobatto_dz_f17_0, lobatto_dz_f17_1,
    lobatto_dz_f17_2, lobatto_dz_f17_3, lobatto_dz_f17_4, lobatto_dz_f17_5, lobatto_dz_f18_0,
    lobatto_dz_f18_1, lobatto_dz_f18_2, lobatto_dz_f18_3, lobatto_dz_f18_4, lobatto_dz_f18_5,
    lobatto_dz_f19_0, lobatto_dz_f19_1, lobatto_dz_f19_2, lobatto_dz_f19_3, lobatto_dz_f19_4,
    lobatto_dz_f19_5
};

static shape_fn_3d_t * lobatto_tetra_fn_table[] = { lobatto_tetra_fn };
static shape_fn_3d_t * lobatto_tetra_dx_table[] = { lobatto_tetra_dx };
static shape_fn_3d_t * lobatto_tetra_dy_table[] = { lobatto_tetra_dy };
static shape_fn_3d_t * lobatto_tetra_dz_table[] = { lobatto_tetra_dz };

H1LobattoShapesetTetra::H1LobattoShapesetTetra() : Shapeset3D(MODE_TETRAHEDRON, 1)
{
    // fn values are calculated by the tables
    this->shape_table[FN] = lobatto_tetra_fn_table;
    this->shape_table[DX] = lobatto_tetra_dx_table;
    this->shape_table[DY] = lobatto_tetra_dy_table;
    this->shape_table[DZ] = lobatto_tetra_dz_table;
    this->shape_table[DXY] = nullptr;
    this->shape_table[DXZ] = nullptr;
    this->shape_table[DYZ] = nullptr;

    this->vertex_indices = lobatto_tetra_vertex_indices;

    this->edge_indices = lobatto_tetra_edge_indices;
    this->edge_count = lobatto_tetra_edge_count;

    this->face_indices = lobatto_tetra_face_indices;
    this->face_count = lobatto_tetra_face_count;

    this->bubble_indices = lobatto_tetra_bubble_indices;
    this->bubble_count = lobatto_tetra_bubble_count;
}

H1LobattoShapesetTetra::~H1LobattoShapesetTetra() {}

uint
H1LobattoShapesetTetra::get_order(uint index) const
{
    _F_;
    return lobatto_tetra_index_to_order[index];
}

} // namespace godzilla
