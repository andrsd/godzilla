#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/Parameters.h"
#include "godzilla/ValueFunctional.h"
#include "GTestFENonlinearProblem.h"
#include "ExceptionTestMacros.h"

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

class NeedBFnl : public ValueFunctional {
public:
    explicit NeedBFnl(const Parameters & pars) :
        ValueFunctional(pars.get<Ref<FEProblemInterface>>("_fepi"), "region"),
        b(get_value<double>("b")),
        c(declare_value<double>("c"))
    {
    }

    void
    evaluate() const override
    {
        c = b * 3.;
    }

protected:
    const double & b;
    double & c;
};

} // namespace

TEST(DependencyEvaluator, DISABLED_create_functional)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    prob.create();

    Parameters params;
    params.set<Ref<FEProblemInterface>>("_fepi", ref(prob));
    prob.create_functional<SimpleFnl>("test", params);

    const auto & fnl = prob.get_functional("test");
    EXPECT_STREQ(fnl.get_region().c_str(), "region");
}

TEST(DependencyEvaluator, DISABLED_create_existing_functional)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    prob.create();

    Parameters params;
    params.set<Ref<FEProblemInterface>>("_fepi", ref(prob));
    prob.create_functional<SimpleFnl>("test", params);

    EXPECT_DEATH(prob.create_functional<SimpleFnl>("test", params),
                 "Functional with name 'test' already exists.");
}

TEST(DependencyEvaluator, DISABLED_get_non_existent_functional)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    prob.create();

    EXPECT_DEATH(prob.get_functional("asdf"), "No functional with name 'asdf' found. Typo?");
}

TEST(DependencyEvaluator, DISABLED_eval)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    prob.create();

    Parameters params;
    params.set<Ref<FEProblemInterface>>("_fepi", ref(prob));
    // create NeedAFnl first, so we make sure values can be obtained before they
    // are declared
    prob.create_functional<NeedAFnl>("b", params);
    prob.create_functional<SimpleFnl>("a", params);
    prob.create_functional<NeedBFnl>("c", params);

    const auto & fnls = prob.get_functionals();
    EXPECT_EQ(fnls.size(), 3);
    EXPECT_THAT(fnls, testing::Contains(testing::Key("a")));
    EXPECT_THAT(fnls, testing::Contains(testing::Key("b")));
    EXPECT_THAT(fnls, testing::Contains(testing::Key("c")));

    const auto & a = fnls.find("a")->second;
    const auto & b = fnls.find("b")->second;
    const auto & c = fnls.find("c")->second;

    a->evaluate();
    b->evaluate();
    c->evaluate();

    EXPECT_DOUBLE_EQ(prob.get_value<double>("a@region"), 2.);
    EXPECT_DOUBLE_EQ(prob.get_value<double>("b@region"), 6.);
    EXPECT_DOUBLE_EQ(prob.get_value<double>("c@region"), 18.);
}

TEST(DependencyEvaluator, DISABLED_redeclare_a_value)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    prob.create();

    Parameters params;
    params.set<Ref<FEProblemInterface>>("_fepi", ref(prob));
    prob.create_functional<SimpleFnl>("b", params);
    EXPECT_DEATH(prob.create_functional<SimpleFnl>("a", params),
                 "Trying to declare an already existing value 'a@region'.");
}

TEST(DependencyEvaluator, DISABLED_get_suppliers)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    prob.create();

    Parameters params;
    params.set<Ref<FEProblemInterface>>("_fepi", ref(prob));
    prob.create_functional<NeedAFnl>("b", params);
    prob.create_functional<SimpleFnl>("a", params);
    prob.create_functional<NeedBFnl>("c", params);

    const auto & fnls = prob.get_functionals();
    auto a = fnls.find("a")->second;
    auto b = fnls.find("b")->second;
    auto c = fnls.find("c")->second;

    auto suppliers = prob.get_suppliers();
    EXPECT_THAT(suppliers,
                testing::UnorderedElementsAre(testing::Pair("a@region", a),
                                              testing::Pair("b@region", b),
                                              testing::Pair("c@region", c)));
}

TEST(DependencyEvaluator, DISABLED_build_dep_graph)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    prob.create();

    Parameters params;
    params.set<FEProblemInterface *>("_fepi", &prob);
    prob.create_functional<NeedAFnl>("b", params);
    prob.create_functional<SimpleFnl>("a", params);
    prob.create_functional<NeedBFnl>("c", params);

    const auto & fnls = prob.get_functionals();
    auto a = fnls.find("a")->second;
    auto b = fnls.find("b")->second;
    auto c = fnls.find("c")->second;

    auto suppliers = prob.get_suppliers();
    auto graph = prob.build_dependecy_graph(suppliers);

    auto v = graph.bfs({ c });
    EXPECT_THAT(v, testing::ElementsAre(c, b, a));
}
