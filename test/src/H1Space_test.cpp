#include "GodzillaApp_test.h"
#include "Common.h"
#include "LineMesh.h"
#include "Edge.h"
#include "H1LobattoShapesetEdge.h"
#include "H1Space.h"
#include "EssentialBC.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(H1SpaceTest, first_order_1d)
{
    TestApp app;

    InputParameters params = LineMesh::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<Real>("xmax") = 2.;
    params.set<uint>("nx") = 2;
    LineMesh mesh(params);

    mesh.create();

    H1LobattoShapesetEdge ss;

    H1Space sp(&mesh, &ss);
    sp.set_uniform_order(2);

    sp.assign_dofs();
    EXPECT_EQ(sp.get_dof_count(), 5);

    AssemblyList al;
    const Element * e = mesh.get_element(0);
    sp.get_element_assembly_list(e, &al);
    EXPECT_EQ(al.cnt, 2);
}

TEST(H1SpaceTest, n_dofs)
{
    class MockH1Space : public H1Space {
    public:
        MockH1Space(Mesh * mesh, Shapeset * ss) : H1Space(mesh, ss) {}
        virtual uint
        get_vertex_ndofs()
        {
            return H1Space::get_vertex_ndofs();
        }
        virtual uint
        get_edge_ndofs(uint order)
        {
            return H1Space::get_edge_ndofs(order);
        }
        virtual uint
        get_face_ndofs(uint order)
        {
            return H1Space::get_face_ndofs(order);
        }
        virtual uint
        get_element_ndofs(uint order)
        {
            return H1Space::get_element_ndofs(order);
        }
    };

    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Real>("xmax") = 2.;
    mesh_pars.set<uint>("nx") = 2;
    LineMesh mesh(mesh_pars);
    mesh.create();

    H1LobattoShapesetEdge ss;

    MockH1Space sp(&mesh, &ss);
    EXPECT_EQ(sp.get_vertex_ndofs(), 1);
    EXPECT_EQ(sp.get_edge_ndofs(3), 2);
    EXPECT_EQ(sp.get_face_ndofs(3), 0);
    EXPECT_EQ(sp.get_element_ndofs(3), 2);
}

TEST(H1SpaceTest, bc_proj)
{
    class MockH1Space : public H1Space {
    public:
        MockH1Space(Mesh * mesh, Shapeset * ss) : H1Space(mesh, ss) {}

        virtual void
        calc_vertex_boundary_projection(Index vtx_idx)
        {
            H1Space::calc_vertex_boundary_projection(vtx_idx);
        }
        virtual void
        calc_edge_boundary_projection(const Element * elem, uint iedge)
        {
            H1Space::calc_edge_boundary_projection(elem, iedge);
        }
        virtual void
        calc_face_boundary_projection(const Element * elem, uint iface)
        {
            H1Space::calc_face_boundary_projection(elem, iface);
        }
        uint
        get_vertex_data_n(Index idx)
        {
            return this->vertex_data[idx]->n;
        }
        Scalar
        get_vertex_data_bc_proj(Index idx)
        {
            return this->vertex_data[idx]->bc_proj;
        }
    };

    class MockDirichletBC : public EssentialBC {
    public:
        explicit MockDirichletBC(const InputParameters & params) : EssentialBC(params) {}
        virtual Scalar
        evaluate(Real time, Real x, Real y, Real z)
        {
            return 1234.;
        }
    };

    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Real>("xmax") = 2.;
    mesh_pars.set<uint>("nx") = 2;
    LineMesh mesh(mesh_pars);
    mesh.create();

    InputParameters bc_pars = EssentialBC::valid_params();
    bc_pars.set<const App *>("_app") = &app;
    bc_pars.set<std::string>("boundary") = "left";
    MockDirichletBC bc(bc_pars);
    bc.create();

    H1LobattoShapesetEdge ss;

    MockH1Space sp(&mesh, &ss);
    sp.add_boundary_condition(&bc);
    sp.set_uniform_order(1);

    sp.assign_dofs();

    const Element * e = mesh.get_element(0);
    sp.calc_vertex_boundary_projection(2);
    EXPECT_EQ(sp.get_vertex_data_n(2), 1);
    EXPECT_EQ(sp.get_vertex_data_bc_proj(2), 1234.);

    EXPECT_DEATH(sp.calc_edge_boundary_projection(e, 0), "Not implemented");
    EXPECT_DEATH(sp.calc_face_boundary_projection(e, 0), "Not implemented");
}
