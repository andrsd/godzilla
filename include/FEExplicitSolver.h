#pragma once

#include "Problem.h"
#include "UnstructuredMesh.h"
#include "Vector.h"
#include "Matrix.h"
#include "FESolver.h"

namespace godzilla {

/// Base class for explicit solvers
class FEExplicitSolver : public FESolver {
public:
    FEExplicitSolver(const Problem * problem) : FESolver(problem) {}
    virtual ~FEExplicitSolver() = default;

    virtual void set_initial_guess() = 0;
    virtual void compute_rhs(Real time, const Vector & x, Vector & f) = 0;
    virtual void compute_matrix(Matrix & M) = 0;
    virtual void post_step() = 0;

protected:
};

} // namespace godzilla
