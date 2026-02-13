// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/PetscObjectWrapper.h"
#include "godzilla/Types.h"
#include "godzilla/IndexSet.h"
#include "godzilla/String.h"
#include "godzilla/Range.h"
#include "petscdmlabel.h"

namespace godzilla {

class Label : public PetscObjectWrapper<DMLabel> {
public:
    Label();
    explicit Label(DMLabel label);

    /// Create the label
    ///
    /// @param comm The communicator
    /// @param name The label name
    void create(MPI_Comm comm, String name);

    /// Destroy the label
    void destroy();

    /// Destroys internal data structures in the underlying DMLabel
    void reset();

    /// Set the default value returned by `get_value()` if a point has not been explicitly given a
    /// value. When a label is created, it is initialized to -1.
    ///
    /// @param default_value The default value
    void set_default_value(Int default_value) const;

    /// Get the default value returned by `get_value()` if a point has not been explicitly given a
    /// value. When a label is created, it is initialized to -1.
    ///
    /// @return The default value
    Int get_default_value() const;

    /// Get the number of values that the label takes
    ///
    /// @return The number of values
    Int get_num_values() const;

    /// Return the value a label assigns to a point
    ///
    /// @param point The point
    /// @return The value associated with the point. If value is not set, return the label’s default
    /// value
    Int get_value(Int point) const;

    /// Get an IndexSet of all values that the Label takes
    ///
    /// @return IndexSet with all label values
    IndexSet get_value_index_set() const;

    /// Get all values that the `Label` takes
    ///
    /// @return `std::vector` with all label values
    std::vector<Int> get_values() const;

    void set_value(Int point, Int value);

    /// Get the size of a stratum
    ///
    /// @param value The stratum value
    /// @return Size of the stratum
    Int get_stratum_size(Int value) const;

    /// Get the largest and smallest point of a stratum
    ///
    /// @param value The stratum value
    /// @return Tuple with the smallest and largest point of the stratum
    std::tuple<Int, Int> get_stratum_bounds(Int value) const;

    /// Get the largest and smallest point of a stratum as a `Range`
    ///
    /// @param value The stratum value
    /// @return Range with the smallest and largest point of the stratum
    Range get_stratum_range(Int value) const;

    /// Get an IndexSet with the stratum points
    ///
    /// @param value The stratum value
    /// @return The stratum points
    IndexSet get_stratum(Int value) const;

    /// Set the stratum points using an IndexSet
    ///
    /// @param value The stratum value
    /// @param is The stratum points
    void set_stratum(Int value, const IndexSet & is) const;

    /// View the label
    ///
    /// @param viewer The PETSc viewer
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    // /// typecast operator so we can use our class directly with PETSc API
    // operator DMLabel() const;

    // /// Convert this object to a PETSc object so it can be passed directly into PETSc API
    // operator DMLabel *();
};

} // namespace godzilla
