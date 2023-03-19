#pragma once

#include "Types.h"
#include "Array1D.h"
#include "DenseVector.h"
#include "Problem.h"
#include "UnstructuredMesh.h"

namespace godzilla {

/// Base class for solvers
class FESolver {
public:
    FESolver(const Problem * prob) :
        problem(prob),
        mesh(dynamic_cast<const UnstructuredMesh *>(problem->get_mesh())),
        n_nodes(mesh->get_num_vertices()),
        n_elems(mesh->get_num_cells())
    {
    }
    virtual ~FESolver() = default;

    virtual void create() = 0;
    virtual void destroy() = 0;

protected:
    /// Problem associated with this solver
    const Problem * problem;
    /// Mesh associated with this solver
    const UnstructuredMesh * mesh;
    /// Local number of elements
    Int n_elems;
    /// Local number of nodes
    Int n_nodes;
};

} // namespace godzilla
