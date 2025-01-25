// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petscsystypes.h"

namespace godzilla {

enum CopyMode {
    /// The array values are copied into new space, the user is free to reuse or delete the passed
    /// in array
    COPY_VALUES = PETSC_COPY_VALUES,
    ///
    OWN_POINTER = PETSC_OWN_POINTER,
    /// The array values are *not* copied, the object uses the array but does *not* take ownership
    /// of the array. The user cannot use the array but the user must delete the array after the
    /// object is destroyed.
    USE_POINTER = PETSC_USE_POINTER,
};

enum class FileAccess {
    /// Open the file for reading
    READ,
    /// Open the file for writing
    WRITE,
    /// Create a new file
    CREATE
};

} // namespace godzilla
