#pragma once

#include "GodzillaConfig.h"
#include "petsc.h"
#include <vector>

namespace godzilla {

class IndexSet {
public:
    IndexSet();
    explicit IndexSet(IS is);
    ~IndexSet();

    void create(MPI_Comm comm);

    NO_DISCARD PetscInt size() const;
    NO_DISCARD const PetscInt * data() const;
    PetscInt operator[](unsigned int i) const;

    /// Convert indices from this index set into std::vector
    ///
    /// @return std::vector containing the indices
    std::vector<PetscInt> to_std_vector();

private:
    void get_indices_internal();

    IS is;
    PetscInt n;
    const PetscInt * indices;

public:
    static IndexSet values_from_label(DMLabel label);
};

} // namespace godzilla
