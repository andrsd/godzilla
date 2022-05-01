#include "QuadratureOutput1D.h"
#include "CallStack.h"

namespace godzilla {

static QPoint1D out_pts_1d[] = { QPoint1D(-1.0, 0.), QPoint1D(1.0, 0.) };

static QPoint1D * out_tables_1d[] = { out_pts_1d, out_pts_1d, out_pts_1d, out_pts_1d };

static uint out_np_1d[] = {
    sizeof(out_pts_1d) / sizeof(QPoint1D),
    sizeof(out_pts_1d) / sizeof(QPoint1D),
    sizeof(out_pts_1d) / sizeof(QPoint1D),
    sizeof(out_pts_1d) / sizeof(QPoint1D),
};

static QuadratureOutput1D quad_out_1d;

QuadratureOutput1D::QuadratureOutput1D() : Quadrature1D()
{
    _F_;
    this->max_order = 3;
    this->np = out_np_1d;
    this->tables = out_tables_1d;
}

QuadratureOutput1D &
QuadratureOutput1D::get()
{
    return quad_out_1d;
}

} // namespace godzilla
