#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "CallStack.h"
#include "LineMesh.h"
#include "NaturalRiemannBC.h"
#include "ExplicitFVLinearProblem.h"
#include "Parameters.h"
#include "TestApp.h"

using namespace godzilla;

class TestBC : public NaturalRiemannBC {
public:
    explicit TestBC(const InputParameters & params) :
        NaturalRiemannBC(params),
        inlet(get_param<bool>("inlet"))
    {
    }

    virtual PetscInt
    get_num_components() const override
    {
        return 1;
    }

    virtual std::vector<PetscInt>
    get_components() const override
    {
        return { 0 };
    }

    virtual void
    evaluate(PetscReal time,
             const PetscReal * c,
             const PetscReal * n,
             const PetscScalar * xI,
             PetscScalar * xG) override
    {
        if (this->inlet)
            xG[0] = 1.;
        else
            xG[0] = xI[0];
    }

protected:
    bool inlet;

public:
    static InputParameters
    valid_params()
    {
        InputParameters params = NaturalRiemannBC::valid_params();
        params.add_required_param<bool>("inlet", "inlet?");
        return params;
    }
};

//

class TestExplicitFVLinearProblem : public ExplicitFVLinearProblem {
public:
    explicit TestExplicitFVLinearProblem(const InputParameters & params) :
        ExplicitFVLinearProblem(params)
    {
    }

    virtual void
    set_up_time_scheme() override
    {
        ExplicitFVLinearProblem::set_up_time_scheme();
    }

protected:
    virtual void
    set_up_fields() override
    {
        add_field(0, "u", 1);
    }

    virtual void
    compute_flux(PetscInt dim,
                 PetscInt nf,
                 const PetscReal x[],
                 const PetscReal n[],
                 const PetscScalar uL[],
                 const PetscScalar uR[],
                 PetscInt n_consts,
                 const PetscScalar constants[],
                 PetscScalar flux[]) override
    {
        _F_;
        PetscReal wind[] = { 0.5 };
        PetscReal wn = 0;
        wn += wind[0] * n[0];
        flux[0] = (wn > 0 ? uL[0] : uR[0]) * wn;
    }
};

REGISTER_OBJECT(TestExplicitFVLinearProblem);

TEST(ExplicitFVLinearProblemTest, api)
{
    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    InputParameters prob_pars = TestExplicitFVLinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 1e-3;
    prob_pars.set<PetscReal>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFVLinearProblem prob(prob_pars);
    app.problem = &prob;

    mesh.create();
    prob.create();

    EXPECT_EQ(prob.get_num_fields(), 1);
    EXPECT_THAT(prob.get_field_names(), testing::ElementsAre(""));

    EXPECT_EQ(prob.get_field_name(0), "u");
    EXPECT_DEATH(prob.get_field_name(65536), "\\[ERROR\\] Field with ID = '65536' does not exist.");

    EXPECT_EQ(prob.get_field_num_components(0), 1);
    EXPECT_DEATH(prob.get_field_num_components(65536),
                 "\\[ERROR\\] Field with ID = '65536' does not exist\\.");

    EXPECT_EQ(prob.get_field_id("u"), 0);
    EXPECT_EQ(prob.get_field_id("nonexistent"), 0);

    EXPECT_TRUE(prob.has_field_by_id(0));
    EXPECT_FALSE(prob.has_field_by_id(65536));

    EXPECT_FALSE(prob.has_field_by_name("u"));
    EXPECT_FALSE(prob.has_field_by_name("nonexistent"));

    EXPECT_EQ(prob.get_field_order(0), 0);
    EXPECT_DEATH(prob.get_field_order(65536),
                 "\\[ERROR\\] Multiple-field problems are not implemented");

    EXPECT_EQ(prob.get_field_component_name(0, 0).compare("u"), 0);
    EXPECT_DEATH(prob.get_field_component_name(65536, 0),
                 "\\[ERROR\\] Multiple-field problems are not implemented");

    EXPECT_DEATH(prob.set_field_component_name(0, 0, "x"),
                 "\\[ERROR\\] Unable to set component name for single-component field");
}

TEST(ExplicitFVLinearProblemTest, fields)
{
    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    InputParameters prob_pars = TestExplicitFVLinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 1e-3;
    prob_pars.set<PetscReal>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFVLinearProblem prob(prob_pars);
    app.problem = &prob;

    prob.add_field(1, "vec", 3);
    EXPECT_EQ(prob.get_field_id("vec"), 0);

    EXPECT_DEATH(prob.add_field(1, "dup", 1),
                 "\\[ERROR\\] Cannot add field 'dup' with ID = 1. ID already exists\\.");
    prob.set_field_component_name(1, 0, "x");
    prob.set_field_component_name(1, 1, "y");
    prob.set_field_component_name(1, 2, "z");
    EXPECT_DEATH(prob.set_field_component_name(65536, 0, "A"),
                 "\\[ERROR\\] Field with ID = '65536' does not exist\\.");

    mesh.create();
    prob.create();

    EXPECT_EQ(prob.get_num_fields(), 1);
    EXPECT_THAT(prob.get_field_names(), testing::ElementsAre(""));

    EXPECT_EQ(prob.get_field_component_name(0, 1).compare("vec_x"), 0);
    EXPECT_EQ(prob.get_field_component_name(0, 2).compare("vec_y"), 0);
    EXPECT_EQ(prob.get_field_component_name(0, 3).compare("vec_z"), 0);
}

TEST(ExplicitFVLinearProblemTest, solve)
{
    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    InputParameters prob_pars = TestExplicitFVLinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 1e-3;
    prob_pars.set<PetscReal>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFVLinearProblem prob(prob_pars);
    app.problem = &prob;

    InputParameters bc_left_pars = TestBC::valid_params();
    bc_left_pars.set<const App *>("_app") = &app;
    bc_left_pars.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_left_pars.set<std::string>("boundary") = "left";
    bc_left_pars.set<bool>("inlet") = true;
    TestBC bc_left(bc_left_pars);
    prob.add_boundary_condition(&bc_left);

    InputParameters bc_right_pars = TestBC::valid_params();
    bc_right_pars.set<const App *>("_app") = &app;
    bc_right_pars.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_right_pars.set<std::string>("boundary") = "right";
    bc_right_pars.set<bool>("inlet") = false;
    TestBC bc_right(bc_right_pars);
    prob.add_boundary_condition(&bc_right);

    mesh.create();
    prob.create();
    prob.check();

    prob.run();

    EXPECT_TRUE(prob.converged());

    Vec sln = prob.get_solution_vector();
    PetscInt ni = 2;
    PetscInt ix[2] = { 0, 1 };
    PetscScalar x[2];
    VecGetValues(sln, ni, ix, x);
    EXPECT_NEAR(x[0], 0.001, 1e-15);
    EXPECT_NEAR(x[1], 0., 1e-15);
}

TEST(ExplicitFVLinearProblemTest, set_schemes)
{
    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    InputParameters prob_pars = TestExplicitFVLinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 1e-3;
    prob_pars.set<PetscReal>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFVLinearProblem prob(prob_pars);

    mesh.create();
    prob.create();

    TS ts = prob.get_ts();
    TSType type;
    std::vector<std::string> schemes = { "euler", "ssp", "rk" };
    std::vector<TSType> types = { TSEULER, TSSSP, TSRK };
    for (std::size_t i = 0; i < schemes.size(); i++) {
        prob_pars.set<std::string>("scheme") = schemes[i];
        prob.set_up_time_scheme();
        TSGetType(ts, &type);
        EXPECT_STREQ(type, types[i]);
    }
}

TEST(ExplicitFVLinearProblemTest, wrong_schemes)
{
    testing::internal::CaptureStderr();

    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    InputParameters prob_pars = TestExplicitFVLinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 1e-3;
    prob_pars.set<PetscReal>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "asdf";
    TestExplicitFVLinearProblem prob(prob_pars);

    mesh.create();
    prob.create();

    prob.check();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'scheme' parameter can be either 'euler', 'ssp' or 'rk'."));
}
