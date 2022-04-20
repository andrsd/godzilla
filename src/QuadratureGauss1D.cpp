#include "QuadratureGauss1D.h"
#include "CallStack.h"

namespace godzilla {

static QPoint1D gauss_pts_0_1_1d[] = { QPoint1D(0.0, 2.0) };

static QPoint1D gauss_pts_2_3_1d[] = { QPoint1D(-0.57735026918963, 1.0),
                                       QPoint1D(0.57735026918963, 1.0) };

static QPoint1D gauss_pts_4_5_1d[] = { QPoint1D(-0.77459666924148, 5.0 / 9.0),
                                       QPoint1D(0.0, 8.0 / 9.0),
                                       QPoint1D(0.77459666924148, 5.0 / 9.0) };

static QPoint1D gauss_pts_6_7_1d[] = { QPoint1D(-0.86113631159405, 0.34785484513745),
                                       QPoint1D(-0.33998104358486, 0.65214515486255),
                                       QPoint1D(0.33998104358486, 0.65214515486255),
                                       QPoint1D(0.86113631159405, 0.34785484513745) };

static QPoint1D gauss_pts_8_9_1d[] = { QPoint1D(-0.90617984593866, 0.23692688505619),
                                       QPoint1D(-0.53846931010568, 0.47862867049937),
                                       QPoint1D(0.00000000000000, 128.0 / 225.0),
                                       QPoint1D(0.53846931010568, 0.47862867049937),
                                       QPoint1D(0.90617984593866, 0.23692688505619) };

static QPoint1D gauss_pts_10_11_1d[] = {
    QPoint1D(-0.93246951420315, 0.17132449237917), QPoint1D(-0.66120938646627, 0.36076157304814),
    QPoint1D(-0.23861918608320, 0.46791393457269), QPoint1D(0.23861918608320, 0.46791393457269),
    QPoint1D(0.66120938646627, 0.36076157304814),  QPoint1D(0.93246951420315, 0.17132449237917)
};

static QPoint1D gauss_pts_12_13_1d[] = {
    QPoint1D(-0.94910791234276, 0.12948496616887), QPoint1D(-0.74153118559939, 0.27970539148928),
    QPoint1D(-0.40584515137740, 0.38183005050512), QPoint1D(0.00000000000000, 0.41795918367347),
    QPoint1D(0.40584515137740, 0.38183005050512),  QPoint1D(0.74153118559939, 0.27970539148928),
    QPoint1D(0.94910791234276, 0.12948496616887)
};

static QPoint1D gauss_pts_14_15_1d[] = {
    QPoint1D(-0.96028985649754, 0.10122853629038), QPoint1D(-0.79666647741363, 0.22238103445337),
    QPoint1D(-0.52553240991633, 0.31370664587789), QPoint1D(-0.18343464249565, 0.36268378337836),
    QPoint1D(0.18343464249565, 0.36268378337836),  QPoint1D(0.52553240991633, 0.31370664587789),
    QPoint1D(0.79666647741363, 0.22238103445337),  QPoint1D(0.96028985649754, 0.10122853629038)
};

static QPoint1D gauss_pts_16_17_1d[] = {
    QPoint1D(-0.96816023950763, 0.08127438836157), QPoint1D(-0.83603110732664, 0.18064816069486),
    QPoint1D(-0.61337143270059, 0.26061069640294), QPoint1D(-0.32425342340381, 0.31234707704000),
    QPoint1D(0.00000000000000, 0.33023935500126),  QPoint1D(0.32425342340381, 0.31234707704000),
    QPoint1D(0.61337143270059, 0.26061069640294),  QPoint1D(0.83603110732664, 0.18064816069486),
    QPoint1D(0.96816023950763, 0.08127438836157)
};

static QPoint1D gauss_pts_18_19_1d[] = {
    QPoint1D(-0.97390652851717, 0.06667134430869), QPoint1D(-0.86506336668898, 0.14945134915058),
    QPoint1D(-0.67940956829902, 0.21908636251598), QPoint1D(-0.43339539412925, 0.26926671931000),
    QPoint1D(-0.14887433898163, 0.29552422471475), QPoint1D(0.14887433898163, 0.29552422471475),
    QPoint1D(0.43339539412925, 0.26926671931000),  QPoint1D(0.67940956829902, 0.21908636251598),
    QPoint1D(0.86506336668898, 0.14945134915058),  QPoint1D(0.97390652851717, 0.06667134430869)
};

static QPoint1D gauss_pts_20_21_1d[] = {
    QPoint1D(-0.97822865814606, 0.05566856711617), QPoint1D(-0.88706259976810, 0.12558036946490),
    QPoint1D(-0.73015200557405, 0.18629021092773), QPoint1D(-0.51909612920681, 0.23319376459199),
    QPoint1D(-0.26954315595234, 0.26280454451025), QPoint1D(0.00000000000000, 0.27292508677790),
    QPoint1D(0.26954315595234, 0.26280454451025),  QPoint1D(0.51909612920681, 0.23319376459199),
    QPoint1D(0.73015200557405, 0.18629021092773),  QPoint1D(0.88706259976810, 0.12558036946490),
    QPoint1D(0.97822865814606, 0.05566856711617)
};

static QPoint1D gauss_pts_22_23_1d[] = {
    QPoint1D(-0.98156063424672, 0.04717533638651), QPoint1D(-0.90411725637047, 0.10693932599532),
    QPoint1D(-0.76990267419430, 0.16007832854335), QPoint1D(-0.58731795428662, 0.20316742672307),
    QPoint1D(-0.36783149899818, 0.23349253653835), QPoint1D(-0.12523340851147, 0.24914704581340),
    QPoint1D(0.12523340851147, 0.24914704581340),  QPoint1D(0.36783149899818, 0.23349253653835),
    QPoint1D(0.58731795428662, 0.20316742672307),  QPoint1D(0.76990267419430, 0.16007832854335),
    QPoint1D(0.90411725637047, 0.10693932599532),  QPoint1D(0.98156063424672, 0.04717533638651)
};

static QPoint1D gauss_pts_24_1d[] = {
    QPoint1D(-0.98418305471859, 0.04048400476532), QPoint1D(-0.91759839922298, 0.09212149983773),
    QPoint1D(-0.80157809073331, 0.13887351021979), QPoint1D(-0.64234933944034, 0.17814598076195),
    QPoint1D(-0.44849275103645, 0.20781604753689), QPoint1D(-0.23045831595513, 0.22628318026290),
    QPoint1D(0.00000000000000, 0.23255155323087),  QPoint1D(0.23045831595513, 0.22628318026290),
    QPoint1D(0.44849275103645, 0.20781604753689),  QPoint1D(0.64234933944034, 0.17814598076195),
    QPoint1D(0.80157809073331, 0.13887351021979),  QPoint1D(0.91759839922298, 0.09212149983773),
    QPoint1D(0.98418305471859, 0.04048400476532)
};

static QPoint1D * gauss_tables_1d[] = { gauss_pts_0_1_1d,   gauss_pts_0_1_1d,   gauss_pts_2_3_1d,
                                        gauss_pts_2_3_1d,   gauss_pts_4_5_1d,   gauss_pts_4_5_1d,
                                        gauss_pts_6_7_1d,   gauss_pts_6_7_1d,   gauss_pts_8_9_1d,
                                        gauss_pts_8_9_1d,   gauss_pts_10_11_1d, gauss_pts_10_11_1d,
                                        gauss_pts_12_13_1d, gauss_pts_12_13_1d, gauss_pts_14_15_1d,
                                        gauss_pts_14_15_1d, gauss_pts_16_17_1d, gauss_pts_16_17_1d,
                                        gauss_pts_18_19_1d, gauss_pts_18_19_1d, gauss_pts_20_21_1d,
                                        gauss_pts_20_21_1d, gauss_pts_22_23_1d, gauss_pts_22_23_1d,
                                        gauss_pts_24_1d };

static uint gauss_np_1d[] = {
    sizeof(gauss_pts_0_1_1d) / sizeof(QPoint1D),   sizeof(gauss_pts_0_1_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_2_3_1d) / sizeof(QPoint1D),   sizeof(gauss_pts_2_3_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_4_5_1d) / sizeof(QPoint1D),   sizeof(gauss_pts_4_5_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_6_7_1d) / sizeof(QPoint1D),   sizeof(gauss_pts_6_7_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_8_9_1d) / sizeof(QPoint1D),   sizeof(gauss_pts_8_9_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_10_11_1d) / sizeof(QPoint1D), sizeof(gauss_pts_10_11_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_12_13_1d) / sizeof(QPoint1D), sizeof(gauss_pts_12_13_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_14_15_1d) / sizeof(QPoint1D), sizeof(gauss_pts_14_15_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_16_17_1d) / sizeof(QPoint1D), sizeof(gauss_pts_16_17_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_18_19_1d) / sizeof(QPoint1D), sizeof(gauss_pts_18_19_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_20_21_1d) / sizeof(QPoint1D), sizeof(gauss_pts_20_21_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_22_23_1d) / sizeof(QPoint1D), sizeof(gauss_pts_22_23_1d) / sizeof(QPoint1D),
    sizeof(gauss_pts_24_1d) / sizeof(QPoint1D)
};

static QuadratureGauss1D quad_std_1d;

QuadratureGauss1D::QuadratureGauss1D() : Quadrature1D()
{
    this->max_order = MAX_ORDER;
    this->np = gauss_np_1d;
    this->tables = gauss_tables_1d;
}

QuadratureGauss1D &
QuadratureGauss1D::get()
{
    return quad_std_1d;
}

} // namespace godzilla
