#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ImplicitFENonlinearProblem.h"
#include "godzilla/NaturalBC.h"
#include "godzilla/BndJacobianFunc.h"
#include "godzilla/Types.h"

using namespace godzilla;

namespace {

class TestBC : public NaturalBC {
public:
    explicit TestBC(const Parameters & pars) : NaturalBC(pars) {}

protected:
    void
    set_up_weak_form() override
    {
    }

    std::vector<Int>
    create_components() override
    {
        return { 0 };
    }
};

class GTestProblem : public ImplicitFENonlinearProblem {
public:
    explicit GTestProblem(const Parameters & pars) : ImplicitFENonlinearProblem(pars) {}

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

class TestJ : public BndJacobianFunc {
public:
    explicit TestJ(Ref<TestBC> bc) :
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
    const Dimension & dim;
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

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = app.make_parameters<GTestProblem>();
    prob_pars.set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 20)
        .set<Real>("dt", 5);
    auto prob = app.make_problem<GTestProblem>(prob_pars);

    auto bc_pars = app.make_parameters<TestBC>();
    bc_pars.set<String>("field", "u");
    bc_pars.set<std::vector<String>>("boundary", { "left" });
    auto bc = prob->add_boundary_condition<TestBC>(bc_pars);

    prob->create();
    bc->create();

    TestJ jac(bc);
}
