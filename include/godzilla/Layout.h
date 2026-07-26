// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Span.h"
#include "godzilla/Range.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include "petscis.h"

namespace mpi = mpicpp_lite;

namespace godzilla {

class Layout {
public:
    Layout();
    Layout(PetscLayout lo);

    /// Create a layout object
    void create(mpi::Communicator comm);

    /// Creates a new PetscLayout with the same information as a given one
    Layout duplicate() const;

    /// Find the owning MPI process for a global index
    ///
    /// @param idx Global index to find the owner of
    /// @return The owning rank
    int find_owner(Int idx) const;

    /// Find the owning MPI process and the local index on that process for a global index
    ///
    /// @param idx Global index to find the owner of
    /// @return (rank, local index)
    ///     `rank` - the owning rank,
    ///     `local_index` - local index used by the owner for `idx`
    std::tuple<int, Int> find_owner_index(Int idx) const;

    /// Get the block size
    Int get_block_size() const;

    /// Get the local size
    Int get_local_size() const;

    /// Get the range of values owned by this process
    Range get_range() const;

    /// Get the size
    Int get_size() const;

    /// Set the block size for this `Layout` object
    void set_block_size(Int size);

    /// Set the local size for this `Layout` object.
    void set_local_size(Int size);

    /// Set the global size for this `Layout` object.
    void set_size(Int size);

    /// Given a map where you have set either the global or local size sets up the map so that it
    /// may be used.
    void set_up();

    /// Compare this layout with another one
    ///
    /// @return `true` if layouts are congruent, `false` otherwise
    bool compare(const Layout & other) const;

    bool
    is_null() const
    {
        CALL_STACK_MSG();
        return this->obj == nullptr;
    }

    PetscLayout
    get() const
    {
        return this->obj;
    }

private:
    PetscLayout obj;

public:
    /// Allocates Layout object and sets the layout sizes, and sets the layout up.
    ///
    /// @param comm The MPI communicator
    /// @param n The local size (or `PETSC_DECIDE`)
    /// @param N The global size (or `PETSC_DECIDE`)
    /// @param bs The block size
    static Layout create_from_sizes(mpi::Communicator comm, Int n, Int N, Int bs = 1);

    /// Creates a new Layout with the given ownership ranges and sets it up.
    ///
    /// @param comm The MPI communicator
    /// @param range The array of ownership ranges for each rank with length commsize+1
    static Layout create_from_ranges(mpi::Communicator comm, Span<const Int> range, Int bs);
};

} // namespace godzilla
