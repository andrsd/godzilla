#pragma once

#include "GodzillaConfig.h"

namespace godzilla {

/// Dynamic array of items using Judy
template <class TYPE>
class Array {
public:
    Array();
    virtual ~Array();

    /// Insert an item at position index.
    /// @param[in] idx Index to insert.
    /// @return true, if ok, else false
    bool set(Index idx, TYPE item);

    /// Add an item to the end of the array
    Index add(TYPE item);

    /// Check that item in `index` exists
    bool exists(Index idx) const;

    /// Get item from index `idx`
    const TYPE & get(Index idx) const;

    /// Get const reference to an item at position `idx`
    const TYPE & operator[](Index idx) const;

    /// Get write reference to an item at position `idx`
    TYPE & operator[](Index idx);

    /// Delete an item at index from the array.
    /// @param[in] idx Index to delete.
    void remove(Index idx);

    /// Count the number of indices present in the array between index1 and index2 (inclusive).
    /// @param[in] index1 Starting index. Optional, default value `0`.
    /// @param[in] index2 End index. Optional, default value `(Index) -1`.
    /// @return Number of indices present in the array between index1 and index2 (inclusive).
    Index count(Index index1 = 0, Index index2 = (Index) -1) const;

    /// Remove all items from the array
    /// Does NOT destruct items in the array
    void remove_all();

    // Iterators

    /// Get the first index that is present and is equal to or greater than the passed `idx`.
    /// Typically used to begin an iteration over all indices present in the array.
    /// @param[in] idx Optional, default value `0` (finds the first present index).
    /// @return
    ///     - First index present in the array that is equal or greater than the passed `idx` (if
    ///       found),
    ///     - `INVALID_IDX` (if not found).
    Index first(Index idx = 0) const;

    /// Get the first index that is present and is greater than the passed `idx`.
    /// Typically used to continue an iteration over all indices present in the array.
    /// @param[in] idx Index whose succesor we want to find. Optional, default value `0`.
    /// @return
    ///     - First idx present in the array that is greater than the passed `idx` (if found),
    ///     - `INVALID_IDX` (if not found).
    Index next(Index idx = 0) const;

    /// Get the last index present in the array that is equal to or less than the passed `idx`.
    /// Typically used to begin a reverse iteration over all indices present in the array.
    /// @param[in] idx Optional, default value `(Index) -1` (finds the last index present in
    /// the array).
    /// @return
    ///     - Last index present in the array that is equal or less than the passed `idx` (if
    ///     found),
    ///     - `INVALID_IDX` (if not found).
    Index last(Index idx = (Index) -1) const;

    /// Get the last index present in the array that is less than the passed `idx`.
    /// Typically used to continue a reverse iteration over all indices present in the array.
    /// @param[in] idx Index whose predecessor we want to find. Optional, default value `(Index)
    /// -1`.
    /// @return
    ///     - Last index present in the array that is less than the passed `idx` (if found),
    ///     - `INVALID_IDX` (if not found).
    Index prev(Index idx = (Index) -1) const;

protected:
    void free_item(Index idx);

    void * judy;
};

template <class TYPE>
Array<TYPE>::Array()
{
    judy = NULL;
};

template <class TYPE>
Array<TYPE>::~Array()
{
    remove_all();
}

template <class TYPE>
bool
Array<TYPE>::set(Index idx, TYPE item)
{
    void * pval;
    JLG(pval, judy, idx);
    if (pval == NULL) {
        JLI(pval, judy, idx);
        if (pval == NULL)
            return false;
        else {
            *((TYPE **) pval) = new TYPE;
            **((TYPE **) pval) = item;
            return true;
        }
    }
    else {
        **((TYPE **) pval) = item;
        return true;
    }
}

template <class TYPE>
Index
Array<TYPE>::add(TYPE item)
{
    int rc;
    Index idx = last();
    if (idx == INVALID_IDX)
        idx = 0;
    JLFE(rc, judy, idx);
    if (rc) {
        set(idx, item);
        return idx;
    }
    else
        return INVALID_IDX;
}

template <class TYPE>
bool
Array<TYPE>::exists(Index idx) const
{
    void * pval;
    JLG(pval, judy, idx);
    return pval != NULL;
}

template <class TYPE>
const TYPE &
Array<TYPE>::get(Index idx) const
{
    void * pval;
    JLG(pval, judy, idx);
    assert(pval != NULL);
    return **((TYPE **) pval);
}

template <class TYPE>
const TYPE &
Array<TYPE>::operator[](Index idx) const
{
    return get(idx);
}

template <class TYPE>
TYPE &
Array<TYPE>::operator[](Index idx)
{
    void * pval;
    JLG(pval, judy, idx);
    if (pval == NULL) {
        JLI(pval, judy, idx);
        *((TYPE **) pval) = new TYPE;
    }

    return **((TYPE **) pval);
}

template <class TYPE>
void
Array<TYPE>::free_item(Index idx)
{
    void * pval;
    JLG(pval, judy, idx);
    if (pval != NULL) {
        TYPE * n = *((TYPE **) pval);
        delete n;
    }
}

template <class TYPE>
void
Array<TYPE>::remove(Index idx)
{
    free_item(idx);
    int rc;
    JLD(rc, judy, idx);
}

template <class TYPE>
Index
Array<TYPE>::count(Index index1 /* = 0*/, Index index2 /* = (Index) -1*/) const
{
    Index count;
    JLC(count, judy, index1, index2);
    return count;
}

template <class TYPE>
void
Array<TYPE>::remove_all()
{
    for (Index i = first(); i != INVALID_IDX; i = next(i))
        free_item(i);

    int val;
    JLFA(val, judy);
}

template <class TYPE>
Index
Array<TYPE>::first(Index idx /* = 0*/) const
{
    void * pval;
    JLF(pval, judy, idx);
    return pval ? idx : INVALID_IDX;
}

template <class TYPE>
Index
Array<TYPE>::next(Index idx /* = 0*/) const
{
    void * pval;
    JLN(pval, judy, idx);
    return pval ? idx : INVALID_IDX;
}

template <class TYPE>
Index
Array<TYPE>::last(Index idx /* = (Index) -1*/) const
{
    void * pval;
    JLL(pval, judy, idx);
    return pval ? idx : INVALID_IDX;
}

template <class TYPE>
Index
Array<TYPE>::prev(Index idx /* = (Index) -1*/) const
{
    void * pval;
    JLP(pval, judy, idx);
    return pval ? idx : INVALID_IDX;
}

} // namespace godzilla
