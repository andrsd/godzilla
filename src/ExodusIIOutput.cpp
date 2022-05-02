#include "Godzilla.h"
#include "GodzillaConfig.h"
#include "ExodusIIOutput.h"
#include "Problem.h"
#include "FELinearProblem.h"
#include "Solution1D.h"
#include "QuadratureOutput1D.h"
#include "exodusII.h"

namespace godzilla {

const char *
ExodusIIOutput::get_elem_type(DMPolytopeType elem_type)
{
    switch (elem_type) {
    case DM_POLYTOPE_SEGMENT:
        return "BAR2";
    case DM_POLYTOPE_TRIANGLE:
        return "TRI2";
    case DM_POLYTOPE_QUADRILATERAL:
        return "QUAD4";
    case DM_POLYTOPE_TETRAHEDRON:
        return "TET4";
    case DM_POLYTOPE_HEXAHEDRON:
        return "HEX8";
    default:
        error("Unsupported type.");
    }
}

registerObject(ExodusIIOutput);

InputParameters
ExodusIIOutput::valid_params()
{
    InputParameters params = FileOutput::valid_params();
    return params;
}

ExodusIIOutput::ExodusIIOutput(const InputParameters & params) : FileOutput(params)
{
    _F_;
}

std::string
ExodusIIOutput::get_file_ext() const
{
    return std::string("exo");
}

void
ExodusIIOutput::create()
{
    _F_;
}

void
ExodusIIOutput::check()
{
    _F_;
}

void
ExodusIIOutput::output()
{
    _F_;
    set_file_name();

    int cpu_word_size = sizeof(Real);
    int io_word_size = sizeof(Real);
    int exoid = ex_create(get_file_name().c_str(), EX_CLOBBER, &cpu_word_size, &io_word_size);
    if (exoid != -1) {
        output_file(exoid);
        ex_close(exoid);
    }
    else
        error("Could not open file '", get_file_name(), "' for writing.");
}

void
ExodusIIOutput::output_file(int exoid)
{
    _F_;

    const FELinearProblem * fep = dynamic_cast<const FELinearProblem *>(this->problem);
    const Mesh * mesh = fep->get_mesh();
    int n_dim = mesh->get_dimension();
    int n_nodes = mesh->get_num_vertices();
    int n_elem = mesh->get_num_elements();
    // Only homogeneous meshes with one subdomain now
    int n_elem_blk = 1;
    int n_node_sets = 0;
    int n_side_sets = 0;
    ex_put_init(exoid, "", n_dim, n_nodes, n_elem, n_elem_blk, n_node_sets, n_side_sets);

    int n_info = 1;
    const char * info[] = { "Created by godzilla " GODZILLA_VERSION };
    ex_put_info(exoid, n_info, (char **) info);

    output_coords(exoid);
    output_elements(exoid);
    output_variables(exoid);
}

void
ExodusIIOutput::output_coords(int exoid)
{
    _F_;
    const FELinearProblem * fep = dynamic_cast<const FELinearProblem *>(this->problem);
    const Mesh * mesh = fep->get_mesh();
    int n_dim = mesh->get_dimension();
    int n_nodes = mesh->get_num_vertices();

    Real * x = new Real[n_nodes];
    MEM_CHECK(x);
    Real * y = nullptr;
    if (n_dim >= 2) {
        y = new Real[n_nodes];
        MEM_CHECK(y);
    }
    Real * z = nullptr;
    if (n_dim >= 3) {
        z = new Real[n_nodes];
        MEM_CHECK(z);
    }
    uint i = 0;
    for (auto & v : mesh->get_vertices()) {
        const Vertex1D * v1d = static_cast<const Vertex1D *>(v);
        assert(v1d != nullptr);
        x[i] = v1d->x;
        // TODO: 2D vertices
        // TODO: 3D vertices
        ++i;
    }
    ex_put_coord(exoid, x, y, z);

    const char * coord_names[3] = { "x", "y", "z" };
    ex_put_coord_names(exoid, (char **) coord_names);

    delete[] x;
    delete[] y;
    delete[] z;
}

void
ExodusIIOutput::output_elements(int exoid)
{
    _F_;
    const FELinearProblem * fep = dynamic_cast<const FELinearProblem *>(this->problem);
    const Mesh * mesh = fep->get_mesh();
    int n_elems = mesh->get_num_elements();

    int n_elems_in_block = mesh->get_num_elements();

    Index first = mesh->get_elements().first();
    const Element * e = mesh->get_elements()[first];
    int n_nodes_per_elem = e->get_num_vertices();
    const char * cell_type = get_elem_type(e->get_potytope_type());

    // FIXME: this should match the block `marker` from the mesh
    ex_entity_id blk_id = 0;

    ex_put_block(exoid,
                 EX_ELEM_BLOCK,
                 blk_id,
                 cell_type,
                 n_elems_in_block,
                 n_nodes_per_elem,
                 0,
                 0,
                 0);

    // TODO: store block name
    // char * block_names[] = { "block_1" };
    // ex_put_names(exoid, EX_ELEM_BLOCK, block_names);

    int * connect = new int[n_elems_in_block * n_nodes_per_elem];
    MEM_CHECK(connect);
    int j = 0;
    for (auto & e : mesh->get_elements()) {
        const PetscInt * vtcs = e->get_vertices();
        for (uint i = 0; i < n_nodes_per_elem; i++)
            connect[j++] = vtcs[i] - n_elems + 1;
    }
    ex_put_conn(exoid, EX_ELEM_BLOCK, blk_id, connect, nullptr, nullptr);

    delete[] connect;
}

void
ExodusIIOutput::output_variables(int exoid)
{
    _F_;
    output_nodal_variables(exoid);
}

void
ExodusIIOutput::output_nodal_variables(int exoid)
{
    _F_;

    const FELinearProblem * fep = dynamic_cast<const FELinearProblem *>(this->problem);
    assert(fep != nullptr);
    const Mesh * mesh = fep->get_mesh();

    int n_nodes = mesh->get_num_vertices();
    int n_elems = mesh->get_num_elements();

    // TODO: loop over variables
    int n_nodal_vars = 1;
    const std::string & var_name = "u";

    ex_put_variable_param(exoid, EX_NODAL, n_nodal_vars);
    const char * var_names[] = { var_name.c_str() };
    ex_put_variable_names(exoid, EX_NODAL, n_nodal_vars, (char **) var_names);

    Solution1D * sln = fep->get_solution(var_name);
    Real * point_data = new Real[n_nodes];
    MEM_CHECK(point_data);
    for (const auto & e : mesh->get_elements()) {
        const Element1D * e1d = dynamic_cast<const Element1D *>(e);
        assert(e1d != nullptr);
        sln->set_active_element(e1d);

        uint qorder = sln->get_order();
        QuadratureOutput1D & quad = QuadratureOutput1D::get();
        uint np = quad.get_num_points(qorder);
        QPoint1D * pts = quad.get_points(qorder);

        const PetscInt * vtcs = e->get_vertices();
        sln->precalculate(np, pts, Solution1D::FN_VAL);
        Scalar * vals = sln->get_fn_values();
        for (uint i = 0; i < e->get_num_vertices(); i++)
            point_data[vtcs[i] - n_elems] = vals[i];
    }

    Real time = 1.;
    // variable index
    int k = 1;
    ex_put_var(exoid, time, EX_NODAL, k, 1, n_nodes, point_data);

    delete[] point_data;
}

} // namespace godzilla
