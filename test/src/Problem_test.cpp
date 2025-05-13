#include "gmock/gmock.h"
#include <petscdm.h>
#include "GodzillaApp_test.h"
#include "godzilla/Error.h"
#include "godzilla/Mesh.h"
#include "godzilla/LineMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/Function.h"
#include "godzilla/FileOutput.h"
#include "godzilla/Postprocessor.h"
#include "godzilla/Section.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Array1D.h"
#include "ExceptionTestMacros.h"
#include "petscsystypes.h"
#include "petscfe.h"

using namespace godzilla;

namespace {

class TestProblem : public Problem {
public:
    explicit TestProblem(const Parameters & params) : Problem(params) {}

    void
    run() override
    {
    }
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
    };

    class TestFunction : public Function {
    public:
        explicit TestFunction(const Parameters & params) : Function(params) {}

        void
        register_callback(mu::Parser & parser) override
        {
        }
    };

    class TestOutput : public FileOutput {
    public:
        explicit TestOutput(const Parameters & params) : FileOutput(params) {}

        MOCK_METHOD(void, output_step, ());

        std::string
        get_file_ext() const override
        {
            return "none";
        }
    };

    auto mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);

    auto prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);

    auto pp_params = Postprocessor::parameters();
    pp_params.set<App *>("_app") = &app;
    pp_params.set<Problem *>("_problem") = &problem;
    pp_params.set<std::string>("_name") = "pp";
    TestPostprocessor pp(pp_params);
    problem.add_postprocessor(&pp);

    auto fn_params = Function::parameters();
    fn_params.set<App *>("_app") = &app;
    fn_params.set<Problem *>("_problem") = &problem;
    fn_params.set<std::string>("_name") = "fn";
    TestFunction fn(fn_params);
    problem.add_function(&fn);

    auto out_params = FileOutput::parameters();
    out_params.set<App *>("_app") = &app;
    out_params.set<Problem *>("_problem") = &problem;
    out_params.set<std::string>("_name") = "out";
    out_params.set<std::string>("file") = "file";
    out_params.set<std::vector<std::string>>("on") = { "initial" };
    out_params.set<Int>("interval") = 1;
    TestOutput out(out_params);
    out.create();
    problem.add_output(&out);

    EXPECT_EQ(problem.get_postprocessor("pp"), &pp);
    EXPECT_EQ(problem.get_postprocessor("asdf"), nullptr);

    auto & pps_names = problem.get_postprocessor_names();
    EXPECT_EQ(pps_names.size(), 1);
    EXPECT_EQ(pps_names[0], "pp");

    EXPECT_CALL(out, output_step);
    problem.output(EXECUTE_ON_INITIAL);
}

TEST(ProblemTest, local_vec)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();
    auto m = mesh.get_mesh<Mesh>();

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);
    problem.set_local_section(create_section(m->get_dm()));

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
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);

    auto m = mesh.get_mesh<Mesh>();
    problem.set_local_section(create_section(m->get_dm()));

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
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);
    problem.create();

    auto m = mesh.get_mesh<Mesh>();
    problem.set_local_section(create_section(m->get_dm()));

    Matrix mat = problem.create_matrix();
    EXPECT_EQ(mat.get_n_rows(), 3);
    EXPECT_EQ(mat.get_n_cols(), 3);

    mat.destroy();
}

TEST(ProblemTest, get_local_section)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);
    problem.create();

    auto m = mesh.get_mesh<Mesh>();
    auto dm = m->get_dm();
    Section s = create_section(dm);
    problem.set_local_section(s);

    Section ls = problem.get_local_section();

    PetscBool congruent = PETSC_FALSE;
    PetscSectionCompare(s, ls, &congruent);
    EXPECT_EQ(congruent, PETSC_TRUE);
}

TEST(ProblemTest, get_global_section)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);
    problem.create();

    auto m = mesh.get_mesh<Mesh>();
    auto dm = m->get_dm();
    Section s = create_section(dm);
    problem.set_local_section(s);
    problem.set_global_section(s);

    Section ls = problem.get_global_section();

    PetscBool congruent = PETSC_FALSE;
    PetscSectionCompare(s, ls, &congruent);
    EXPECT_EQ(congruent, PETSC_TRUE);
}

TEST(ProblemTest, aux_vecs)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);
    app.set_problem(&problem);

    mesh.create();
    problem.create();

    auto aux_0 = Vector::create_seq(app.get_comm(), 1);
    problem.set_auxiliary_vec(Label(), 0, 0, aux_0);
    auto aux_1 = Vector::create_seq(app.get_comm(), 3);
    problem.set_auxiliary_vec(Label(), 0, 1, aux_1);

    EXPECT_EQ(problem.get_num_auxiliary_vec(), 2);

    auto a0 = problem.get_auxiliary_vec(Label(), 0, 0);
    EXPECT_EQ(a0.get_size(), 1);

    auto a1 = problem.get_auxiliary_vec(Label(), 0, 1);
    EXPECT_EQ(a1.get_size(), 3);
}

TEST(ProblemTest, aux_vecs_clear)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);
    app.set_problem(&problem);

    mesh.create();
    problem.create();

#if PETSC_VERSION_GE(3, 21, 0)
#else
    EXPECT_THROW_MSG({ problem.clear_auxiliary_vec(); },
                     "You need PETSC 3.21+ for `Problem::clear_auxiliary_vec()`");
#endif
}

TEST(ProblemTest, mat_vec_types)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);

    EXPECT_EQ(problem.get_vector_type(), VECSTANDARD);
    EXPECT_EQ(problem.get_matrix_type(), MATAIJ);
}

TEST(ProblemTest, loc_glob_arithmetic_type)
{
    TestApp app;
    auto comm = app.get_comm();

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 9;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);

    mesh.create();
    problem.create();

    auto part = problem.get_partitioner();
    part.set_up();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    m->set_partitioner(part);
    m->distribute(problem.get_partition_overlap());

    DM dm = clone(problem.get_dm());
    PetscBool simplex = PETSC_TRUE;
    PetscFE fe;
    constexpr int DIM = 1;
    PETSC_CHECK(PetscFECreateLagrange(comm, DIM, 1, simplex, 1, PETSC_DECIDE, &fe));
    PETSC_CHECK(DMSetField(dm, 0, nullptr, (PetscObject) fe));
    PETSC_CHECK(DMCreateDS(dm));

    auto l = create_local_array1d<Real>(dm);
    l.set(comm.rank() + 1);
    auto g = create_global_array1d<Real>(dm);
    g.set(0.);
    local_to_global(dm, l, ADD_VALUES, g);
    global_to_local(dm, g, INSERT_VALUES, l);

    EXPECT_NEAR(l(0), 1., 1e-10);
    EXPECT_NEAR(l(1), 1., 1e-10);
    EXPECT_NEAR(l(2), 1., 1e-10);
    EXPECT_NEAR(l(3), 1., 1e-10);
    EXPECT_NEAR(l(4), 1., 1e-10);
    EXPECT_NEAR(l(5), 1., 1e-10);
    EXPECT_NEAR(l(6), 1., 1e-10);
    EXPECT_NEAR(l(7), 1., 1e-10);
    EXPECT_NEAR(l(8), 1., 1e-10);
    EXPECT_NEAR(l(9), 1., 1e-10);

    PETSC_CHECK(DMDestroy(&dm));
}

TEST(ProblemTest, loc_glob_arithmetic_type_min_max)
{
    TestApp app;
    auto comm = app.get_comm();

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 9;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);

    mesh.create();
    problem.create();

    auto part = problem.get_partitioner();
    part.set_up();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    m->set_partitioner(part);
    m->distribute(problem.get_partition_overlap());

    DM dm = clone(problem.get_dm());
    PetscBool simplex = PETSC_TRUE;
    PetscFE fe;
    constexpr int DIM = 1;
    PETSC_CHECK(PetscFECreateLagrange(comm, DIM, 1, simplex, 1, PETSC_DECIDE, &fe));
    PETSC_CHECK(DMSetField(dm, 0, nullptr, (PetscObject) fe));
    PETSC_CHECK(DMCreateDS(dm));

    auto l = create_local_array1d<Real>(dm);
    l.set(comm.rank() + 1);
    auto g = create_global_array1d<Real>(dm);
    g.set(0.);
    local_to_global(dm, l, MAX_VALUES, g);
    global_to_local(dm, g, INSERT_VALUES, l);

    EXPECT_NEAR(l(0), 1., 1e-10);
    EXPECT_NEAR(l(1), 1., 1e-10);
    EXPECT_NEAR(l(2), 1., 1e-10);
    EXPECT_NEAR(l(3), 1., 1e-10);
    EXPECT_NEAR(l(4), 1., 1e-10);
    EXPECT_NEAR(l(5), 1., 1e-10);
    EXPECT_NEAR(l(6), 1., 1e-10);
    EXPECT_NEAR(l(7), 1., 1e-10);
    EXPECT_NEAR(l(8), 1., 1e-10);
    EXPECT_NEAR(l(9), 1., 1e-10);

    PETSC_CHECK(DMDestroy(&dm));
}

TEST(ProblemTest, loc_glob_vec_type)
{
    TestApp app;
    auto comm = app.get_comm();

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 9;
    LineMesh mesh(mesh_params);
    mesh.create();

    Parameters prob_params = Problem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem problem(prob_params);

    mesh.create();
    problem.create();

    auto part = problem.get_partitioner();
    part.set_up();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    m->set_partitioner(part);
    m->distribute(problem.get_partition_overlap());

    DM dm = clone(problem.get_dm());
    PetscBool simplex = PETSC_TRUE;
    PetscFE fe;
    constexpr int DIM = 1;
    PETSC_CHECK(PetscFECreateLagrange(comm, DIM, 1, simplex, 1, PETSC_DECIDE, &fe));
    PETSC_CHECK(DMSetField(dm, 0, nullptr, (PetscObject) fe));
    PETSC_CHECK(DMCreateDS(dm));

    auto l = create_local_array1d<DenseVector<Real, 2>>(dm);
    double d = 5 * (comm.rank() + 1);
    l.set(DenseVector<Real, 2>({ d, d + 1. }));
    auto g = create_global_array1d<DenseVector<Real, 2>>(dm);
    g.set(DenseVector<Real, 2>({ 0., 0. }));
    local_to_global(dm, l, ADD_VALUES, g);
    global_to_local(dm, g, INSERT_VALUES, l);

    EXPECT_NEAR(l(0)(0), 5., 1e-10);
    EXPECT_NEAR(l(0)(1), 6., 1e-10);
    EXPECT_NEAR(l(1)(0), 5., 1e-10);
    EXPECT_NEAR(l(1)(1), 6., 1e-10);
    EXPECT_NEAR(l(2)(0), 5., 1e-10);
    EXPECT_NEAR(l(2)(1), 6., 1e-10);
    EXPECT_NEAR(l(3)(0), 5., 1e-10);
    EXPECT_NEAR(l(3)(1), 6., 1e-10);
    EXPECT_NEAR(l(4)(0), 5., 1e-10);
    EXPECT_NEAR(l(4)(1), 6., 1e-10);
    EXPECT_NEAR(l(5)(0), 5., 1e-10);
    EXPECT_NEAR(l(5)(1), 6., 1e-10);
    EXPECT_NEAR(l(6)(0), 5., 1e-10);
    EXPECT_NEAR(l(6)(1), 6., 1e-10);
    EXPECT_NEAR(l(7)(0), 5., 1e-10);
    EXPECT_NEAR(l(7)(1), 6., 1e-10);
    EXPECT_NEAR(l(8)(0), 5., 1e-10);
    EXPECT_NEAR(l(8)(1), 6., 1e-10);
    EXPECT_NEAR(l(9)(0), 5., 1e-10);
    EXPECT_NEAR(l(9)(1), 6., 1e-10);

    PETSC_CHECK(DMDestroy(&dm));
}
