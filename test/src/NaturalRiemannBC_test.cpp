#include "gmock/gmock.h"
#include "LineMesh.h"
#include "NaturalRiemannBC.h"
#include "ExplicitFVLinearProblem.h"
#include "TestApp.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestBC : public NaturalRiemannBC {
public:
    explicit TestBC(const Parameters & params) : NaturalRiemannBC(params), comps({ 0 }) {}

    virtual const std::vector<Int> &
    get_components() const override
    {
        return this->comps;
    }

    virtual void
    evaluate(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) override
    {
        xG[0] = xI[0];
    }

protected:
    std::vector<Int> comps;

public:
    static Parameters
    parameters()
    {
        Parameters params = NaturalRiemannBC::parameters();
        return params;
    }
};

class TestExplicitFVLinearProblem : public ExplicitFVLinearProblem {
public:
    explicit TestExplicitFVLinearProblem(const Parameters & params) :
        ExplicitFVLinearProblem(params)
    {
    }

protected:
    virtual void
    set_up_fields() override
    {
        add_field(0, "u", 1);
    }
};

} // namespace

TEST(NaturalRiemannBCTest, api)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFVLinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 1e-3;
    prob_pars.set<Real>("dt") = 1e-3;
    TestExplicitFVLinearProblem prob(prob_pars);
    app.problem = &prob;

    Parameters bc_pars = TestBC::parameters();
    bc_pars.set<const App *>("_app") = &app;
    bc_pars.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_pars.set<std::string>("boundary") = "left";
    TestBC bc(bc_pars);
    prob.add_boundary_condition(&bc);

    mesh.create();
    prob.create();

    EXPECT_THAT(bc.get_components(), ElementsAre(0));
    EXPECT_EQ(bc.get_field_id(), 0);
    EXPECT_THAT(bc.get_ids(), ElementsAre(1));

    Label left = mesh.get_label("left");
    EXPECT_EQ(bc.get_label(), left);
}
