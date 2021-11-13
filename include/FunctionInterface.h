#pragma once

#include "petsc.h"
#include "muParser.h"
#include "InputParameters.h"

namespace godzilla {

/// Interface for parsed function
///
class FunctionInterface {
public:
    FunctionInterface(const InputParameters & params);

    /// Evaluate parsed function
    ///
    /// @return the result of evaluation
    /// @param idx The index of the parsed expression. Ranges from 0..`num_comps`
    /// @param dim Spatial dimension of the evaluated function
    /// @param time Time of the time-dependent function
    /// @param x Spatial coordinate where we evaluate the function (has size of `dim`)
    PetscReal evaluateFunction(unsigned int idx, PetscInt dim, PetscReal time, const PetscReal x[]);

protected:
    /// Array of function expressions - one per component
    const std::vector<std::string> & function_expr;
    /// Number of parsed function expressions
    const unsigned int num_comps;
    /// Parser objects - one per component
    std::vector<mu::Parser> parser;

public:
    static InputParameters validParams();
};

/// This is the API that we hand to PETSc for fields.
///
/// @param dim The spatial dimension
/// @param Nf The number of input fields
/// @param NfAux The number of input auxiliary fields
/// @param uOff The offset of each field in u[]
/// @param uOff_x The offset of each field in u_x[]
/// @param u The field values at this point in space
/// @param u_t The field time derivative at this point in space (or NULL)
/// @param u_x The field derivatives at this point in space
/// @param aOff The offset of each auxiliary field in u[]
/// @param aOff_x The offset of each auxiliary field in u_x[]
/// @param a The auxiliary field values at this point in space
/// @param a_t The auxiliary field time derivative at this point in space (or NULL)
/// @param a_x The auxiliary field derivatives at this point in space
/// @param t The current time
/// @param x The coordinates of this point
/// @param numConstants The number of constants
/// @param constants The value of each constant
/// @param f The value of the function at this point in space
typedef void PetscFieldFunc(PetscInt dim,
                            PetscInt Nf,
                            PetscInt NfAux,
                            const PetscInt uOff[],
                            const PetscInt uOff_x[],
                            const PetscScalar u[],
                            const PetscScalar u_t[],
                            const PetscScalar u_x[],
                            const PetscInt aOff[],
                            const PetscInt aOff_x[],
                            const PetscScalar a[],
                            const PetscScalar a_t[],
                            const PetscScalar a_x[],
                            PetscReal t,
                            const PetscReal x[],
                            PetscInt numConstants,
                            const PetscScalar constants[],
                            PetscScalar f[]);

typedef PetscErrorCode PetscFunc(PetscInt dim,
                                 PetscReal time,
                                 const PetscReal x[],
                                 PetscInt Nc,
                                 PetscScalar u[],
                                 void * ctx);

} // namespace godzilla
