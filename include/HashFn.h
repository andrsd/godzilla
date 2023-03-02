#pragma once

#include "petsc/private/hashtable.h"
#include "Types.h"

namespace godzilla {

/// Template for hash functions
template <class KEY>
class HashFn {
};

/// Hash function for `Int`
template <>
class HashFn<Int> {
public:
    HashFn(Int k)
    {
#if defined(PETSC_USE_64BIT_INDICES)
        this->key = PetscHash_UInt64((PetscHash64_t) k);
#else
        this->key = PetscHash_UInt32((PetscHash32_t) k);
#endif
    }

    operator khint_t() const
    {
        return this->key;
    }

private:
    khint_t key;
};

} // namespace godzilla
