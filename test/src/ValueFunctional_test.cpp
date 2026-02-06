#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/Parameters.h"
#include "godzilla/ValueFunctional.h"
#include "GTestFENonlinearProblem.h"

using namespace godzilla;

namespace {

class SimpleFnl : public ValueFunctional {
public:
    explicit SimpleFnl(const Parameters & pars) :
        ValueFunctional(pars.get<Ref<FEProblemInterface>>("_fepi"), "region"),
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
        ValueFunctional(pars.get<Ref<FEProblemInterface>>("_fepi"), "region"),
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

TEST(ValueFunctional, DISABLED_eval)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    prob.create();

    Parameters params;
    params.set<Ref<FEProblemInterface>>("_fepi", ref(prob));
    prob.create_functional<SimpleFnl>("a", params);
    prob.create_functional<NeedAFnl>("b", params);

    const auto & fnls = prob.get_functionals();
    EXPECT_EQ(fnls.size(), 2);
    EXPECT_THAT(fnls, testing::Contains(testing::Key("a")));
    EXPECT_THAT(fnls, testing::Contains(testing::Key("b")));

    const auto & a = prob.get_functional("a");
    const auto & b = prob.get_functional("b");

    EXPECT_STREQ(a.get_region().c_str(), "region");
    EXPECT_THAT(a.get_provided_values(), testing::ElementsAre("a@region"));

    EXPECT_STREQ(b.get_region().c_str(), "region");
    EXPECT_THAT(b.get_provided_values(), testing::ElementsAre("b@region"));
    EXPECT_THAT(b.get_dependent_values(), testing::ElementsAre("a@region"));
}
