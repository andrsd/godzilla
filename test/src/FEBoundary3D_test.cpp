#include "gmock/gmock.h"
#include "TestApp.h"
#include "FEGeometry.h"
#include "FEVolumes.h"
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

}; // namespace

namespace {

class TestMesh3D : public godzilla::UnstructuredMesh {
public:
    explicit TestMesh3D(const godzilla::Parameters & parameters) : UnstructuredMesh(parameters) {}

protected:
    void
    create_dm() override
    {
        const PetscInt DIM = 3;
        const PetscInt N_ELEM_NODES = 4;

        PetscInt n_cells = 1;
        PetscInt n_verts = 4;
        PetscInt cells[] = { 0, 1, 2, 3 };
        double coords[] = { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
        PETSC_CHECK(DMPlexCreateFromCellListPetsc(get_comm(),
                                                  DIM,
                                                  n_cells,
                                                  n_verts,
                                                  N_ELEM_NODES,
                                                  PETSC_TRUE,
                                                  cells,
                                                  DIM,
                                                  coords,
                                                  &this->dm));

        // create "side sets"
        DMLabel face_sets = create_label("Face Sets");

        create_side_set(face_sets, 1, { 5 }, "front");
        create_side_set(face_sets, 2, { 6 }, "bottom");
        create_side_set(face_sets, 3, { 7 }, "left");
        create_side_set(face_sets, 4, { 8 }, "slanted");

        std::map<Int, std::string> face_set_names;
        face_set_names[1] = "front";
        face_set_names[2] = "bottom";
        face_set_names[3] = "left";
        face_set_names[4] = "slanted";
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
            PETSC_CHECK(DMSetLabelValue(this->dm, name, f, id));
        }
    }

public:
    static godzilla::Parameters parameters();
};

class TestBoundary3D : public fe::BoundaryInfo<TET4, 3, 4> {
public:
    TestBoundary3D(const UnstructuredMesh * mesh,
                   const Array1D<DenseVector<Real, 3>> * coords,
                   const Array1D<DenseVector<Int, 4>> * connect,
                   const Array1D<std::vector<Int>> * nelcom,
                   const Array1D<Real> * fe_volume,
                   const Array1D<DenseVector<DenseVector<Real, 3>, 4>> * grad_phi,
                   const IndexSet & facets) :
        fe::BoundaryInfo<TET4, 3, 4>(mesh, coords, connect, nelcom, fe_volume, grad_phi, facets)
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
TestMesh3D::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    return params;
}

} // namespace

TEST(FEBoundaryTest, nodal_normals_3d)
{
    /*
    TestApp app;

    Parameters mesh_pars = TestMesh3D::parameters();
    mesh_pars.set<const godzilla::App *>("_app") = &app;
    TestMesh3D mesh(mesh_pars);
    mesh.create();

    auto coords = fe::coordinates<3>(mesh);
    auto connect = fe::connectivity<3, 4>(mesh);
    auto fe_volume = fe::calc_volumes<TET4, 3>(coords, connect);
    auto grad_phi = fe::calc_grad_shape<TET4, 3>(coords, connect, fe_volume);

    auto n_nodes = mesh.get_num_vertices();

    Array1D<std::vector<Int>> nelcom;
    nelcom.create(n_nodes);
    fe::common_elements_by_node(connect, nelcom);

    {
        auto label = mesh.get_label("left");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary3D bnd(&mesh, &coords, &connect, &nelcom, &fe_volume, &grad_phi, bnd_facets);
         bnd.create();
         bnd.destroy();
    }
    */
}
