#include "gmock/gmock.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "Parameters.h"
#include "ValueFunctional.h"
#include "GTestFENonlinearProblem.h"

using namespace godzilla;
using namespace testing;

namespace {

class SimpleFnl : public ValueFunctional {
public:
    explicit SimpleFnl(const Parameters & pars) :
        ValueFunctional(pars.get<FEProblemInterface *>("_fepi"), "region"),
        a(declare_value<double>("a"))
    {
    }

    void
    evaluate() const override
    {
        a = 2.;
    }

protected:
    double & a;
};

class NeedAFnl : public ValueFunctional {
public:
    explicit NeedAFnl(const Parameters & pars) :
        ValueFunctional(pars.get<FEProblemInterface *>("_fepi"), "region"),
        b(declare_value<double>("b")),
        a(get_value<double>("a"))
    {
    }

    void
    evaluate() const override
    {
        b = a + 4.;
    }

protected:
    double & b;
    const double & a;
};

} // namespace

TEST(ValueFunctional, eval)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    mesh.create();
    prob.create();

    Parameters params;
    params.set<FEProblemInterface *>("_fepi") = &prob;
    prob.create_functional<SimpleFnl>("a", params);
    prob.create_functional<NeedAFnl>("b", params);

    const auto & fnls = prob.get_functionals();
    EXPECT_EQ(fnls.size(), 2);
    EXPECT_THAT(fnls, Contains(Key("a")));
    EXPECT_THAT(fnls, Contains(Key("b")));

    const auto & a = prob.get_functional("a");
    const auto & b = prob.get_functional("b");

    EXPECT_STREQ(a.get_region().c_str(), "region");
    EXPECT_THAT(a.get_provided_values(), ElementsAre("a@region"));

    EXPECT_STREQ(b.get_region().c_str(), "region");
    EXPECT_THAT(b.get_provided_values(), ElementsAre("b@region"));
    EXPECT_THAT(b.get_dependent_values(), ElementsAre("a@region"));
}
