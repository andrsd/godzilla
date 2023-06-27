#pragma once

#include "GodzillaConfig.h"
#include "Types.h"
#include "petsc.h"
#include <vector>

namespace godzilla {

class IndexSet {
public:
    IndexSet();
    explicit IndexSet(IS is);
    ~IndexSet();

    void create(MPI_Comm comm);
    void destroy();

    void get_indices();
    void restore_indices();

    NO_DISCARD Int get_size() const;
    NO_DISCARD Int get_local_size() const;
    NO_DISCARD const Int * data() const;
    Int operator[](unsigned int i) const;
    Int operator()(unsigned int i) const;

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

private:
    IS is;
    const Int * indices;

public:
    static IndexSet create_general(MPI_Comm comm, const std::vector<Int> & idx);
    static IndexSet intersect_caching(const IndexSet & is1, const IndexSet & is2);
    static IndexSet intersect(const IndexSet & is1, const IndexSet & is2);
};

} // namespace godzilla
