#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ImplicitFENonlinearProblem.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/Types.h"

using namespace godzilla;
using namespace testing;

TEST(ResidualFuncTest, test)
{
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

    class TestF : public ResidualFunc {
    public:
        explicit TestF(GTestProblem * prob) :
            ResidualFunc(prob),
            dim(get_spatial_dimension()),
            u(get_field_value("u")),
            u_x(get_field_gradient("u")),
            u_t(get_field_dot("u")),
            t(get_time())
        {
        }

        void
        evaluate(Scalar f[]) const override
        {
            f[0] = 0.;
        }

    protected:
        const Int & dim;
        const FieldValue & u;
        const FieldGradient & u_x;
        const FieldValue & u_t;
        const Real & t;
    };

    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<MeshObject *>("_mesh_obj", &mesh)
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 20)
        .set<Real>("dt", 5);
    GTestProblem prob(prob_pars);

    mesh.create();
    prob.create();

    TestF res(&prob);
}

TEST(ResidualFuncTest, test_vals)
{
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

    class TestF : public ResidualFunc {
    public:
        explicit TestF(GTestProblem * prob) :
            ResidualFunc(prob),
            dim(get_spatial_dimension()),
            u(get_field_value("u")),
            u_x(get_field_gradient("u")),
            u_t(get_field_dot("u")),
            t(get_time())
        {
        }

        void
        evaluate(Scalar f[]) const override
        {
            f[0] = dim;
            f[1] = u(0);
            f[2] = u_x(0);
            f[3] = u_t(0);
            f[4] = t;
        }

    protected:
        const Int & dim;
        const FieldValue & u;
        const FieldGradient & u_x;
        const FieldValue & u_t;
        const Real & t;
    };

    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<MeshObject *>("_mesh_obj", &mesh)
        .set<Real>("start_time", 1.23)
        .set<Real>("end_time", 20)
        .set<Real>("dt", 5);
    GTestProblem prob(prob_pars);

    mesh.create();
    prob.create();

    TestF res(&prob);
    Scalar vals[5];
    res.evaluate(vals);
    EXPECT_DOUBLE_EQ(vals[0], 1);
    EXPECT_DOUBLE_EQ(vals[4], 0.);
}
