#pragma once

#include "petscdmlabel.h"
#include "Types.h"
#include "IndexSet.h"

namespace godzilla {

class Label {
public:
    Label();
    explicit Label(DMLabel label);

    /// Create the label
    ///
    /// @param comm The communicator
    /// @param name The label name
    void create(MPI_Comm comm, const std::string & name);

    /// Destroy the label
    void destroy();

    /// Destroys internal data structures in the underlying DMLabel
    void reset();

    /// Test if the label is "null"
    ///
    /// @return `true` is the label is null, `false` otherwise
    bool is_null() const;

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
    IndexSet get_values() const;

    void set_value(Int point, Int value);

    /// Get the size of a stratum
    ///
    /// @param value The stratum value
    /// @return Size of the stratum
    Int get_stratum_size(Int value) const;

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

    /// typecast operator so we can use our class directly with PETSc API
    operator DMLabel() const;

private:
    DMLabel label;
};

} // namespace godzilla