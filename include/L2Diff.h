#pragma once

#include "Postprocessor.h"
#include "FunctionInterface.h"

namespace godzilla {

/// Compute the L_2 difference between a function 'u' and an FEM interpolant solution 'u_h'
///
class L2Diff : public Postprocessor, public FunctionInterface {
public:
    L2Diff(const Parameters & params);

    void create() override;
    void compute() override;
    PetscReal get_value() override;

    /// Evaluate the function 'u'
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    void evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]);

protected:
    /// Computed L_2 error
    PetscReal l2_diff;

public:
    static Parameters parameters();
};

} // namespace godzilla
