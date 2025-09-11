#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ImplicitFENonlinearProblem.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/Types.h"

using namespace godzilla;
using namespace testing;

namespace {

class GTestProblem : public ImplicitFENonlinearProblem {
public:
    explicit GTestProblem(const Parameters & params) : ImplicitFENonlinearProblem(params) {}

protected:
    void
    set_up_fields() override
    {
        set_field(FieldID(0), "u", 1, Order(1));
    }

    void
    set_up_weak_form() override
    {
    }
};

class TestJ : public JacobianFunc {
public:
    explicit TestJ(GTestProblem * prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension()),
        u(get_field_value("u")),
        u_x(get_field_gradient("u")),
        t(get_time()),
        t_shift(get_time_shift())
    {
    }

    void
    evaluate(Scalar g[]) const override
    {
        g[0] = 1.;
    }

protected:
    const Dimension & dim;
    const FieldValue & u;
    const FieldGradient & u_x;
    const Real & t;
    const Real & t_shift;
};

} // namespace

TEST(JacobianFuncTest, test)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto prob_pars = GTestProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    prob_pars.set<Real>("start_time", 0.);
    prob_pars.set<Real>("end_time", 20);
    prob_pars.set<Real>("dt", 5);
    GTestProblem prob(prob_pars);

    prob.create();

    TestJ jac(&prob);
}
