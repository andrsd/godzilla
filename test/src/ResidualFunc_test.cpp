#include "gmock/gmock.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "ImplicitFENonlinearProblem.h"
#include "ResidualFunc.h"

using namespace godzilla;
using namespace testing;

namespace {

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

} // namespace

TEST(ResidualFuncTest, test)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
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
