#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/Mesh.h"
#include "godzilla/LineMesh.h"
#include "godzilla/LinearProblem.h"
#include "godzilla/VTKOutput.h"
#include "petscdmda.h"

using namespace godzilla;

namespace {

class TestProblem : public LinearProblem {
public:
    explicit TestProblem(const Parameters & params) : LinearProblem(params) {}
};

} // namespace

TEST(VTKOutputTest, wrong_mesh_type)
{
    class MeshDA1D : public Mesh {
    public:
        MeshDA1D(mpi::Communicator comm) : Mesh()
        {
            DM dm;
            DMDACreate1d(comm, DM_BOUNDARY_NONE, 1, 1, 1, nullptr, &dm);
            set_dm(dm);
        }
    };

    class TestMesh : public MeshObject {
    public:
        explicit TestMesh(const Parameters & params) : MeshObject(params) {}

        Qtr<Mesh>
        create_mesh() override
        {
            return Qtr<MeshDA1D>::alloc(get_comm());
        }
    };

    TestApp app;

    testing::internal::CaptureStderr();

    auto mesh_pars = TestMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    TestMesh mesh(mesh_pars);

    auto prob_pars = TestProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    TestProblem prob(prob_pars);

    auto pars = VTKOutput::parameters();
    pars.set<App *>("_app", &app);
    pars.set<Problem *>("_problem", &prob);
    pars.set<MeshObject *>("_mesh_obj", &mesh);
    VTKOutput out(pars);
    prob.add_output(&out);

    mesh.create();
    prob.create();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("VTK output works only with unstructured meshes."));
}

TEST(VTKOutputTest, test)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto prob_pars = TestProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    TestProblem prob(prob_pars);

    auto pars = VTKOutput::parameters();
    pars.set<App *>("_app", &app);
    pars.set<Problem *>("_problem", &prob);
    pars.set<MeshObject *>("_mesh_obj", &mesh);
    VTKOutput out(pars);

    prob.add_output(&out);
    prob.create();

    EXPECT_EQ(out.get_file_name(), ".vtk");
    out.output_step();
}
