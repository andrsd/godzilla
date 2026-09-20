#include "gmock/gmock.h"
#include "TestApp.h"
#include "TestMesh1D.h"
#include "godzilla/Array1D.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/FEGeometry.h"
#include "godzilla/FEVolumes.h"
#include "godzilla/FEShapeFns.h"
#include "godzilla/FEBoundary.h"

using namespace godzilla;

namespace {

class TestEssentialBoundary1D : public fe::EssentialBoundaryInfo<EDGE2, 1_D, 2> {
public:
    TestEssentialBoundary1D(Ref<UnstructuredMesh> mesh, const IndexSet & vertices) :
        fe::EssentialBoundaryInfo<EDGE2, 1_D, 2>(mesh, vertices),
        vals(get_mesh()->get_comm(), this->num_vertices())
    {
    }

    void
    compute()
    {
        for_each(vertices(), [&](Int idx, Int vtx) { this->vals[idx] = vtx; });
    }

    Array1D<Int> vals;
};

class TestNaturalBoundary1D : public fe::NaturalBoundaryInfo<EDGE2, 1_D, 2> {
public:
    TestNaturalBoundary1D(Ref<UnstructuredMesh> mesh,
                          Array1D<DenseMatrix<Real, 1, 2>> grad_phi,
                          const IndexSet & facets) :
        fe::NaturalBoundaryInfo<EDGE2, 1_D, 2>(mesh, grad_phi, facets),
        vals(get_mesh()->get_comm(), num_facets())
    {
    }

    TestNaturalBoundary1D(Ref<UnstructuredMesh> mesh, const IndexSet & facets) :
        fe::NaturalBoundaryInfo<EDGE2, 1_D, 2>(mesh, facets),
        vals(get_mesh()->get_comm(), num_facets())
    {
    }

    void
    compute()
    {
        for_each(facets(), [&](Int idx, Int facet) { this->vals[idx] = facet; });
    }

    Array1D<Int> vals;
};

} // namespace

TEST(FEBoundaryTest, test_1d)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<TestMesh1D>();
    auto mesh_qtr = MeshFactory::create<TestMesh1D>(mesh_pars);
    auto mesh = mesh_qtr.get();

    auto fe_volume = fe::calc_volumes<EDGE2, 1>(*mesh);
    auto grad_phi = fe::calc_grad_shape<EDGE2, 1>(*mesh, fe_volume);

    {
        auto bnd_facets = points_from_label(mesh->get_label("left"));
        auto vertices = mesh->get_cone_recursive_vertices(bnd_facets);
        vertices.sort_remove_dups();

        TestEssentialBoundary1D bnd(ref(*mesh), vertices);
        bnd.compute();
        EXPECT_EQ(bnd.vals[0], 2);
    }

    {
        IndexSet bnd_facets = points_from_label(mesh->get_label("right"));
        TestNaturalBoundary1D bnd(ref(*mesh), bnd_facets);
        bnd.compute();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 1);
    }
}
