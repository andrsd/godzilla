#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ImplicitFENonlinearProblem.h"
#include "godzilla/NaturalBC.h"
#include "godzilla/BndJacobianFunc.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestBC : public NaturalBC {
public:
    explicit TestBC(const Parameters & params) : NaturalBC(params), components({ 0 }) {}

    const std::vector<Int> &
    get_components() const override
    {
        return this->components;
    }

protected:
    void
    set_up_weak_form() override
    {
    }

    std::vector<Int> components;
};

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

class TestJ : public BndJacobianFunc {
public:
    explicit TestJ(const TestBC * bc) :
        BndJacobianFunc(bc),
        dim(get_spatial_dimension()),
        u(get_field_value("u")),
        u_x(get_field_gradient("u")),
        t(get_time()),
        t_shift(get_time_shift()),
        normal(get_normal()),
        xyz(get_xyz())
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
    const Real & t;
    const Real & t_shift;
    const Normal & normal;
    const Point & xyz;
};

} // namespace

TEST(BndJacobianFuncTest, test)
{
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
    app.set_problem(&prob);

    Parameters bc_pars = NaturalBC::parameters();
    bc_pars.set<App *>("_app") = &app;
    bc_pars.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_pars.set<std::string>("field") = "u";
    bc_pars.set<std::vector<std::string>>("boundary") = { "marker" };
    TestBC bc(bc_pars);
    prob.add_boundary_condition(&bc);

    mesh.create();
    prob.create();
    bc.create();

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
    Normal n(1);
    EXPECT_CALL(prob, get_normal()).Times(1).WillOnce(ReturnRef(n));
    Point coord(1);
    EXPECT_CALL(prob, get_xyz()).Times(1).WillOnce(ReturnRef(coord));

    TestJ jac(&bc);
}
