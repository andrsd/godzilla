#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ImplicitFENonlinearProblem.h"
#include "godzilla/ResidualFunc.h"

using namespace godzilla;
using namespace testing;

TEST(ResidualFuncTest, test)
{
    class GTestProblem : public ImplicitFENonlinearProblem {
    public:
        explicit GTestProblem(const Parameters & params) : ImplicitFENonlinearProblem(params) {}

        MOCK_METHOD(const Int &, get_spatial_dimension, (), (const));
        MOCK_METHOD(const FieldValue &, get_field_value, (const std::string & field_name), (const));
        MOCK_METHOD(const FieldGradient &,
                    get_field_gradient,
                    (const std::string & field_name),
                    (const));
        MOCK_METHOD(const FieldValue &, get_field_dot, (const std::string & field_name), (const));
        MOCK_METHOD(const Real &, get_time_shift, (), (const));
        MOCK_METHOD(const Real &, get_assembly_time, (), (const));
        MOCK_METHOD(const Normal &, get_normal, (), (const));
        MOCK_METHOD(const Point &, get_xyz, (), (const));

    protected:
        void
        set_up_fields() override
        {
            set_fe(0, "u", 1, 1);
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

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 20;
    prob_pars.set<Real>("dt") = 5;
    GTestProblem prob(prob_pars);

    mesh.create();
    prob.create();

    Int dim;
    EXPECT_CALL(prob, get_spatial_dimension()).Times(1).WillOnce(ReturnRef(dim));
    FieldValue val(1);
    EXPECT_CALL(prob, get_field_value(_)).Times(1).WillOnce(ReturnRef(val));
    FieldGradient grad(1, 1);
    EXPECT_CALL(prob, get_field_gradient(_)).Times(1).WillOnce(ReturnRef(grad));
    FieldValue dot(1);
    EXPECT_CALL(prob, get_field_dot(_)).Times(1).WillOnce(ReturnRef(dot));
    Real time;
    EXPECT_CALL(prob, get_assembly_time()).Times(1).WillOnce(ReturnRef(time));

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
            set_fe(0, "u", 1, 1);
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

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    prob_pars.set<Real>("start_time") = 1.23;
    prob_pars.set<Real>("end_time") = 20;
    prob_pars.set<Real>("dt") = 5;
    GTestProblem prob(prob_pars);

    mesh.create();
    prob.create();

    TestF res(&prob);
    Scalar vals[5];
    res.evaluate(vals);
    EXPECT_DOUBLE_EQ(vals[0], 1);
    EXPECT_DOUBLE_EQ(vals[4], 0.);
}
