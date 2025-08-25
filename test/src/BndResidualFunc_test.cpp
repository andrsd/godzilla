#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ImplicitFENonlinearProblem.h"
#include "godzilla/NaturalBC.h"
#include "godzilla/BndResidualFunc.h"

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

protected:
    void
    set_up_fields() override
    {
        set_field(0, "u", 1, Order(1));
    }

    void
    set_up_weak_form() override
    {
    }
};

class TestF : public BndResidualFunc {
public:
    explicit TestF(const TestBC * bc) :
        BndResidualFunc(bc),
        dim(get_spatial_dimension()),
        u(get_field_value("u")),
        u_x(get_field_gradient("u")),
        u_t(get_field_dot("u")),
        t(get_time()),
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
    const FieldValue & u_t;
    const Real & t;
    const Normal & normal;
    const Point & xyz;
};

} // namespace

TEST(BndResidualFuncTest, test)
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

    TestF res(&bc);
}
