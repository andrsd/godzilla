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

class TestMesh1D : public godzilla::UnstructuredMesh {
public:
    explicit TestMesh1D(const godzilla::Parameters & parameters) : UnstructuredMesh(parameters) {}

protected:
    void
    create_dm() override
    {
        const PetscInt DIM = 1;
        const PetscInt N_ELEM_NODES = 2;

        PetscInt n_cells = 2;
        PetscInt n_verts = 3;
        PetscInt cells[] = { 0, 1, 1, 2 };
        double coords[] = { 0, 0.4, 1 };
        PETSC_CHECK(DMPlexCreateFromCellListPetsc(comm(),
                                                  DIM,
                                                  n_cells,
                                                  n_verts,
                                                  N_ELEM_NODES,
                                                  PETSC_TRUE,
                                                  cells,
                                                  DIM,
                                                  coords,
                                                  &this->_dm));

        // create "side sets"
        DMLabel face_sets = create_label("Face Sets");

        create_side_set(face_sets, 1, { 2 }, "left");
        create_side_set(face_sets, 2, { 4 }, "right");

        std::map<Int, std::string> face_set_names;
        face_set_names[1] = "left";
        face_set_names[2] = "right";
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

class TestBoundary1D : public fe::BoundaryInfo<EDGE2, 1, 2> {
public:
    TestBoundary1D(const UnstructuredMesh * mesh,
                   const Array1D<DenseVector<Real, 1>> * coords,
                   const Array1D<DenseVector<Int, 2>> * connect,
                   const Array1D<std::vector<Int>> * nelcom,
                   const Array1D<Real> * fe_volume,
                   const Array1D<DenseVector<DenseVector<Real, 1>, 2>> * grad_phi,
                   const IndexSet & facets) :
        fe::BoundaryInfo<EDGE2, 1, 2>(mesh, coords, connect, nelcom, fe_volume, grad_phi, facets)
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
TestMesh1D::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    return params;
}

} // namespace

TEST(FEBoundaryTest, nodal_normals_1d)
{
    TestApp app;

    Parameters mesh_pars = TestMesh1D::parameters();
    mesh_pars.set<const godzilla::App *>("_app") = &app;
    TestMesh1D mesh(mesh_pars);
    mesh.create();

    auto coords = fe::coordinates<1>(mesh);
    auto connect = fe::connectivity<1, 2>(mesh);
    auto fe_volume = fe::calc_volumes<EDGE2, 1>(coords, connect);
    auto grad_phi = fe::calc_grad_shape<EDGE2, 1>(coords, connect, fe_volume);

    auto n_nodes = mesh.get_num_vertices();

    Array1D<std::vector<Int>> nelcom;
    nelcom.create(n_nodes);
    fe::common_elements_by_node(connect, nelcom);

    {
        IndexSet bnd_facets = points_from_label(mesh.get_label("left"));
        TestBoundary1D bnd(&mesh, &coords, &connect, &nelcom, &fe_volume, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), -1);
        bnd.destroy();
    }

    {
        IndexSet bnd_facets = points_from_label(mesh.get_label("right"));
        TestBoundary1D bnd(&mesh, &coords, &connect, &nelcom, &fe_volume, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 1);
        bnd.destroy();
    }
}
