#pragma once

#include "petsc/private/khash/khash.h"
#include "petsc/private/hashtable.h"
#include "CallStack.h"
#include "Types.h"
#include "HashFn.h"
#include "HashEqual.h"

namespace godzilla {

/// Template for hash map
template <class KEY, class VAL, class HASH_FN = HashFn<KEY>, class HASH_EQUAL = HashEqual<KEY>>
class HashMap {
private:
    __KHASH_TYPE(ght, KEY, VAL);
    __KHASH_IMPL(ght, inline, KEY, VAL, 1, HASH_FN, HASH_EQUAL);

public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;

        Iterator(kh_ght_t * ht, khiter_t it) : ht(ht), it(it) {}

        Iterator &
        operator++()
        {
            PetscHashIterNext(this->ht, this->it);
            return *this;
        }

        Iterator
        operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool
        operator==(const Iterator & a, const Iterator & b)
        {
            return (a.ht == b.ht) && (a.it == b.it);
        }

        friend bool
        operator!=(const Iterator & a, const Iterator & b)
        {
            return (a.ht != b.ht) || (a.it != b.it);
        }

    private:
        kh_ght_t * ht;
        khiter_t it;
    };

public:
    HashMap();

    /// Create a hash table
    void create();

    /// Destroy a hash table
    void destroy();

    /// Reset a hash table
    void reset();

    /// Clear a hash table
    void clear();

    /// Set the number of buckets in a hash table
    void resize(Int nb);

    /// Get the number of entries in a hash table
    Int get_size() const;

    /// Get the current size of the array in the hash table
    Int get_capacity() const;

    /// Query for a key in the hash table
    bool has(const KEY & key);

    /// Get the value for a key in the hash table
    const VAL & get(const KEY & key);

    /// Set a (key,value) entry in the hash table
    void set(const KEY & key, const VAL & val);

    /// Remove a key and its value from the hash table
    void del(const KEY & key);

    /// Query and set a (key,value) entry in the hash table
    ///
    /// @param key
    /// @param val
    /// @return Boolean indicating whether the key was missing
    bool query_set(const KEY & key, const VAL & val);

    /// Query and remove a (key,value) entry from the hash table
    ///
    /// @param key
    /// @return Boolean indicating whether the key was present
    bool query_del(const KEY & key);

    /// Query for key in the hash table
    ///
    /// @param key The key
    /// @return
    Iterator find(const KEY & key);

    Iterator put(const KEY & key);

    // ...

    /// Get all keys from a hash table
    std::vector<KEY> get_keys(PetscInt off = 0);

    /// Get all values from a hash table
    std::vector<VAL> get_vals(PetscInt off = 0);

    Iterator
    begin()
    {
        return Iterator(this->ht, kh_begin(this->ht));
    }

    Iterator
    end()
    {
        return Iterator(this->ht, kh_end(this->ht));
    }

private:
    kh_ght_t * ht;
};

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::HashMap() : ht(nullptr)
{
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::create()
{
    _F_;
    this->ht = kh_init(ght);
    assert(this->ht != nullptr);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::destroy()
{
    _F_;
    kh_destroy(ght, this->ht);
    this->ht = nullptr;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::reset()
{
    _F_;
    kh_reset(ght, this->ht);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::clear()
{
    _F_;
    kh_clear(ght, this->ht);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::resize(Int nb)
{
    _F_;
    int ret = kh_resize(ght, this->ht, (khint_t) nb);
    assert(ret >= 0);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
Int
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::get_size() const
{
    _F_;
    return (Int) kh_size(this->ht);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
Int
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::get_capacity() const
{
    _F_;
    return (Int) kh_n_buckets(this->ht);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
bool
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::has(const KEY & key)
{
    _F_;
    khiter_t iter = kh_get(ght, this->ht, key);
    return (iter != kh_end(this->ht));
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
const VAL &
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::get(const KEY & key)
{
    _F_;
    khiter_t iter = kh_get(ght, this->ht, key);
    if (iter == kh_end(this->ht)) {
        int ret;
        iter = kh_put(ght, this->ht, key, &ret);
        assert(ret >= 0);
        kh_val(this->ht, iter) = VAL();
    }
    return kh_val(this->ht, iter);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::set(const KEY & key, const VAL & val)
{
    _F_;
    int ret;
    khiter_t iter = kh_put(ght, this->ht, key, &ret);
    assert(ret >= 0);
    kh_val(this->ht, iter) = val;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::del(const KEY & key)
{
    _F_;
    khiter_t iter = kh_get(ght, this->ht, key);
    kh_del(ght, this->ht, iter);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
bool
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::query_set(const KEY & key, const VAL & val)
{
    _F_;
    int ret;
    khiter_t iter = kh_put(ght, this->ht, key, &ret);
    assert(ret >= 0);
    kh_val(this->ht, iter) = val;
    return ret != 0;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
bool
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::query_del(const KEY & key)
{
    _F_;
    khiter_t iter = kh_get(ght, this->ht, key);
    if (iter != kh_end(this->ht)) {
        kh_del(ght, this->ht, iter);
        return true;
    }
    else
        return false;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
typename HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::Iterator
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::find(const KEY & key)
{
    _F_;
    PetscHashIter iter = kh_get(ght, this->ht, key);
    return Iterator(this->ht, iter);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
typename HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::Iterator
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::put(const KEY & key)
{
    _F_;
    int ret;
    PetscHashIter iter = kh_put(ght, this->ht, key, &ret);
    assert(ret >= 0);
    return Iterator(this->ht, iter);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
std::vector<KEY>
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::get_keys(PetscInt off)
{
    _F_;
    KEY k;
    std::vector<KEY> keys;
    keys.resize((std::size_t) get_size());
    Int pos = off;
    kh_foreach_key(this->ht, k, keys[pos++] = k);
    return keys;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
std::vector<VAL>
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::get_vals(PetscInt off)
{
    _F_;
    VAL v;
    std::vector<VAL> vals;
    vals.resize((std::size_t) get_size());
    Int pos = off;
    kh_foreach_value(this->ht, v, vals[pos++] = v);
    return vals;
}

} // namespace godzilla
