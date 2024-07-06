#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/MeshObject.h"
#include "godzilla/FEGeometry.h"
#include "godzilla/FEVolumes.h"
#include "godzilla/FEShapeFns.h"
#include "godzilla/FEBoundary.h"

using namespace godzilla;
using namespace testing;

namespace {

IndexSet
points_from_label(const Label & label)
{
    auto is = label.get_value_index_set();
    auto ids = label.get_values();
    return label.get_stratum(ids[0]);
}

} // namespace

namespace {

class TestMesh3D : public MeshObject {
public:
    explicit TestMesh3D(const godzilla::Parameters & parameters) : MeshObject(parameters) {}

    Mesh *
    create_mesh() override
    {
        const Int DIM = 3;
        const Int N_ELEM_NODES = 4;
        std::vector<Int> cells = { 0, 1, 2, 3 };
        std::vector<Real> coords = { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
        auto m = UnstructuredMesh::build_from_cell_list(get_comm(),
                                                        DIM,
                                                        N_ELEM_NODES,
                                                        cells,
                                                        DIM,
                                                        coords,
                                                        true);

        // create "side sets"
        m->create_label("Face Sets");
        auto face_sets = m->get_label("Face Sets");

        create_side_set(m, face_sets, 1, { 6 }, "front");
        create_side_set(m, face_sets, 2, { 5 }, "bottom");
        create_side_set(m, face_sets, 3, { 7 }, "left");
        create_side_set(m, face_sets, 4, { 8 }, "slanted");

        std::map<Int, std::string> face_set_names;
        face_set_names[1] = "front";
        face_set_names[2] = "bottom";
        face_set_names[3] = "left";
        face_set_names[4] = "slanted";
        m->create_face_set_labels(face_set_names);
        for (auto it : face_set_names)
            m->set_face_set_name(it.first, it.second);

        return m;
    }

    void
    create_side_set(UnstructuredMesh * mesh,
                    Label & face_sets,
                    Int id,
                    const std::vector<Int> & faces,
                    const char * name)
    {
        for (auto & f : faces) {
            face_sets.set_value(f, id);
            PETSC_CHECK(DMSetLabelValue(mesh->get_dm(), name, f, id));
        }
    }

public:
    static godzilla::Parameters parameters();
};

class TestBoundary3D : public fe::BoundaryInfo<TET4, 3, 4> {
public:
    TestBoundary3D(UnstructuredMesh * mesh,
                   const Array1D<DenseMatrix<Real, 4, 3>> * grad_phi,
                   const IndexSet & facets) :
        fe::BoundaryInfo<TET4, 3, 4>(mesh, grad_phi, facets)
    {
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        compute_face_normals();
        create_vertex_index_set();
        compute_nodal_normals();
    }

    void
    destroy() override
    {
        CALL_STACK_MSG();
        free();
    }
};

Parameters
TestMesh3D::parameters()
{
    Parameters params = MeshObject::parameters();
    return params;
}

} // namespace

TEST(FEBoundaryTest, nodal_normals_3d)
{
    /*
    TestApp app;

    Parameters mesh_pars = TestMesh3D::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    TestMesh3D mesh(mesh_pars);
    mesh.create();

    auto coords = fe::coordinates<3>(mesh);
    auto connect = fe::connectivity<3, 4>(mesh);
    auto fe_volume = fe::calc_volumes<TET4, 3>(coords, connect);
    auto grad_phi = fe::calc_grad_shape<TET4, 3>(coords, connect, fe_volume);

    {
        auto label = mesh.get_label("left");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary3D bnd(&mesh, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0);
        bnd.destroy();
    }
    {
        auto label = mesh.get_label("front");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary3D bnd(&mesh, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0.57735026918962584);
        bnd.destroy();
    }
    {
        auto label = mesh.get_label("bottom");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary3D bnd(&mesh, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), -1);
        bnd.destroy();
    }
    {
        auto label = mesh.get_label("slanted");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary3D bnd(&mesh, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0);
        bnd.destroy();
    }
    */
}
