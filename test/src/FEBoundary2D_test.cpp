#include "gmock/gmock.h"
#include "TestApp.h"
#include "FEGeometry.h"
#include "FEVolumes.h"
#include "FEShapeFns.h"
#include "FEBoundary.h"

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

class TestMesh2D : public godzilla::UnstructuredMesh {
public:
    explicit TestMesh2D(const godzilla::Parameters & parameters) : UnstructuredMesh(parameters) {}

protected:
    void
    create_dm() override
    {
        const PetscInt DIM = 2;
        const PetscInt N_ELEM_NODES = 3;
        std::vector<Int> cells = { 0, 1, 2, 1, 3, 2 };
        std::vector<Real> coords = { 0, 0, 1, 0, 0, 1, 1, 1 };
        build_from_cell_list(DIM, N_ELEM_NODES, cells, DIM, coords, true);

        // create "side sets"
        DMLabel face_sets = create_label("Face Sets");

        create_side_set(face_sets, 1, { 8 }, "left");
        create_side_set(face_sets, 2, { 6 }, "bottom");
        create_side_set(face_sets, 3, { 9, 10 }, "top_right");

        std::map<Int, std::string> face_set_names;
        face_set_names[1] = "left";
        face_set_names[2] = "bottom";
        face_set_names[3] = "top_right";
        create_face_set_labels(face_set_names);
        for (auto it : face_set_names)
            set_face_set_name(it.first, it.second);
    }

    void
    create_side_set(const DMLabel & face_sets,
                    Int id,
                    const std::vector<Int> & faces,
                    const char * name)
    {
        for (auto & f : faces) {
            PETSC_CHECK(DMLabelSetValue(face_sets, f, id));
            PETSC_CHECK(DMSetLabelValue(dm(), name, f, id));
        }
    }

public:
    static godzilla::Parameters parameters();
};

class TestBoundary2D : public fe::BoundaryInfo<TRI3, 2, 3> {
public:
    TestBoundary2D(const UnstructuredMesh * mesh,
                   const Array1D<DenseVector<Real, 2>> * coords,
                   const Array1D<DenseVector<Int, 3>> * connect,
                   const Array1D<std::vector<Int>> * nelcom,
                   const Array1D<Real> * fe_volume,
                   const Array1D<DenseMatrix<Real, 3, 2>> * grad_phi,
                   const IndexSet & facets) :
        fe::BoundaryInfo<TRI3, 2, 3>(mesh, coords, connect, nelcom, fe_volume, grad_phi, facets)
    {
    }

    void
    create() override
    {
        _F_;
        compute_face_normals();
        compute_nodal_normals();
    }

    void
    destroy() override
    {
        _F_;
        free();
    }
};

Parameters
TestMesh2D::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    return params;
}

} // namespace

TEST(FEBoundaryTest, nodal_normals_2d)
{
    TestApp app;

    Parameters mesh_pars = TestMesh2D::parameters();
    mesh_pars.set<const godzilla::App *>("_app") = &app;
    TestMesh2D mesh(mesh_pars);
    mesh.create();

    auto coords = fe::coordinates<2>(mesh);
    auto connect = fe::connectivity<2, 3>(mesh);
    auto fe_volume = fe::calc_volumes<TRI3, 2>(coords, connect);
    auto grad_phi = fe::calc_grad_shape<TRI3, 2>(coords, connect, fe_volume);

    auto n_nodes = mesh.get_num_vertices();

    Array1D<std::vector<Int>> nelcom;
    nelcom.create(n_nodes);
    fe::common_elements_by_node(connect, nelcom);

    {
        auto label = mesh.get_label("left");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary2D bnd(&mesh, &coords, &connect, &nelcom, &fe_volume, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), 0);
        bnd.destroy();
    }

    {
        auto label = mesh.get_label("bottom");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary2D bnd(&mesh, &coords, &connect, &nelcom, &fe_volume, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), -1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), -1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), -1);
        bnd.destroy();
    }

    {
        auto label = mesh.get_label("top_right");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary2D bnd(&mesh, &coords, &connect, &nelcom, &fe_volume, &grad_phi, bnd_facets);
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
        bnd.destroy();
    }
}
