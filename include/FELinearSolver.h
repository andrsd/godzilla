#pragma once

#include "Problem.h"
#include "UnstructuredMesh.h"
#include "Vector.h"
#include "Matrix.h"
#include "FESolver.h"

namespace godzilla {

/// Base class for linear solvers
class FELinearSolver : public FESolver {
public:
    FELinearSolver(const Problem * problem) : FESolver(problem) {}
    virtual ~FELinearSolver() = default;

    virtual void compute_rhs(Vector & b) = 0;
    virtual void compute_operators(Matrix & A, Matrix & B) = 0;

protected:
};

} // namespace godzilla
