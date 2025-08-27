#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "godzilla/LinearProblem.h"
#include "godzilla/Problem.h"
#include "godzilla/RestartFile.h"
#include "godzilla/RestartOutput.h"
#include "godzilla/Vector.h"
#include "petscmat.h"

using namespace godzilla;

namespace {

class CustomLinearProblem : public LinearProblem {
public:
    explicit CustomLinearProblem(const Parameters & params) : LinearProblem(params) {}

    void
    create() override
    {
        DMSetNumFields(get_dm(), 1);
        Int nc[1] = { 1 };
        Int n_dofs[2] = { 1, 0 };
        this->s = Section::create(get_dm(), nc, n_dofs, 0, nullptr, nullptr, nullptr, nullptr);
        set_local_section(this->s);
        LinearProblem::create();
    }

    void
    compute_operators(Matrix & A, Matrix & B)
    {
        A.set_value(0, 0, 2.);
        A.set_value(1, 1, 3.);
        A.assemble();
    }

    void
    compute_rhs(Vector & b)
    {
        b.set_values(std::vector<Int>({ 0, 1 }), { 20, 33 });
        b.assemble();
    }

    void
    set_up_callbacks() override
    {
        set_compute_operators(this, &CustomLinearProblem::compute_operators);
        set_compute_rhs(this, &CustomLinearProblem::compute_rhs);
    }

    Section s;
};

} // namespace

TEST(LinearProblemTest, run)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto prob_pars = LinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    prob_pars.set<std::string>("ksp_type", KSPCG);
    CustomLinearProblem prob(prob_pars);

    prob.create();
    prob.run();
    auto & sln = prob.get_solution_vector();
    EXPECT_NEAR(sln(0), 10, 1e-10);
    EXPECT_NEAR(sln(1), 11, 1e-10);

    // Const-version
    const Problem * cprob = &prob;
    const auto & csln = cprob->get_solution_vector();
    EXPECT_NEAR(csln(0), 10, 1e-10);
    EXPECT_NEAR(csln(1), 11, 1e-10);
}

TEST(LinearProblemTest, restart_file)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto prob_pars = LinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    prob_pars.set<std::string>("ksp_type", KSPCG);
    CustomLinearProblem prob(prob_pars);

    auto ro_pars = RestartOutput::parameters();
    ro_pars.set<App *>("_app", &app);
    ro_pars.set<Problem *>("_problem", &prob);
    ro_pars.set<std::string>("file", "lp");
    RestartOutput ro(ro_pars);
    prob.add_output(&ro);

    prob.create();
    prob.run();

    {
        RestartFile f("lp.restart.h5", FileAccess::READ);
        auto v = Vector::create_seq(app.get_comm(), 2);
        f.read<Vector>("sln", v);
        EXPECT_NEAR(v(0), 10, 1e-10);
        EXPECT_NEAR(v(1), 11, 1e-10);
    }
}
