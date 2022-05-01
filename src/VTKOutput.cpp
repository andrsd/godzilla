#include "Godzilla.h"
#include "VTKOutput.h"
#include "Problem.h"
#include "FELinearProblem.h"
#include "Solution1D.h"
#include "QuadratureOutput1D.h"

namespace godzilla {

registerObject(VTKOutput);

static const int MAX_PATH = 1024;

enum VtkCellType {
    // 1D
    VTK_EDGE = 3,
    // 2D
    VTK_TRIANGLE = 5,
    VTK_QUAD = 9,
    // 3D
    VTK_TETRA = 10,
    VTK_HEXAHEDRON = 12,
    VTK_WEDGE = 13,
    VTK_PYRAMID = 14
};

InputParameters
VTKOutput::valid_params()
{
    InputParameters params = FileOutput::valid_params();
    return params;
}

VTKOutput::VTKOutput(const InputParameters & params) : FileOutput(params)
{
    _F_;
}

std::string
VTKOutput::get_file_ext() const
{
    return std::string("vtk");
}

void
VTKOutput::create()
{
    _F_;
}

void
VTKOutput::check()
{
    _F_;
}

void
VTKOutput::output()
{
    _F_;
    set_file_name();
    FILE * f = fopen(get_file_name().c_str(), "w");
    if (f != NULL) {
        output_vtk_file(f);
        fclose(f);
    }
    else
        error("Could not open file '", get_file_name(), "' for writing.");
}

void
VTKOutput::output_vtk_file(FILE * file)
{
    _F_;
    output_header(file);
    output_geometry(file);
    output_variables(file);
}

void
VTKOutput::output_header(FILE * file)
{
    _F_;
    fprintf(file, "# vtk DataFile Version 2.0\n");
    fprintf(file, "\n");
    fprintf(file, "ASCII\n");
    fprintf(file, "\n");
}

void
VTKOutput::output_geometry(FILE * file)
{
    _F_;
    const FELinearProblem * fep = dynamic_cast<const FELinearProblem *>(this->problem);
    assert(fep != nullptr);
    const Mesh * mesh = fep->get_mesh();

    fprintf(file, "DATASET UNSTRUCTURED_GRID\n");
    fprintf(file, "POINTS %ld %s\n", mesh->get_vertices().count(), "float");
    for (const auto & v : mesh->get_vertices()) {
        const Vertex1D * v1d = static_cast<const Vertex1D *>(v);
        fprintf(file, "%e %e %e\n", v1d->x, 0., 0.);
    }
    fprintf(file, "\n");

    uint n_cells = mesh->get_num_elements();
    uint cell_points = 0;
    for (const auto & e : mesh->get_elements())
        cell_points += e->get_num_vertices() + 1;
    fprintf(file, "CELLS %u %d\n", n_cells, cell_points);
    for (const auto & e : mesh->get_elements()) {
        uint n_vtcs = e->get_num_vertices();
        fprintf(file, "%d", n_vtcs);
        const PetscInt * vtcs = e->get_vertices();
        for (uint j = 0; j < n_vtcs; j++)
            // we get this from PETSc DM
            fprintf(file, " %d", vtcs[j] - n_cells);
        fprintf(file, "\n");
    }
    fprintf(file, "\n");

    fprintf(file, "CELL_TYPES %u\n", n_cells);
    for (const auto & e : mesh->get_elements()) {
        int vtk_type = 0;
        switch (e->get_potytope_type()) {
        case DM_POLYTOPE_SEGMENT:
            vtk_type = VTK_EDGE;
            break;
        case DM_POLYTOPE_TRIANGLE:
            vtk_type = VTK_TRIANGLE;
            break;
        case DM_POLYTOPE_QUADRILATERAL:
            vtk_type = VTK_QUAD;
            break;
        case DM_POLYTOPE_TETRAHEDRON:
            vtk_type = VTK_TETRA;
            break;
        case DM_POLYTOPE_HEXAHEDRON:
            vtk_type = VTK_HEXAHEDRON;
            break;
        default:
            error("Polytope type ", e->get_potytope_type(), " not supported.");
            break;
        }
        fprintf(file, "%d\n", vtk_type);
    }
    fprintf(file, "\n");
}

void
VTKOutput::output_variables(FILE * file)
{
    _F_;

    const FELinearProblem * fep = dynamic_cast<const FELinearProblem *>(this->problem);
    assert(fep != nullptr);
    const Mesh * mesh = fep->get_mesh();

    fprintf(file, "POINT_DATA %ld\n", mesh->get_vertices().count());

    // TODO: loop over variables
    const std::string & var_name = "u";
    Solution1D * sln = fep->get_solution(var_name);
    Array<Scalar> point_data;
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
            point_data[vtcs[i]] = vals[i];
    }
    fprintf(file, "SCALARS %s float 1\n", var_name.c_str());
    fprintf(file, "LOOKUP_TABLE default\n");
    for (const auto & v : mesh->get_vertices()) {
        fprintf(file, "%e\n", point_data[v->id]);
    }
    fprintf(file, "\n");
}

} // namespace godzilla
