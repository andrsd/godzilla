#include "Godzilla.h"
#include "ExodusIIMesh.h"
#include "CallStack.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Error.h"
#include "Edge.h"
#include "Tri.h"
#include "Quad.h"
#include "Tetra.h"
#include "Hex.h"
#include "Utils.h"
#include <exodusII.h>

// TODO: error checking on all exodusII functions

namespace godzilla {

enum EElemType { INVALID, EDGE, TRIANGLE, QUAD, HEX, TETRA, PRISM, PYRAMID };

registerObject(ExodusIIMesh);

InputParameters
ExodusIIMesh::validParams()
{
    InputParameters params = Mesh::validParams();
    params.add_required_param<std::string>("file", "The name of the ExodusII file.");
    return params;
}

ExodusIIMesh::ExodusIIMesh(const InputParameters & params) :
    Mesh(params),
    file_name(get_param<std::string>("file")),
    exoid(-1)
{
    _F_;
}

const std::string &
ExodusIIMesh::get_file_name() const
{
    _F_;
    return this->file_name;
}

void
ExodusIIMesh::create()
{
    _F_;
    if (utils::path_exists(this->file_name)) {
        load(this->file_name);
        Mesh::create();
    }
    else
        log_error("Unable to open '",
                  this->file_name,
                  "' for reading. Make sure it exists and you have read permissions.");
}

void
ExodusIIMesh::load_coordinates(int n_nodes)
{
    _F_;

    int err;
    double * x = new double[n_nodes];
    MEM_CHECK(x);

    double * y = nullptr;
    if (this->dim > 1) {
        y = new double[n_nodes];
        MEM_CHECK(y);
    }

    double * z = nullptr;
    if (this->dim > 2) {
        z = new double[n_nodes];
        MEM_CHECK(z);
    }

    if (this->dim == 1) {
        err = ex_get_coord(this->exoid, x, NULL, NULL);
        for (int i = 0; i < n_nodes; i++)
            set_vertex(i, new Vertex1D(x[i]));
    }
    else if (this->dim == 2) {
        err = ex_get_coord(this->exoid, x, y, NULL);
        for (int i = 0; i < n_nodes; i++)
            set_vertex(i, new Vertex2D(x[i], y[i]));
    }
    else if (this->dim == 3) {
        err = ex_get_coord(this->exoid, x, y, z);
        for (int i = 0; i < n_nodes; i++)
            set_vertex(i, new Vertex3D(x[i], y[i], z[i]));
    }

    delete[] x;
    delete[] y;
    delete[] z;
}

void
ExodusIIMesh::load_block(int blk_id, int & elem_id)
{
    _F_;
    int err;
    // get block info
    char elem_type_str[MAX_STR_LENGTH + 1];
    int n_elems_in_blk, n_elem_nodes, n_attrs;
    err = ex_get_block(this->exoid,
                       EX_ELEM_BLOCK,
                       blk_id,
                       elem_type_str,
                       &n_elems_in_blk,
                       &n_elem_nodes,
                       NULL,
                       NULL,
                       &n_attrs);

    EElemType elem_type = INVALID;
    if (strncmp(elem_type_str, "BAR", 3) == 0)
        elem_type = EDGE;
    else if (strncmp(elem_type_str, "TRI", 3) == 0)
        elem_type = TRIANGLE;
    else if (strncmp(elem_type_str, "QUAD", 3) == 0)
        elem_type = QUAD;
    else if (strncmp(elem_type_str, "HEX", 3) == 0)
        elem_type = HEX;
    else if (strncmp(elem_type_str, "TET", 3) == 0)
        elem_type = TETRA;
    else
        error("Unsupported element type '", elem_type_str, "'.");

    // read connectivity array
    int * connect = new int[n_elem_nodes * n_elems_in_blk];
    MEM_CHECK(connect);
    err = ex_get_conn(this->exoid, EX_ELEM_BLOCK, blk_id, connect, 0, 0);

    // add elements into mesh
    int ic = 0;
    for (int j = 0; j < n_elems_in_blk; j++) {
        Index vtcs[n_elem_nodes];
        for (int k = 0; k < n_elem_nodes; k++, ic++)
            vtcs[k] = connect[ic] - 1;

        Element * elem = nullptr;
        switch (elem_type) {
        case EDGE:
            elem = new Edge(vtcs);
            break;
        case TRIANGLE:
            elem = new Tri(vtcs);
            break;
        case QUAD:
            elem = new Quad(vtcs);
            break;
        case TETRA:
            elem = new Tetra(vtcs);
            break;
        case HEX:
            elem = new Hex(vtcs);
            break;
        default:
            break;
        }

        elem->set_marker((uint) blk_id);
        set_element(elem_id, elem);
        elem_id++;
    }
    delete[] connect;
}

void
ExodusIIMesh::load(const std::string & file_name)
{
    _F_;

    int err;
    // use float or double
    int cpu_ws = sizeof(double);
    // store variables as doubles
    int io_ws = 8;
    float version;
    this->exoid = ex_open(file_name.c_str(), EX_READ, &cpu_ws, &io_ws, &version);

    // read initialization parameters
    int n_dims, n_nodes, n_elems, n_eblocks, n_nodesets, n_sidesets;
    char title[MAX_LINE_LENGTH + 1];
    err = ex_get_init(this->exoid,
                      title,
                      &n_dims,
                      &n_nodes,
                      &n_elems,
                      &n_eblocks,
                      &n_nodesets,
                      &n_sidesets);
    this->dim = n_dims;
    load_coordinates(n_nodes);

    int elem_id = 0;
    // load elements block by block
    int * eid_blocks = new int[n_eblocks];
    MEM_CHECK(eid_blocks);
    err = ex_get_ids(this->exoid, EX_ELEM_BLOCK, eid_blocks);
    for (int i = 0; i < n_eblocks; i++) {
        int id = eid_blocks[i];
        load_block(id, elem_id);
    }
    delete[] eid_blocks;

    err = ex_close(this->exoid);

    set_dimension(this->dim);
}

} // namespace godzilla
