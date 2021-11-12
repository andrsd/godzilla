#pragma once

#include <string>
#include "petsc.h"

namespace godzilla {

class InitialCondition;
class BoundaryCondition;

/// Interface for FE problems
///
/// Any problem using PetscFE should inherit from this for unified API
class FEProblemInterface {
public:
    /// Get field name
    ///
    /// @param fid Field Id
    virtual const std::string & getFieldName(PetscInt fid) = 0;

    /// Adds a volumetric field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual PetscInt addField(const std::string & name, PetscInt nc, PetscInt k) = 0;

    /// Add initial condition
    ///
    /// @param ic Initial condition object to add
    virtual void addInitialCondition(const InitialCondition * ic) = 0;

    /// Add essental boundary condition
    ///
    /// @param bc Boundary condition object to add
    virtual void addBoundaryCondition(const BoundaryCondition * bc) = 0;
};

} // namespace godzilla
