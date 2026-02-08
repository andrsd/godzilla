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
        fe::EssentialBoundaryInfo<EDGE2, 1_D, 2>(mesh, vertices)
    {
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        EssentialBoundaryInfo::create();
        this->vals = Array1D<Int>(this->num_vertices());
    }

    void
    destroy() override
    {
        CALL_STACK_MSG();
        EssentialBoundaryInfo::destroy();
    }

    void
    compute()
    {
        for_each_vertex([&](Int idx, Int vtx) { this->vals[idx] = vtx; });
    }

    Array1D<Int> vals;
};

class TestNaturalBoundary1D : public fe::NaturalBoundaryInfo<EDGE2, 1_D, 2> {
public:
    TestNaturalBoundary1D(Ref<UnstructuredMesh> mesh,
                          Array1D<DenseMatrix<Real, 1, 2>> grad_phi,
                          const IndexSet & facets) :
        fe::NaturalBoundaryInfo<EDGE2, 1_D, 2>(mesh, grad_phi, facets)
    {
    }

    TestNaturalBoundary1D(Ref<UnstructuredMesh> mesh, const IndexSet & facets) :
        fe::NaturalBoundaryInfo<EDGE2, 1_D, 2>(mesh, facets)
    {
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        NaturalBoundaryInfo::create();
        this->vals = Array1D<Int>(this->num_facets());
    }

    void
    destroy() override
    {
        CALL_STACK_MSG();
        NaturalBoundaryInfo::destroy();
    }

    void
    compute()
    {
        for_each_facet([&](Int idx, Int facet) { this->vals[idx] = facet; });
    }

    Array1D<Int> vals;
};

} // namespace

TEST(FEBoundaryTest, test_1d)
{
    TestApp app;

    auto mesh_pars = TestMesh1D::parameters();
    mesh_pars.set<Ref<godzilla::App>>("app", ref(app));
    auto mesh_qtr = MeshFactory::create<TestMesh1D>(mesh_pars);
    auto mesh = mesh_qtr.get();

    auto fe_volume = fe::calc_volumes<EDGE2, 1>(*mesh);
    auto grad_phi = fe::calc_grad_shape<EDGE2, 1>(*mesh, fe_volume);

    {
        auto bnd_facets = points_from_label(mesh->get_label("left"));
        auto vertices = mesh->get_cone_recursive_vertices(bnd_facets);
        vertices.sort_remove_dups();

        TestEssentialBoundary1D bnd(ref(*mesh), vertices);
        bnd.create();
        bnd.compute();
        EXPECT_EQ(bnd.vals[0], 2);
        bnd.destroy();
    }

    {
        IndexSet bnd_facets = points_from_label(mesh->get_label("right"));
        TestNaturalBoundary1D bnd(ref(*mesh), bnd_facets);
        bnd.create();
        bnd.compute();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 1);
        bnd.destroy();
    }
}
