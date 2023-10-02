#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "LineMesh.h"
#include "Problem.h"
#include "Function.h"
#include "Output.h"
#include "Postprocessor.h"

using namespace godzilla;

namespace {

class TestProblem : public Problem {
public:
    explicit TestProblem(const Parameters & params) : Problem(params) {}

    void
    run() override
    {
    }

    const Vector &
    get_solution_vector() const override
    {
        return this->sln;
    }

    Vector sln;
};

Section
create_section(DM dm)
{
    DMSetNumFields(dm, 1);
    Int nc[1] = { 1 };
    Int n_dofs[2] = { 1, 0 };
    return Section::create(dm, nc, n_dofs, 0, nullptr, nullptr, nullptr, nullptr);
}

} // namespace

TEST(ProblemTest, add_pp)
{
    TestApp app;

    class TestPostprocessor : public Postprocessor {
    public:
        explicit TestPostprocessor(const Parameters & params) : Postprocessor(params) {}

        void
        compute() override
        {
        }

        Real
        get_value() override
        {
            return 0;
        }

        MOCK_METHOD(void, check, ());
    };

    class TestFunction : public Function {
    public:
        explicit TestFunction(const Parameters & params) : Function(params) {}

        void
        register_callback(mu::Parser & parser) override
        {
        }

        MOCK_METHOD(void, check, ());
    };

    class TestOutput : public Output {
    public:
        explicit TestOutput(const Parameters & params) : Output(params) {}

        MOCK_METHOD(void, output_step, ());
    };

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = Problem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<Mesh *>("_mesh") = &mesh;
    TestProblem problem(prob_params);

    Parameters pp_params = Postprocessor::parameters();
    pp_params.set<const App *>("_app") = &app;
    pp_params.set<Problem *>("_problem") = &problem;
    pp_params.set<std::string>("_name") = "pp";
    TestPostprocessor pp(pp_params);
    problem.add_postprocessor(&pp);

    Parameters fn_params = Function::parameters();
    fn_params.set<const App *>("_app") = &app;
    fn_params.set<Problem *>("_problem") = &problem;
    fn_params.set<std::string>("_name") = "fn";
    TestFunction fn(fn_params);
    problem.add_function(&fn);

    Parameters out_params = Function::parameters();
    out_params.set<const App *>("_app") = &app;
    out_params.set<Problem *>("_problem") = &problem;
    out_params.set<std::string>("_name") = "out";
    out_params.set<std::vector<std::string>>("on") = { "initial" };
    out_params.set<Int>("interval") = 1;
    TestOutput out(out_params);
    out.create();
    problem.add_output(&out);

    EXPECT_CALL(fn, check);
    EXPECT_CALL(pp, check);
    problem.check();

    EXPECT_EQ(problem.get_postprocessor("pp"), &pp);
    EXPECT_EQ(problem.get_postprocessor("asdf"), nullptr);

    auto & pps_names = problem.get_postprocessor_names();
    EXPECT_EQ(pps_names.size(), 1);
    EXPECT_EQ(pps_names[0], "pp");

    EXPECT_CALL(out, output_step);
    problem.output(Output::ON_INITIAL);
}

TEST(ProblemTest, local_vec)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<Mesh *>("_mesh") = &mesh;
    TestProblem problem(prob_params);
    problem.set_local_section(create_section(mesh.dm()));

    Vector loc_vec = problem.get_local_vector();
    EXPECT_EQ(loc_vec.get_size(), 3);
    problem.restore_local_vector(loc_vec);

    Vector vec = problem.create_local_vector();
    EXPECT_EQ(vec.get_size(), 3);
    vec.destroy();
}

TEST(ProblemTest, global_vec)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<Mesh *>("_mesh") = &mesh;
    TestProblem problem(prob_params);
    problem.set_local_section(create_section(mesh.dm()));

    Vector glob_vec = problem.get_global_vector();
    EXPECT_EQ(glob_vec.get_size(), 3);
    problem.restore_global_vector(glob_vec);

    Vector vec = problem.create_global_vector();
    EXPECT_EQ(vec.get_size(), 3);
    vec.destroy();
}

TEST(ProblemTest, create_matrix)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<Mesh *>("_mesh") = &mesh;
    TestProblem problem(prob_params);
    problem.create();
    problem.set_local_section(create_section(mesh.dm()));

    Matrix mat = problem.create_matrix();
    EXPECT_EQ(mat.get_n_rows(), 3);
    EXPECT_EQ(mat.get_n_cols(), 3);
    mat.destroy();
}

TEST(UnstructuredMeshTest, get_local_section)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<Mesh *>("_mesh") = &mesh;
    TestProblem problem(prob_params);
    problem.create();

    auto dm = mesh.dm();
    Section s = create_section(dm);
    problem.set_local_section(s);

    Section ls = problem.get_local_section();

    PetscBool congruent = PETSC_FALSE;
    PetscSectionCompare(s, ls, &congruent);
    EXPECT_EQ(congruent, PETSC_TRUE);
}

TEST(UnstructuredMeshTest, get_global_section)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<Mesh *>("_mesh") = &mesh;
    TestProblem problem(prob_params);
    problem.create();

    auto dm = mesh.dm();
    Section s = create_section(dm);
    problem.set_local_section(s);
    problem.set_global_section(s);

    Section ls = problem.get_global_section();

    PetscBool congruent = PETSC_FALSE;
    PetscSectionCompare(s, ls, &congruent);
    EXPECT_EQ(congruent, PETSC_TRUE);
}
