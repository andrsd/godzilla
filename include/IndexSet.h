#pragma once

#include "GodzillaConfig.h"
#include "Types.h"
#include "petsc.h"
#include <vector>

namespace godzilla {

class IndexSet {
public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = Int;

        explicit Iterator(IndexSet & is, Int idx);
        ~Iterator();

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
    ~IndexSet();

    void create(MPI_Comm comm);
    void destroy();

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

    NO_DISCARD Int get_size() const;
    NO_DISCARD Int get_local_size() const;
    NO_DISCARD const Int * data() const;
    Int operator[](Int i) const;
    Int operator()(Int i) const;

    /// Convert indices from this index set into std::vector
    ///
    /// @return std::vector containing the indices
    std::vector<Int> to_std_vector();

    void inc_ref();

    operator IS() const;
    operator IS *();

    bool empty() const;

    PetscObjectId get_id() const;

    /// Checks the indices to determine whether they have been sorted
    ///
    /// @return `true` is index set is sorted
    bool sorted() const;

    /// Sort the indices of the index set
    void sort() const;

    /// Sort the indices of the index set, removing duplicates.
    void sort_remove_dups() const;

    /// Begin iterator for range-based for-loops
    Iterator begin();

    /// End iterator for range-based for-loops
    Iterator end();

private:
    IS is;
    const Int * indices;

public:
    static IndexSet create_general(MPI_Comm comm, const std::vector<Int> & idx);
    static IndexSet intersect_caching(const IndexSet & is1, const IndexSet & is2);
    static IndexSet intersect(const IndexSet & is1, const IndexSet & is2);
};

} // namespace godzilla
