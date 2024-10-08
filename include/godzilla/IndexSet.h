// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Enums.h"
#include "godzilla/Types.h"
#include "petscis.h"
#include "petscviewer.h"
#include <vector>

namespace godzilla {

/// `IndexSet`s are objects used to do efficient indexing into other data structures such as
/// `Vector` and `Matrix`
class IndexSet {
public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = Int;
        using difference_type = Int;
        using pointer = Int *;
        using reference = Int &;

        explicit Iterator(IndexSet & is, Int idx);
        ~Iterator() = default;

        const value_type & operator*() const;

        /// Prefix increment
        Iterator & operator++();

        /// Postfix increment
        Iterator operator++(int);

        friend bool operator==(const Iterator & a, const Iterator & b);

        friend bool operator!=(const Iterator & a, const Iterator & b);

    private:
        /// IndexSet to iterate over
        IndexSet & is;
        /// Index pointing to the `is`
        Int idx;
    };

    IndexSet();
    explicit IndexSet(IS is);
    ~IndexSet() = default;

    /// Replace the content of this index set with the content of another index set
    ///
    /// @param src The index set to copy from
    void assign(const IndexSet & src);

    /// Creates an index set object
    ///
    /// @param comm The MPI communicator
    void create(MPI_Comm comm);

    /// Destroys an index set
    void destroy();

    /// Creates a duplicate copy of an index set
    ///
    /// @return The copy of the index set
    [[nodiscard]] IndexSet duplicate() const;

    void get_indices();

    void restore_indices();

    /// Returns a description of the points in an IndexSet suitable for traversal
    ///
    /// @param start The first index
    /// @param end  One past the last index
    /// @param points The indices
    void get_point_range(Int & start, Int & end, const Int *& points) const;

    /// Destroys the traversal description created with `get_point_range`
    ///
    /// @param start The first index
    /// @param end One past the last index
    /// @param points The indices
    void restore_point_range(Int start, Int end, const Int * points) const;

    /// Configures the IndexSet to be a subrange for the traversal information given
    ///
    /// @param start The first index of the subrange
    /// @param end One past the last index for the subrange
    /// @param points The indices for the entire range, from `get_point_range`
    void get_point_subrange(Int start, Int end, const Int * points) const;

    /// Returns the global length of an index set.
    ///
    /// @return The global size of the index set
    [[nodiscard]] Int get_size() const;

    /// Returns the local (processor) length of an index set.
    ///
    /// @return The local size of the index set
    [[nodiscard]] Int get_local_size() const;

    [[nodiscard]] const Int * data() const;

    Int operator[](Int i) const;

    Int operator()(Int i) const;

    /// Convert indices from this index set into std::vector
    ///
    /// @return std::vector containing the indices
    std::vector<Int> to_std_vector();

    /// Increase reference of this object
    void inc_ref();

    /// Convert this object to a PETSc object so it can be passed directly into PETSc API
    operator IS() const;

    /// Convert this object to a PETSc object so it can be passed directly into PETSc API
    operator IS *();

    /// Checks if the IndexSet is empty
    ///
    /// @return `true` if the index set is empty, `false` otherwise
    [[nodiscard]] bool empty() const;

    /// Returns the PETSc ID of the index set
    ///
    /// @return The PETSc ID
    [[nodiscard]] PetscObjectId get_id() const;

    /// Shift all indices by given offset
    ///
    /// @param offset The offset to shift the indices by
    void shift(Int offset);

    /// Checks the indices to determine whether they have been sorted
    ///
    /// @return `true` is index set is sorted
    [[nodiscard]] bool sorted() const;

    /// Sort the indices of the index set
    void sort() const;

    /// Sort the indices of the index set, removing duplicates.
    void sort_remove_dups() const;

    /// View the index set
    ///
    /// @param viewer The PETSc viewer
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    /// Begin iterator for range-based for-loops
    Iterator begin();

    /// End iterator for range-based for-loops
    Iterator end();

private:
    IS is;
    const Int * indices;

public:
    /// Creates a data structure for an index set containing a list of integers.
    ///
    /// @param comm The MPI communicator
    /// @param idx The length of the index set
    /// @param copy_mode The copy mode see, `CopyMode` for details
    static IndexSet
    create_general(MPI_Comm comm, const std::vector<Int> & idx, CopyMode copy_mode = COPY_VALUES);

    static IndexSet intersect_caching(const IndexSet & is1, const IndexSet & is2);

    /// Computes the intersection of two index sets, by sorting and comparing.
    ///
    /// @param is1 The first index set
    /// @param is2 The second index set
    /// @return The sorted intersection of `is1` and `is2`
    static IndexSet intersect(const IndexSet & is1, const IndexSet & is2);

    /// Copies an index set
    ///
    /// @param src The index set to copy from
    /// @param dest The index set to copy to
    static void copy(const IndexSet & src, IndexSet & dest);
};

} // namespace godzilla
