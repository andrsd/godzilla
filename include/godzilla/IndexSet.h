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

        explicit Iterator(IndexSet * is, Int idx);
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
        IndexSet * is;
        /// Index pointing to the `is`
        Int idx;
    };

    struct ConstIterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = Int;
        using difference_type = Int;
        using pointer = const Int *;
        using reference = const Int &;

        explicit ConstIterator(const IndexSet * is, Int idx) : is(is), idx(idx) {}
        ~ConstIterator() = default;

        const value_type &
        operator*()
        {
            return this->is->indices[this->idx];
        }

        ConstIterator &
        operator++()
        {
            ++this->idx;
            return *this;
        }

        ConstIterator
        operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool
        operator==(const ConstIterator & a, const ConstIterator & b)
        {
            return ((IS) a.is == (IS) b.is) && (a.idx == b.idx);
        }

        friend bool
        operator!=(const ConstIterator & a, const ConstIterator & b)
        {
            return ((IS) a.is != (IS) b.is) || (a.idx != b.idx);
        }

    private:
        const IndexSet * is;
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

    /// Generates the complement index set. That is all indices that are NOT in the given set.
    ///
    /// @param nmin The first index desired in the local part of the complement
    /// @param nmax The largest index desired in the local part of the complement (note that all
    ///             indices in this indexs set must be greater or equal to `nmin` and less than
    ///             `nmax`)
    IndexSet complement(Int nmin, Int nmax) const;

    /// Destroys an index set
    void destroy();

    /// Test if the index set is "null"
    ///
    /// @return `true` is the label is null, `false` otherwise
    bool is_null() const;

    /// Creates a duplicate copy of an index set
    ///
    /// @return The copy of the index set
    IndexSet duplicate() const;

    /// Compares if this index set has the same set of indices as the `other`
    ///
    /// @param other The index set to compare with
    /// @return `true` if the index sets are equal, `false` otherwise
    bool equal(const IndexSet & other) const;

    /// Compares if this index set has the same set of indices as the `other`
    ///
    /// @param other The index set to compare with
    /// @return `true` if the index sets are equal, `false` otherwise
    ///
    /// @note This routine does NOT sort the contents of the index sets before the comparison is
    /// made, i.e., the order of indices is important.
    bool equal_unsorted(const IndexSet & other) const;

    void get_indices();

    /// Gets the index set type name
    ///
    /// @return The type name
    std::string get_type() const;

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

    /// Gets the minimum and maximum values
    ///
    /// @return A tuple containing the minimum and maximum values
    std::tuple<Int, Int> get_min_max() const;

    /// Returns the global length of an index set.
    ///
    /// @return The global size of the index set
    Int get_size() const;

    /// Returns the local (processor) length of an index set.
    ///
    /// @return The local size of the index set
    Int get_local_size() const;

    const Int * data() const;

    /// Determine the location of an index within the local component of an index set
    ///
    /// @param key The index to locate
    /// @return if >= 0, a location within the index set that is equal to the key, otherwise the key
    ///         is not in the index set
    Int locate(Int key) const;

    Int operator[](Int i) const;

    Int operator()(Int i) const;

    /// Typecast operator to determine if label is null or not
    operator bool() const;
    operator bool();

    /// Convert indices from this index set into std::vector
    ///
    /// @return std::vector containing the indices
    std::vector<Int> to_std_vector();

    /// Determines whether index set is the identity mapping.
    ///
    /// @return `true` if the index set is the identity mapping, `false` otherwise
    bool identity() const;

    /// Sets the index set to be the identity mapping.
    void set_identity();

    /// Determines whether index set is a permutation.
    ///
    /// @return `true` if the index set is a permutation, `false` otherwise
    bool permutation() const;

    /// Informs the index set that it is a permutation.
    void set_permutation();

    /// Builds a index set, for a particular type
    ///
    /// @param type The type of index set to build
    void set_type(const std::string & type);

    /// Increase reference of this object
    void inc_ref();

    /// Convert this object to a PETSc object so it can be passed directly into PETSc API
    operator IS() const;

    /// Convert this object to a PETSc object so it can be passed directly into PETSc API
    operator IS *();

    /// Checks if the IndexSet is empty (i.e., has no indices)
    ///
    /// @return `true` if the index set is empty, `false` otherwise
    bool empty() const;

    /// Returns the PETSc ID of the index set
    ///
    /// @return The PETSc ID
    PetscObjectId get_id() const;

    /// Shift all indices by given offset
    ///
    /// @param offset The offset to shift the indices by
    void shift(Int offset);

    /// Checks the indices to determine whether they have been sorted
    ///
    /// @return `true` is index set is sorted
    bool sorted() const;

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
    ConstIterator begin() const;

    /// End iterator for range-based for-loops
    Iterator end();
    ConstIterator end() const;

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

    /// Forms a new IS by locally concatenating the indices from an IS list without reordering.
    ///
    /// @param comm The MPI communicator
    /// @param is_list The list of index sets to concatenate
    /// @return The concatenated index set
    static IndexSet concatenate(MPI_Comm comm, const std::vector<IndexSet> & is_list);

    /// Computes the difference between two index sets.
    ///
    /// @param is1 The first index set, to have items removed from it
    /// @param is2 Index values to be removed
    /// @return The difference between `is1` and `is2`
    static IndexSet difference(const IndexSet & is1, const IndexSet & is2);

    /// Computes the union of two index sets, by concatenating 2 lists and removing duplicates.
    ///
    /// @param is1 The first index set
    /// @param is2 The second index set
    /// @return The union of `is1` and `is2`
    static IndexSet expand(const IndexSet & is1, const IndexSet & is2);

    /// Computes the sum (union) of two index sets.
    ///
    /// @param is1 The first index set
    /// @param is2 The second index set
    /// @return The sum of `is1` and `is2`
    static IndexSet sum(const IndexSet & is1, const IndexSet & is2);
};

} // namespace godzilla
