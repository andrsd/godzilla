#include "ExodusIO.h"
#include "CallStack.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Error.h"
#include "Edge.h"
#include "Tri.h"
#include "Tetra.h"
#include "Hex.h"
#include <exodusII.h>

// TODO: error checking on all exodusII functions

namespace godzilla {

enum EElemType { INVALID, EDGE, TRIANGLE, QUAD, HEX, TETRA, PRISM, PYRAMID };

static void
loadCoordinates(int exoid, Mesh & mesh, int dim, int n_nodes)
{
    _F_;

    int err;
    double * x = new double[n_nodes];
    MEM_CHECK(x);

    double * y = nullptr;
    if (dim > 1) {
        y = new double[n_nodes];
        MEM_CHECK(y);
    }

    double * z = nullptr;
    if (dim > 2) {
        z = new double[n_nodes];
        MEM_CHECK(z);
    }

    if (dim == 1) {
        err = ex_get_coord(exoid, x, NULL, NULL);
        for (int i = 0; i < n_nodes; i++)
            mesh.setVertex(i + 1, new Vertex1D(x[i]));
    }
    else if (dim == 2) {
        err = ex_get_coord(exoid, x, y, NULL);
        for (int i = 0; i < n_nodes; i++)
            mesh.setVertex(i + 1, new Vertex2D(x[i], y[i]));
    }
    else if (dim == 3) {
        err = ex_get_coord(exoid, x, y, z);
        for (int i = 0; i < n_nodes; i++)
            mesh.setVertex(i + 1, new Vertex3D(x[i], y[i], z[i]));
    }

    delete[] x;
    delete[] y;
    delete[] z;
}

static void
loadBlock(int exoid, Mesh & mesh, int blk_id, int & elem_id)
{
    _F_;

    int err;
    // get block info
    char elem_type_str[MAX_STR_LENGTH + 1];
    int n_elems_in_blk, n_elem_nodes, n_attrs;
    err = ex_get_block(exoid,
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
    else if (strncmp(elem_type_str, "HEX", 3) == 0)
        elem_type = HEX;
    else if (strncmp(elem_type_str, "TET", 3) == 0)
        elem_type = TETRA;
    else
        error("Unsupported element type '", elem_type_str, "'.");

    // read connectivity array
    int * connect = new int[n_elem_nodes * n_elems_in_blk];
    MEM_CHECK(connect);
    err = ex_get_conn(exoid, EX_ELEM_BLOCK, blk_id, connect, 0, 0);

    // add elements into mesh
    int ic = 0;
    for (int j = 0; j < n_elems_in_blk; j++) {
        Index vtcs[n_elem_nodes];
        for (int k = 0; k < n_elem_nodes; k++, ic++)
            vtcs[k] = connect[ic];

        Element * elem = nullptr;
        switch (elem_type) {
        case EDGE:
            elem = new Edge(vtcs);
            break;
        case TRIANGLE:
            elem = new Tri(vtcs);
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

        elem->setMarker((uint) blk_id);
        mesh.setElement(elem_id, elem);
        elem_id++;
    }
    delete[] connect;
}

Mesh
ExodusIO::load(const std::string & file_name)
{
    _F_;

    Mesh mesh;
    int err;
    // use float or double
    int cpu_ws = sizeof(double);
    // store variables as doubles
    int io_ws = 8;
    float version;
    int exoid = ex_open(file_name.c_str(), EX_READ, &cpu_ws, &io_ws, &version);

    // read initialization parameters
    int n_dims, n_nodes, n_elems, n_eblocks, n_nodesets, n_sidesets;
    char title[MAX_LINE_LENGTH + 1];
    err = ex_get_init(exoid,
                      title,
                      &n_dims,
                      &n_nodes,
                      &n_elems,
                      &n_eblocks,
                      &n_nodesets,
                      &n_sidesets);

    loadCoordinates(exoid, mesh, n_dims, n_nodes);

    int elem_id = 0;
    // load elements block by block
    int * eid_blocks = new int[n_eblocks];
    MEM_CHECK(eid_blocks);
    err = ex_get_ids(exoid, EX_ELEM_BLOCK, eid_blocks);
    for (int i = 0; i < n_eblocks; i++) {
        int id = eid_blocks[i];
        loadBlock(exoid, mesh, id, elem_id);
    }
    delete[] eid_blocks;

    err = ex_close(exoid);

    return mesh;
}

} // namespace godzilla
