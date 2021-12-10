#pragma once

#include "NonlinearProblem.h"
#include "FEProblemInterface.h"

namespace godzilla {

/// PETSc non-linear problem that arises from a finite element discretization
/// using the PetscFE system
///
class FENonlinearProblem : public NonlinearProblem, public FEProblemInterface {
public:
    FENonlinearProblem(const InputParameters & parameters);
    virtual ~FENonlinearProblem();

    virtual void create() override;

protected:
    virtual void init() override;
    virtual void setUpCallbacks() override;
    virtual void setUpInitialGuess() override;
    virtual PetscErrorCode computeResidualCallback(Vec x, Vec f) override;
    virtual PetscErrorCode computeJacobianCallback(Vec x, Mat J, Mat Jp) override;

public:
    static InputParameters validParams();
};

} // namespace godzilla
