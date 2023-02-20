#include "gmock/gmock.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "DependencyEvaluator.h"
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

class NeedBFnl : public ValueFunctional {
public:
    explicit NeedBFnl(const Parameters & pars) :
        ValueFunctional(pars.get<FEProblemInterface *>("_fepi"), "region"),
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
    double & c;
    const double & b;
};

} // namespace

TEST(DependencyEvaluator, create_functional)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    mesh.create();
    prob.create();

    Parameters params;
    params.set<FEProblemInterface *>("_fepi") = &prob;
    prob.create_functional<SimpleFnl>("test", params);

    const auto & fnl = prob.get_functional("test");
    EXPECT_STREQ(fnl.get_region().c_str(), "region");
}

TEST(DependencyEvaluator, create_existing_functional)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    mesh.create();
    prob.create();

    Parameters params;
    params.set<FEProblemInterface *>("_fepi") = &prob;
    prob.create_functional<SimpleFnl>("test", params);

    EXPECT_DEATH(prob.create_functional<SimpleFnl>("test", params),
                 "\\[ERROR\\] Functional with name 'test' already exists.");
}

TEST(DependencyEvaluator, get_non_existent_functional)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    mesh.create();
    prob.create();

    EXPECT_DEATH(prob.get_functional("asdf"),
                 "\\[ERROR\\] No functional with name 'asdf' found. Typo?.");
}

TEST(DependencyEvaluator, eval)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    mesh.create();
    prob.create();

    Parameters params;
    params.set<FEProblemInterface *>("_fepi") = &prob;
    // create NeedAFnl first, so we make sure values can be obtained before they
    // are declared
    prob.create_functional<NeedAFnl>("b", params);
    prob.create_functional<SimpleFnl>("a", params);
    prob.create_functional<NeedBFnl>("c", params);

    const auto & fnls = prob.get_functionals();
    EXPECT_EQ(fnls.size(), 3);
    EXPECT_THAT(fnls, Contains(Key("a")));
    EXPECT_THAT(fnls, Contains(Key("b")));
    EXPECT_THAT(fnls, Contains(Key("c")));

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

TEST(DependencyEvaluator, redeclare_a_value)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    mesh.create();
    prob.create();

    Parameters params;
    params.set<FEProblemInterface *>("_fepi") = &prob;
    prob.create_functional<SimpleFnl>("b", params);
    EXPECT_DEATH(prob.create_functional<SimpleFnl>("a", params),
                 "\\[ERROR\\] Trying to declare an already existing value 'a@region'.");
}
