#pragma once

#include "Postprocessor.h"
#include "FunctionInterface.h"

namespace godzilla {

class FEProblemInterface;
class ParsedFunction;

/// Computes the L_2 difference between a function `u` and an FEM interpolant solution `u_h`,
/// separated into field components.
///
class L2FieldDiff : public Postprocessor {
public:
    explicit L2FieldDiff(const Parameters & params);

    void create() override;
    void compute() override;
    PetscReal get_value() override;

protected:
    /// FE problem
    const FEProblemInterface * fepi;

    /// Parsed functions associated with each field
    std::map<std::string, ParsedFunction *> funcs;

    /// Computed L_2 errors
    std::vector<PetscReal> l2_diff;

public:
    static Parameters parameters();
};

} // namespace godzilla
