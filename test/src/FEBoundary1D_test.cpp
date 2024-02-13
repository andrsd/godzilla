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

class TestMesh1D : public MeshObject {
public:
    explicit TestMesh1D(const godzilla::Parameters & parameters) : MeshObject(parameters) {}

    Mesh *
    create_mesh() override
    {
        const Int DIM = 1;
        const Int N_ELEM_NODES = 2;
        std::vector<Int> cells = { 0, 1, 1, 2 };
        std::vector<Real> coords = { 0, 0.4, 1 };
        auto m = UnstructuredMesh::build_from_cell_list(get_comm(),
                                                            DIM,
                                                            N_ELEM_NODES,
                                                            cells,
                                                            DIM,
                                                            coords,
                                                            true);

        // create "side sets"
        auto face_sets = m->create_label("Face Sets");

        create_side_set(m, face_sets, 1, { 2 }, "left");
        create_side_set(m, face_sets, 2, { 4 }, "right");

        std::map<Int, std::string> face_set_names;
        face_set_names[1] = "left";
        face_set_names[2] = "right";
        m->create_face_set_labels(face_set_names);
        for (auto it : face_set_names)
            m->set_face_set_name(it.first, it.second);

        return m;
    }

    void
    create_side_set(UnstructuredMesh * mesh, Label & face_sets, Int id, const std::vector<Int> & faces, const char * name)
    {
        for (auto & f : faces) {
            face_sets.set_value(f, id);
            PETSC_CHECK(DMSetLabelValue(mesh->get_dm(), name, f, id));
        }
    }

public:
    static godzilla::Parameters parameters();
};

class TestBoundary1D : public fe::BoundaryInfo<EDGE2, 1, 2> {
public:
    TestBoundary1D(UnstructuredMesh * mesh,
                   const Array1D<DenseMatrix<Real, 2, 1>> * grad_phi,
                   const IndexSet & facets) :
        fe::BoundaryInfo<EDGE2, 1, 2>(mesh, grad_phi, facets)
    {
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        compute_face_normals();
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
TestMesh1D::parameters()
{
    Parameters params = MeshObject::parameters();
    return params;
}

} // namespace

TEST(FEBoundaryTest, nodal_normals_1d)
{
    TestApp app;

    Parameters mesh_pars = TestMesh1D::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    TestMesh1D mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();

    auto coords = fe::coordinates<1>(*m);
    auto connect = fe::connectivity<1, 2>(*m);
    auto fe_volume = fe::calc_volumes<EDGE2, 1>(coords, connect);
    auto grad_phi = fe::calc_grad_shape<EDGE2, 1>(coords, connect, fe_volume);

    {
        IndexSet bnd_facets = points_from_label(m->get_label("left"));
        TestBoundary1D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.length(0), 1.);
        bnd.destroy();
    }

    {
        IndexSet bnd_facets = points_from_label(m->get_label("right"));
        TestBoundary1D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.length(0), 1.);
        bnd.destroy();
    }
}
