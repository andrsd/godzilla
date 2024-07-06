#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/MeshObject.h"
#include "godzilla/UnstructuredMesh.h"
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

class TestMesh2D : public MeshObject {
public:
    explicit TestMesh2D(const Parameters & parameters) : MeshObject(parameters) {}

    Mesh *
    create_mesh() override
    {
        const Int DIM = 2;
        const Int N_ELEM_NODES = 3;
        std::vector<Int> cells = { 0, 1, 2, 1, 3, 2 };
        std::vector<Real> coords = { 0, 0, 1, 0, 0, 1, 1, 1 };
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

        create_side_set(m, face_sets, 1, { 8 }, "left");
        create_side_set(m, face_sets, 2, { 6 }, "bottom");
        create_side_set(m, face_sets, 3, { 9, 10 }, "top_right");

        std::map<Int, std::string> face_set_names;
        face_set_names[1] = "left";
        face_set_names[2] = "bottom";
        face_set_names[3] = "top_right";
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

class TestBoundary2D : public fe::BoundaryInfo<TRI3, 2, 3> {
public:
    TestBoundary2D(UnstructuredMesh * mesh,
                   const Array1D<DenseMatrix<Real, 3, 2>> * grad_phi,
                   const IndexSet & facets) :
        fe::BoundaryInfo<TRI3, 2, 3>(mesh, grad_phi, facets)
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
TestMesh2D::parameters()
{
    Parameters params = MeshObject::parameters();
    return params;
}

} // namespace

TEST(FEBoundaryTest, nodal_normals_2d)
{
    TestApp app;

    Parameters mesh_pars = TestMesh2D::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    TestMesh2D mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto coords = fe::coordinates<2>(*m);
    auto connect = fe::connectivity<2, 3>(*m);
    auto fe_volume = fe::calc_volumes<TRI3, 2>(coords, connect);
    auto grad_phi = fe::calc_grad_shape<TRI3, 2>(coords, connect, fe_volume);

    {
        auto label = m->get_label("left");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary2D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.length(0), 1.);

        bnd.destroy();
    }

    {
        auto label = m->get_label("bottom");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary2D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), -1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), -1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), -1);

        EXPECT_DOUBLE_EQ(bnd.length(0), 1.);

        bnd.destroy();
    }

    {
        auto label = m->get_label("top_right");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary2D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.normal(1)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(1)(1), 1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), 1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(0), 1. / std::sqrt(2));
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(1), 1. / std::sqrt(2));

        EXPECT_DOUBLE_EQ(bnd.length(0), 1.);
        EXPECT_DOUBLE_EQ(bnd.length(1), 1.);

        bnd.destroy();
    }
}
