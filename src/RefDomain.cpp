#include "RefDomain.h"
#include "Common.h"
#include "Error.h"

namespace godzilla {

// Edge

const Point1D RefEdge::vertices[] = { { -1.0 }, { 1.0 } };

// Triangle

const Point2D RefTri::vertices[] = { { -1.0, -1.0 }, { 1.0, -1.0 }, { -1.0, 1.0 } };

const uint2 RefTri::edge_vtcs[] = { { 0, 1 }, { 1, 2 }, { 2, 0 } };

// Quad

const Point2D RefQuad::vertices[] = { { -1.0, -1.0 }, { 1.0, -1.0 }, { 1.0, 1.0 }, { -1.0, 1.0 } };

const uint2 RefQuad::edge_vtcs[] = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 } };

// Tetra

const Point3D RefTetra::vertices[] = { { -1.0, -1.0, -1.0 },
                                       { 1.0, -1.0, -1.0 },
                                       { -1.0, 1.0, -1.0 },
                                       { -1.0, -1.0, 1.0 } };

const uint2 RefTetra::edge_vtcs[] = { { 0, 1 }, { 1, 2 }, { 2, 0 }, { 0, 3 }, { 1, 3 }, { 2, 3 } };
static uint tet_face_vtcs_0[] = { 0, 1, 3 };
static uint tet_face_vtcs_1[] = { 1, 2, 3 };
static uint tet_face_vtcs_2[] = { 2, 0, 3 };
static uint tet_face_vtcs_3[] = { 0, 2, 1 };
const uint * RefTetra::face_vtcs[] = { tet_face_vtcs_0,
                                       tet_face_vtcs_1,
                                       tet_face_vtcs_2,
                                       tet_face_vtcs_3 };
const uint RefTetra::face_nvtcs[] = { 3, 3, 3, 3 };
static uint tet_face_edges_0[] = { 0, 4, 3 };
static uint tet_face_edges_1[] = { 1, 5, 4 };
static uint tet_face_edges_2[] = { 2, 3, 5 };
static uint tet_face_edges_3[] = { 0, 2, 1 };
const uint * RefTetra::face_edges[] = { tet_face_edges_0,
                                        tet_face_edges_1,
                                        tet_face_edges_2,
                                        tet_face_edges_3 };
const uint RefTetra::face_nedges[] = { 3, 3, 3, 3 };
const EMode2D RefTetra::face_mode[] = { MODE_TRIANGLE,
                                        MODE_TRIANGLE,
                                        MODE_TRIANGLE,
                                        MODE_TRIANGLE };
const uint RefTetra::face_orientations[] = { 6, 6, 6, 6 };
const Point3D RefTetra::face_normal[] = { { 0, -1, 0 },
                                          { 1.0 / sqrt(3.0), 1.0 / sqrt(3.0), 1.0 / sqrt(3.0) },
                                          { -1, 0, 0 },
                                          { 0, 0, -1 } };

// Hex

const Point3D RefHex::vertices[] = { { -1.0, -1.0, -1.0 }, { 1.0, -1.0, -1.0 }, { 1.0, 1.0, -1.0 },
                                     { -1.0, 1.0, -1.0 },  { -1.0, -1.0, 1.0 }, { 1.0, -1.0, 1.0 },
                                     { 1.0, 1.0, 1.0 },    { -1.0, 1.0, 1.0 } };

const uint2 RefHex::edge_vtcs[] = { { 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 }, { 0, 4 }, { 1, 5 },
                                    { 2, 6 }, { 3, 7 }, { 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 } };

static uint hex_face_vtcs_0[] = { 0, 3, 7, 4 };
static uint hex_face_vtcs_1[] = { 1, 2, 6, 5 };
static uint hex_face_vtcs_2[] = { 0, 1, 5, 4 };
static uint hex_face_vtcs_3[] = { 3, 2, 6, 7 };
static uint hex_face_vtcs_4[] = { 0, 1, 2, 3 };
static uint hex_face_vtcs_5[] = { 4, 5, 6, 7 };
const uint * RefHex::face_vtcs[] = { hex_face_vtcs_0, hex_face_vtcs_1, hex_face_vtcs_2,
                                     hex_face_vtcs_3, hex_face_vtcs_4, hex_face_vtcs_5 };
const uint RefHex::face_nvtcs[] = { 4, 4, 4, 4, 4, 4 };

static uint hex_face_edges_0[] = { 3, 7, 11, 4 };
static uint hex_face_edges_1[] = { 1, 6, 9, 5 };
static uint hex_face_edges_2[] = { 0, 5, 8, 4 };
static uint hex_face_edges_3[] = { 2, 6, 10, 7 };
static uint hex_face_edges_4[] = { 0, 1, 2, 3 };
static uint hex_face_edges_5[] = { 8, 9, 10, 11 };
const uint * RefHex::face_edges[] = { hex_face_edges_0, hex_face_edges_1, hex_face_edges_2,
                                      hex_face_edges_3, hex_face_edges_4, hex_face_edges_5 };
const uint RefHex::face_nedges[] = { 4, 4, 4, 4, 4, 4 };
const EMode2D RefHex::face_mode[] = { MODE_QUAD, MODE_QUAD, MODE_QUAD,
                                      MODE_QUAD, MODE_QUAD, MODE_QUAD };
const uint RefHex::face_orientations[] = { 8, 8, 8, 8, 8, 8 };
const uint RefHex::edge_tangent[12] = { 0, 1, 0, 1, 2, 2, 2, 2, 0, 1, 0, 1 };
const uint RefHex::face_tangent[6][2] = {
    { 1, 2 }, { 1, 2 }, { 0, 2 }, { 0, 2 }, { 0, 1 }, { 0, 1 }
};

// Prism

const Point3D RefPrism::vertices[] = { { -1.0, -1.0, -1.0 }, { 1.0, -1.0, -1.0 },
                                       { -1.0, 1.0, -1.0 },  { -1.0, -1.0, 1.0 },
                                       { 1.0, -1.0, 1.0 },   { -1.0, 1.0, 1.0 } };

const uint2 RefPrism::edge_vtcs[] = { { 0, 1 }, { 1, 2 }, { 2, 0 }, { 0, 3 }, { 1, 4 },
                                      { 2, 5 }, { 3, 4 }, { 4, 5 }, { 5, 3 } };
static uint prism_face_vtcs_0[] = { 0, 1, 4, 3 };
static uint prism_face_vtcs_1[] = { 1, 2, 5, 4 };
static uint prism_face_vtcs_2[] = { 2, 0, 3, 5 };
static uint prism_face_vtcs_3[] = { 0, 2, 1 };
static uint prism_face_vtcs_4[] = { 3, 4, 5 };
const uint * RefPrism::face_vtcs[] = { prism_face_vtcs_0,
                                       prism_face_vtcs_1,
                                       prism_face_vtcs_2,
                                       prism_face_vtcs_3,
                                       prism_face_vtcs_4 };
const uint RefPrism::face_nvtcs[] = { 4, 4, 4, 3, 3 };
static uint prism_face_edges_0[] = { 0, 4, 6, 3 };
static uint prism_face_edges_1[] = { 1, 5, 7, 4 };
static uint prism_face_edges_2[] = { 2, 3, 8, 5 };
static uint prism_face_edges_3[] = { 0, 2, 1 };
static uint prism_face_edges_4[] = { 6, 7, 8 };
const uint * RefPrism::face_edges[] = { prism_face_edges_0,
                                        prism_face_edges_1,
                                        prism_face_edges_2,
                                        prism_face_edges_3,
                                        prism_face_edges_4 };
const uint RefPrism::face_nedges[] = { 4, 4, 4, 3, 3 };
const EMode2D RefPrism::face_mode[] = { MODE_QUAD,
                                        MODE_QUAD,
                                        MODE_QUAD,
                                        MODE_TRIANGLE,
                                        MODE_TRIANGLE };
const uint RefPrism::face_orientations[] = { 8, 8, 8, 6, 6 };

} // namespace godzilla
