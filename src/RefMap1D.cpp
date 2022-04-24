#include "RefMap1D.h"
#include "CallStack.h"
#include "Determinant.h"
#include "RefMapShapeset1D.h"

namespace godzilla {

static const RefMapShapesetEdge * ss_edge = RefMapShapesetEdge::get();
static ShapeFunction1D pss_edge(ss_edge);
static ShapeFunction1D * ref_map_pss[] = { &pss_edge };

RefMap1D::RefMap1D(const Mesh * mesh) :
    mesh(mesh)
{
    _F_;
    this->element = nullptr;
    this->pss = nullptr;
}

RefMap1D::~RefMap1D()
{
    _F_;
}

void
RefMap1D::set_active_element(const Element1D * e)
{
    _F_;
    assert(e != nullptr);

    this->pss = ref_map_pss[0];
    this->pss->set_active_element(e);

    if (e == this->element)
        return;
    this->element = e;

    this->is_const_jacobian = true;

    uint nvertices = this->element->get_num_vertices();

    // prepare the shapes and coefficients of the reference map
    const Shapeset1D * shapeset = this->pss->get_shapeset();
    uint k = 0;
    for (uint i = 0; i < nvertices; i++)
        this->indices[k++] = shapeset->get_vertex_index(i);

    // straight element
    for (uint i = 0; i < nvertices; i++)
        /// FIMXE: remove the static_cast
        this->vertex[i] = static_cast<const Vertex1D *>(this->mesh->get_vertex(e->get_vertex(i)));
    this->coefs = vertex;
    this->n_coefs = nvertices;

    // calculate the order of the reference map
    ref_order = 0;
    inv_ref_order = 0;

    calc_const_inv_ref_map();
}

Real1x1 *
RefMap1D::get_ref_map(const uint np, const QPoint1D * pt)
{
    _F_;

    Real1x1 * m = new Real1x1[np];
    MEM_CHECK(m);
    memset(m, 0, np * sizeof(Real1x1));

    for (uint i = 0; i < np; i++)
        memcpy(m + i, this->const_ref_map, sizeof(Real1x1));

    return m;
}

Real *
RefMap1D::get_jacobian(const uint np, const QPoint1D * pt, bool trans)
{
    _F_;

    Real * jac = new Real[np];
    MEM_CHECK(jac);
    if (trans)
        for (uint i = 0; i < np; i++)
            jac[i] = this->const_jacobian * pt[i].w;
    else
        for (uint i = 0; i < np; i++)
            jac[i] = this->const_jacobian;

    return jac;
}

Real1x1 *
RefMap1D::get_inv_ref_map(const uint np, const QPoint1D * pt)
{
    _F_;

    Real1x1 * irm = new Real1x1[np];
    MEM_CHECK(irm);

    for (uint i = 0; i < np; i++)
        memcpy(irm + i, this->const_inv_ref_map, sizeof(Real1x1));

    return irm;
}

Real *
RefMap1D::get_phys_x(const uint np, const QPoint1D * pt)
{
    _F_;
    // transform all x coordinates of the integration points
    Real * x = new Real[np];
    MEM_CHECK(x);
    memset(x, 0, np * sizeof(Real));
    for (uint i = 0; i < this->n_coefs; i++) {
        this->pss->set_active_shape(this->indices[i]);
        this->pss->precalculate(np, pt, RealFunction1D::FN_DEFAULT);
        blas_axpy(np, this->coefs[i]->x, this->pss->get_fn_values(), 1, x, 1);
    }

    return x;
}

void
RefMap1D::calc_const_inv_ref_map()
{
    _F_;
    Real1x1 m = { { (this->vertex[1]->x - this->vertex[0]->x) / 2 } };
    memcpy(&this->const_ref_map, &m, sizeof(Real1x1));

    this->const_jacobian = m[0][0];

    Real ij = 1.0 / this->const_jacobian;

    this->const_inv_ref_map[0][0] = ij;
}

} // namespace godzilla
