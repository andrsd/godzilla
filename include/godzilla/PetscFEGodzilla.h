// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petsc.h"

namespace godzilla {
namespace internal {

/// Create PetscFE object for Lagrange FE
///
/// @param comm MPI communicator
/// @param dim Spatial dimension
/// @param Nc Number of components
/// @param is_simplex `true` is simplex cells
/// @param k Polynomial order
/// @param qorder Quadrature order
/// @param fem Created PetscFE object
/// @return PETSC error code
PetscErrorCode create_lagrange_petscfe(MPI_Comm comm,
                                       Int dim,
                                       Int Nc,
                                       PetscBool is_simplex,
                                       Int k,
                                       Int qorder,
                                       PetscFE * fem);

} // namespace internal
} // namespace godzilla
