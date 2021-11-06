#pragma once

#include "Object.h"
#include "GPrintInterface.h"
#include "petscdm.h"

namespace godzilla {

class GGrid;

/// Problem
///
class GProblem : public Object,
                 public GPrintInterface
{
public:
    GProblem(const InputParameters & parameters);

    /// Build the problem to solve
    virtual void create() = 0;
    /// Solve the problem
    virtual void solve() = 0;
    /// true if solve converged, otherwise false
    virtual bool converged() = 0;
    /// provide DM for the underlying KSP object
    virtual const DM & getDM() const = 0;
    /// Return solution vector
    virtual const Vec & getSolutionVector() const = 0;

protected:
    /// Grid
    GGrid & grid;

public:
    static InputParameters validParams();
};

}
