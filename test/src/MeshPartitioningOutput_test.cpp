#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "LinearProblem_test.h"
#include "godzilla/LineMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/MeshPartitioningOutput.h"
#include "petsc.h"
#include "petscviewerhdf5.h"

namespace {

class EmptyMesh : public Mesh {
public:
    EmptyMesh() : Mesh() {}

    void
    distribute() override
    {
    }
};

class EmptyMeshObject : public MeshObject {
public:
    explicit EmptyMeshObject(const Parameters & params) : MeshObject(params) {}

    Mesh *
    create_mesh() override
    {
        return new EmptyMesh();
    }
};

} // namespace

TEST(MeshPartitioningOutputTest, get_file_ext)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Real>("xmin") = 0;
    mesh_params.set<Real>("xmax") = 1;
    mesh_params.set<Int>("nx") = 4;
    LineMesh mesh(mesh_params);

    Parameters prob_params = G1DTestLinearProblem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    G1DTestLinearProblem prob(prob_params);

    Parameters params = MeshPartitioningOutput::parameters();
    params.set<App *>("_app") = &app;
    params.set<Problem *>("_problem") = &prob;
    MeshPartitioningOutput out(params);

    EXPECT_EQ(out.get_file_ext(), "h5");
}

TEST(MeshPartitioningOutputTest, output)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Real>("xmin") = 0;
    mesh_params.set<Real>("xmax") = 1;
    mesh_params.set<Int>("nx") = 4;
    LineMesh mesh(mesh_params);

    Parameters prob_params = G1DTestLinearProblem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    G1DTestLinearProblem prob(prob_params);

    Parameters params = MeshPartitioningOutput::parameters();
    params.set<App *>("_app") = &app;
    params.set<Problem *>("_problem") = &prob;
    MeshPartitioningOutput out(params);
    prob.add_output(&out);

    mesh.create();
    prob.create();

    out.output_step();

    PetscViewer viewer;
    Vec p;
    VecCreate(app.get_comm(), &p);
    PetscObjectSetName((PetscObject) p, "fields/partitioning");
    PetscViewerHDF5Open(app.get_comm(), "part.h5", FILE_MODE_READ, &viewer);
    VecLoad(p, viewer);
    Real l2_norm;
    VecNorm(p, NORM_2, &l2_norm);
    EXPECT_NEAR(l2_norm, 0, 1e-10);
    VecDestroy(&p);
    PetscViewerDestroy(&viewer);
}
