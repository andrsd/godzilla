#include "gmock/gmock.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "ImplicitFENonlinearProblem.h"
#include "JacobianFunc.h"

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
    const Int & dim;
    const FieldValue & u;
    const FieldGradient & u_x;
    const Real & t;
    const Real & t_shift;
};

} // namespace

TEST(JacobianFuncTest, test)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
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
    Real time;
    EXPECT_CALL(prob, get_assembly_time()).Times(1).WillOnce(ReturnRef(time));
    Real time_shift;
    EXPECT_CALL(prob, get_time_shift()).Times(1).WillOnce(ReturnRef(time_shift));

    TestJ jac(&prob);
}
