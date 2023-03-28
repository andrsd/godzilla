#pragma once

#include "Problem.h"
#include "UnstructuredMesh.h"
#include "Vector.h"
#include "Matrix.h"
#include "FESolver.h"

namespace godzilla {

/// Base class for nonlinear solvers
class FENonlinearSolver : public FESolver {
public:
    FENonlinearSolver(const Problem * problem) : FESolver(problem) {}
    virtual ~FENonlinearSolver() = default;

    virtual void set_initial_guess() = 0;
    virtual void compute_residual(const Vector & x, Vector & f) = 0;
    virtual void compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp) = 0;

protected:
};

} // namespace godzilla
