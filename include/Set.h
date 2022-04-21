#pragma once

#include <Judy.h>

namespace godzilla {

/// Implementation of a hash table mapping an Index to a bit (0/1).
///
/// Provides functionality of a sparse bit vector.
class Set {
    void * judy;

public:
    Set() : judy(NULL) {}
    virtual ~Set() { free(); }

    /// Set `index`'s bit in the array.
    /// @param[in] index index to set.
    /// @retval false if the bit was already set.
    /// @retval true if the bit was previously unset and was succesfully set.
    bool
    set(Index index)
    {
        int rc;
        J1S(rc, judy, index);
        return rc == 1;
    }

    /// Unset `index`'s bit in the array.
    /// @param[in] index index to unset.
    /// @retval false if the bit was already unset.
    /// @retval true if the bit was previously set and was succesfully unset (removed from the
    /// array).
    bool
    unset(Index index)
    {
        int rc;
        J1U(rc, judy, index);
        return rc == 1;
    }

    /// Test whether `index`'s bit is set.
    /// @param[in] index Index to test.
    /// @retval false if the index is unset (absent in the array).
    /// @retval true if the index is set (present in the array).
    bool
    has(Index index)
    {
        int rc;
        J1T(rc, judy, index);
        return rc == 1;
    }

    /// Count the number of indices present in the array between index1 and index2 (inclusive).
    /// @param[in] index1 Starting index. Optional, default value `0`.
    /// @param[in] index2 End index. Optional, default value `(Index) -1`.
    /// @return Number of indices present in the array between index1 and index2 (inclusive).
    Index
    count(Index index1 = 0, Index index2 = (Index) -1) const
    {
        Index count;
        J1C(count, judy, index1, index2);
        return count;
    }

    /// Locate the `nth` index that is present in the array.
    /// @param[in] nth N-th index we want to locate. `nth` equal to 1 returns the first index set.
    /// @return N-th set index in the array if it is found, `INVALID_IDX` otherwise.
    Index
    by_count(Index nth) const
    {
        int rc;
        Index index;
        J1BC(rc, judy, nth, index);
        return rc ? index : INVALID_IDX;
    }

    /// Get the first index that is set and is equal to or greater than the passed \c index.
    /// Typically used to begin an iteration over all set indices in the array.
    /// @param[in] index Optional, default value `0` (finds the first set index).
    /// @retval First set index that is equal or greater than the passed `index`, if found,
    /// @retval `INVALID_IDX`, if not found.
    Index
    first(Index index = 0) const
    {
        int rc;
        J1F(rc, judy, index);
        return rc ? index : INVALID_IDX;
    }

    /// Get the first set index that is greater than the passed `index`.
    /// Typically used to continue an iteration over all set indices in the array.
    /// @param[in] index Index whose succesor we want to find. Optional, default value \c 0.
    /// @retval First set index that is greater than the passed `index`, if found,
    /// @reval `INVALID_IDX`, if not found.
    Index
    next(Index index = 0) const
    {
        int rc;
        J1N(rc, judy, index);
        return rc ? index : INVALID_IDX;
    }

    /// Get the last index that is set and is equal to or less than the passed `index`.
    /// Typically used to begin a reverse iteration over all set indices in the array.
    /// @param[in] index Optional, default value `(Index) -1` (finds the last set index).
    /// @retval Last set index that is equal or less than the passed `index` (if found),
    /// @retval `INVALID_IDX`, if not found.
    Index
    last(Index index = (Index) -1) const
    {
        int rc;
        J1L(rc, judy, index);
        return rc ? index : INVALID_IDX;
    }

    /// Get the last set index that is less than the passed `index`.
    /// Typically used to continue a reverse iteration over all set indices in the array.
    /// @param[in] index Index whose predecessor we want to find. Optional, default value
    /// `(Index) -1`.
    /// @retval Last set index that is less than the passed `index`, if found,
    /// @retval `INVALID_IDX`, if not found.
    Index
    prev(Index index = (Index) -1) const
    {
        int rc;
        J1P(rc, judy, index);
        return rc ? index : INVALID_IDX;
    }

    /// Get the first unset index in the array that is equal to or greater than the passed `index`.
    /// @param[in] index Optional, default value `0` (finds the first unset index).
    /// @retval First unset index that is equal or greater than the passed `index`, if found,
    /// @retval `INVALID_IDX`, if not found.
    Index
    first_empty(Index index = 0) const
    {
        int rc;
        J1FE(rc, judy, index);
        return rc ? index : INVALID_IDX;
    }

    /// Get the first unset index that is greater than passed `index`.
    /// @param[in] index Index whose unset succesor we want to find. Optional, default value `0`.
    /// @retval First unset index that is greater than the passed `index`, if found,
    /// @retval `INVALID_IDX`, if not found.
    Index
    next_empty(Index index = 0) const
    {
        int rc;
        J1NE(rc, judy, index);
        return rc ? index : INVALID_IDX;
    }

    /// Get the last unset index that is equal to or less than the passed `index`.
    /// @param[in] index Optional, default value `(Index)-1` (finds the last unset index).
    /// @retval Last unset index that is equal or less than the passed `index`, if found,
    /// @reval `INVALID_IDX`, if not found.
    Index
    last_empty(Index index = (Index) -1) const
    {
        int rc;
        J1LE(rc, judy, index);
        return rc ? index : INVALID_IDX;
    }

    /// Get the last unset index that is less than passed `index`.
    /// @param[in] index Index whose unset predecessor we want to find. Optional, default value
    /// `(Index)-1`.
    /// @retval Last unset index that is less than the passed `index`, if found,
    /// @retval `INVALID_IDX`, if not found.
    Index
    prev_empty(Index index = (Index) -1) const
    {
        int rc;
        J1PE(rc, judy, index);
        return rc ? index : INVALID_IDX;
    }

    /// Make a copy of array
    bool
    copy(Set * original)
    {
        for (Index ind = original->first(); ind != INVALID_IDX; ind = original->next(ind)) {
            if (!set(ind))
                return false;
        }
        return true;
    }

    /// unset all elements
    void
    free()
    {
        int val;
        J1FA(val, judy);
        judy = NULL;
    }
};

}
