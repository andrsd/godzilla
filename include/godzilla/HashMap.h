// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
#include <stdexcept>

namespace godzilla {

#if UINT_MAX == 0xffffffffu
typedef unsigned int khint32_t;
#elif ULONG_MAX == 0xffffffffu
typedef unsigned long khint32_t;
#endif

#if ULONG_MAX == ULLONG_MAX
typedef unsigned long khint64_t;
#else
typedef unsigned long long khint64_t;
#endif

typedef khint32_t khint_t;
typedef khint32_t Hash32_t;
typedef khint64_t Hash64_t;
typedef khint_t Hash_t;

// Thomas Wang's version for 64bit integer -> 32bit hash
static inline Hash32_t
Hash_UInt64_32(Hash64_t key)
{
    key = ~key + (key << 18); /* key = (key << 18) - key - 1; */
    key = key ^ (key >> 31);
    key = key * 21; /* key = (key + (key << 2)) + (key << 4);  */
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);
    return (Hash32_t) key;
}

// Thomas Wang's version for 64bit integer -> 64bit hash
static inline Hash64_t
Hash_UInt64_64(Hash64_t key)
{
    key = ~key + (key << 21); /* key = (key << 21) - key - 1; */
    key = key ^ (key >> 24);
    key = key * 265; /* key = (key + (key << 3)) + (key << 8);  */
    key = key ^ (key >> 14);
    key = key * 21; /* key = (key + (key << 2)) + (key << 4); */
    key = key ^ (key >> 28);
    key = key + (key << 31);
    return key;
}

// Thomas Wang's second version for 32bit integers
static inline Hash_t
Hash_UInt32_v1(Hash32_t key)
{
    key = ~key + (key << 15); /* key = (key << 15) - key - 1; */
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057; /* key = (key + (key << 3)) + (key << 11); */
    key = key ^ (key >> 16);
    return key;
}

/// Template for hash functions
template <class KEY>
class HashFn {};

/// Hash function for `Int`
template <>
class HashFn<Int> {
public:
    HashFn(Int k)
    {
#if defined(PETSC_USE_64BIT_INDICES)
        this->key = Hash_UInt64_32(k);
#else
        this->key = Hash_UInt32_v1(k);
#endif
    }

    operator khint_t() const { return this->key; }

private:
    khint_t key;
};

///

/// Template for determining the equality of 2 hashes
template <class T>
class HashEqual {};

template <>
class HashEqual<Int> {
public:
    HashEqual(Int a, Int b) { this->equal = (a == b); }

    operator bool() const { return this->equal; }
    operator bool() { return this->equal; }

private:
    bool equal;
};

/// Template for hash map
///
/// @tparam KEY
/// @tparam VAL
/// @tparam HASH_FN
/// @tparam HASH_EQUAL
///
/// Based on
/// https://github.com/attractivechaos/klib/blob/928581a78413bed4efa956731b35b18a638f20f3/khash.h
template <class KEY, class VAL, class HASH_FN = HashFn<KEY>, class HASH_EQUAL = HashEqual<KEY>>
class HashMap {
public:
    struct Iterator;

    HashMap();

    /// Create a hash table
    ///
    /// @param n Initial number of entries in the hash table
    void create(Int n = 0);

    /// Destroy a hash table
    void destroy();

    /// Reset a hash table
    void reset();

    /// Clear a hash table
    void clear();

    /// Set the number of buckets in a hash table
    int resize(Int nb);

    /// Get the number of entries in a hash table
    Int get_size() const;

    /// Get the current size of the array in the hash table
    Int get_capacity() const;

    /// Query for a key in the hash table
    bool has(const KEY & key);

    /// Get the value for a key in the hash table
    const VAL & get(const KEY & key) const;

    /// Get a writeable reference to an entry with a `key`
    VAL & set(const KEY & key);

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
    std::vector<KEY> get_keys(Int off = 0);

    /// Get all values from a hash table
    std::vector<VAL> get_vals(Int off = 0);

    Iterator
    begin()
    {
        return Iterator(this, kh_begin());
    }

    Iterator
    end()
    {
        return Iterator(this, kh_end());
    }

private:
    static constexpr double HASH_UPPER = 0.77;

    inline bool
    isempty(khint32_t * flag, int i) const
    {
        return (flag[i >> 4] >> ((i & 0xfU) << 1)) & 2;
    }

    inline bool
    isdel(khint32_t * flag, int i) const
    {
        return (flag[i >> 4] >> ((i & 0xfU) << 1)) & 1;
    }

    inline bool
    iseither(khint32_t * flag, int i) const
    {
        return (flag[i >> 4] >> ((i & 0xfU) << 1)) & 3;
    }

    inline void
    set_isdel_false(khint32_t * flag, int i)
    {
        flag[i >> 4] &= ~(1U << ((i & 0xfU) << 1));
    }

    inline void
    set_isempty_false(khint32_t * flag, int i)
    {
        flag[i >> 4] &= ~(2U << ((i & 0xfU) << 1));
    }

    inline void
    set_isboth_false(khint32_t * flag, int i)
    {
        flag[i >> 4] &= ~(3U << ((i & 0xfU) << 1));
    }

    inline void
    set_isdel_true(khint32_t * flag, int i)
    {
        flag[i >> 4] |= 1U << ((i & 0xfU) << 1);
    }

    inline khint_t
    fsize(khint_t m) const
    {
        return m < 16 ? 1 : m >> 4;
    }

    inline void
    kroundup32(khint_t & x)
    {
        --x, x |= x >> 1, x |= x >> 2, x |= x >> 4, x |= x >> 8, x |= x >> 16, ++x;
    }

    typedef khint_t khiter_t;

    khint_t n_buckets, size, n_occupied, upper_bound;
    khint32_t * flags;
    KEY * keys;
    VAL * vals;

    inline khint_t
    kh_get(KEY key) const
    {
        if (this->n_buckets) {
            khint_t k, i, last, mask, step = 0;
            mask = this->n_buckets - 1;
            k = HASH_FN(key);
            i = k & mask;
            last = i;
            while (!isempty(this->flags, i) &&
                   (isdel(this->flags, i) || !HASH_EQUAL(this->keys[i], key))) {
                i = (i + (++step)) & mask;
                if (i == last)
                    return this->n_buckets;
            }
            return iseither(this->flags, i) ? this->n_buckets : i;
        }
        else
            return 0;
    }

    inline khint_t
    kh_put(KEY key, int * ret)
    {
        khint_t x;
        if (this->n_occupied >= this->upper_bound) {
            /* update the hash table */
            if (this->n_buckets > (this->size << 1)) {
                if (resize(this->n_buckets - 1) < 0) {
                    /* clear "deleted" elements */
                    *ret = -1;
                    return this->n_buckets;
                }
            }
            else if (resize(this->n_buckets + 1) < 0) {
                /* expand the hash table */
                *ret = -1;
                return this->n_buckets;
            }
        }
        /* TODO: to implement automatically shrinking; resize() already support shrinking */
        {
            khint_t k, i, site, last, mask = this->n_buckets - 1, step = 0;
            x = site = this->n_buckets;
            k = HASH_FN(key);
            i = k & mask;
            if (isempty(this->flags, i))
                /* for speed up */
                x = i;
            else {
                last = i;
                while (!isempty(this->flags, i) &&
                       (isdel(this->flags, i) || !HASH_EQUAL(this->keys[i], key))) {
                    if (isdel(this->flags, i))
                        site = i;
                    i = (i + (++step)) & mask;
                    if (i == last) {
                        x = site;
                        break;
                    }
                }
                if (x == this->n_buckets) {
                    if (isempty(this->flags, i) && site != this->n_buckets)
                        x = site;
                    else
                        x = i;
                }
            }
        }
        if (isempty(this->flags, x)) {
            /* not present at all */
            this->keys[x] = key;
            set_isboth_false(this->flags, x);
            ++this->size;
            ++this->n_occupied;
            *ret = 1;
        }
        else if (isdel(this->flags, x)) {
            /* deleted */
            this->keys[x] = key;
            set_isboth_false(this->flags, x);
            ++this->size;
            *ret = 2;
        }
        else
            /* Don't touch h->keys[x] if present and not deleted */
            *ret = 0;
        return x;
    }

    inline void
    kh_del(khint_t x)
    {
        if (x != this->n_buckets && !iseither(this->flags, x)) {
            set_isdel_true(this->flags, x);
            --this->size;
        }
    }

    inline khiter_t
    kh_begin() const
    {
        return 0;
    }

    inline khiter_t
    kh_end() const
    {
        return this->n_buckets;
    }

    inline bool
    kh_exist(khint_t x) const
    {
        return !iseither(this->flags, x);
    }

    inline KEY
    kh_key(khint_t x) const
    {
        return this->keys[x];
    }

    inline VAL
    kh_val(khint_t x) const
    {
        return this->vals[x];
    }

public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;

        Iterator(HashMap * hash, khiter_t it) : hash(hash), it(it) {}

        Iterator &
        operator++()
        {
            next();
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
            return (a.hash == b.hash) && (a.it == b.it);
        }

        friend bool
        operator!=(const Iterator & a, const Iterator & b)
        {
            return (a.hash != b.hash) || (a.it != b.it);
        }

    private:
        inline void
        next()
        {
            do {
                ++this->it;
            } while (this->it != this->hash->kh_end() && !this->hash->kh_exist(this->it));
        }

        HashMap<KEY, VAL, HASH_FN, HASH_EQUAL> * hash;
        khiter_t it;
    };
};

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::HashMap() :
    n_buckets(0),
    size(0),
    n_occupied(0),
    upper_bound(0),
    flags(nullptr),
    keys(nullptr),
    vals(nullptr)
{
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::create(Int n)
{
    CALL_STACK_MSG();
    if (n == 0) {
        this->flags = nullptr;
        this->keys = nullptr;
        this->vals = nullptr;

        this->n_buckets = 0;
        this->size = 0;
        this->n_occupied = 0;
        this->upper_bound = 0;
    }
    else
        resize(n);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::destroy()
{
    CALL_STACK_MSG();
    if (this->keys != nullptr) {
        free((void *) this->keys);
        free(this->flags);
        free((void *) this->vals);

        this->keys = nullptr;
        this->flags = nullptr;
        this->vals = nullptr;
    }
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::reset()
{
    CALL_STACK_MSG();
    if (this->keys != nullptr) {
        free(this->keys);
        free(this->flags);
        free(this->vals);

        this->n_buckets = 0;
        this->size = 0;
        this->n_occupied = 0;
        this->upper_bound = 0;

        this->keys = nullptr;
        this->flags = nullptr;
        this->vals = nullptr;
    }
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::clear()
{
    CALL_STACK_MSG();
    if (this->flags) {
        memset(this->flags, 0xaa, fsize(this->n_buckets) * sizeof(khint32_t));
        this->size = this->n_occupied = 0;
    }
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
int
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::resize(Int nb)
{
    CALL_STACK_MSG();
    /* This function uses 0.25*n_buckets bytes of working space instead of
     * [sizeof(key_t+val_t)+.25]*n_buckets. */
    khint32_t * new_flags = nullptr;
    khint_t j = 1;
    khint_t new_n_buckets = nb;
    {
        kroundup32(new_n_buckets);
        if (new_n_buckets < 4)
            new_n_buckets = 4;
        if (this->size >= (khint_t) (new_n_buckets * HASH_UPPER + 0.5))
            /* requested size is too small */
            j = 0;
        else {
            /* hash table size to be changed (shrink or expand); rehash */
            new_flags = (khint32_t *) malloc(fsize(new_n_buckets) * sizeof(khint32_t));
            if (!new_flags)
                return -1;
            memset(new_flags, 0xaa, fsize(new_n_buckets) * sizeof(khint32_t));
            if (this->n_buckets < new_n_buckets) { /* expand */
                KEY * new_keys = (KEY *) realloc((void *) this->keys, new_n_buckets * sizeof(KEY));
                if (!new_keys) {
                    free(new_flags);
                    return -1;
                }
                this->keys = new_keys;
                VAL * new_vals = (VAL *) realloc((void *) this->vals, new_n_buckets * sizeof(VAL));
                if (!new_vals) {
                    free(new_flags);
                    return -1;
                }
                this->vals = new_vals;
            }
            /* otherwise shrink */
        }
    }
    if (j) {
        /* rehashing is needed */
        for (j = 0; j != this->n_buckets; ++j) {
            if (iseither(this->flags, j) == 0) {
                KEY key = this->keys[j];
                VAL val;
                khint_t new_mask;
                new_mask = new_n_buckets - 1;
                val = this->vals[j];
                set_isdel_true(this->flags, j);
                while (1) {
                    /* kick-out process; sort of like in Cuckoo hashing */
                    khint_t k, i, step = 0;
                    k = HASH_FN(key);
                    i = k & new_mask;
                    while (!isempty(new_flags, i))
                        i = (i + (++step)) & new_mask;
                    set_isempty_false(new_flags, i);
                    if (i < this->n_buckets && iseither(this->flags, i) == 0) {
                        /* kick out the existing element */
                        {
                            KEY tmp = this->keys[i];
                            this->keys[i] = key;
                            key = tmp;
                        }
                        VAL tmp = this->vals[i];
                        this->vals[i] = val;
                        val = tmp;
                        /* mark it as deleted in the old hash table */
                        set_isdel_true(this->flags, i);
                    }
                    else {
                        /* write the element and jump out of the loop */
                        this->keys[i] = key;
                        this->vals[i] = val;
                        break;
                    }
                }
            }
        }
        if (this->n_buckets > new_n_buckets) {
            /* shrink the hash table */
            this->keys = (KEY *) realloc((void *) this->keys, new_n_buckets * sizeof(KEY));
            this->vals = (VAL *) realloc((void *) this->vals, new_n_buckets * sizeof(VAL));
        }
        free(this->flags);
        /* free the working space */
        this->flags = new_flags;
        this->n_buckets = new_n_buckets;
        this->n_occupied = this->size;
        this->upper_bound = (khint_t) (this->n_buckets * HASH_UPPER + 0.5);
    }
    return 0;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
Int
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::get_size() const
{
    CALL_STACK_MSG();
    return (Int) this->size;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
Int
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::get_capacity() const
{
    CALL_STACK_MSG();
    return (Int) this->n_buckets;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
bool
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::has(const KEY & key)
{
    CALL_STACK_MSG();
    khiter_t iter = kh_get(key);
    return (iter != kh_end());
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
const VAL &
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::get(const KEY & key) const
{
    CALL_STACK_MSG();
    khiter_t iter = kh_get(key);
    if (iter != kh_end())
        return this->vals[iter];
    else
        throw std::out_of_range("Key not found");
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
VAL &
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::set(const KEY & key)
{
    CALL_STACK_MSG();
    int ret;
    khiter_t iter = kh_put(key, &ret);
    if (ret >= 0)
        return this->vals[iter];
    else
        throw std::invalid_argument("Invalid key");
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
void
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::del(const KEY & key)
{
    CALL_STACK_MSG();
    khiter_t iter = kh_get(key);
    kh_del(iter);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
bool
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::query_set(const KEY & key, const VAL & val)
{
    CALL_STACK_MSG();
    int ret;
    khiter_t iter = kh_put(key, &ret);
    assert(ret >= 0);
    this->vals[iter] = val;
    return ret != 0;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
bool
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::query_del(const KEY & key)
{
    CALL_STACK_MSG();
    khiter_t iter = kh_get(key);
    if (iter != kh_end()) {
        kh_del(iter);
        return true;
    }
    else
        return false;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
typename HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::Iterator
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::find(const KEY & key)
{
    CALL_STACK_MSG();
    khint_t iter = kh_get(key);
    return Iterator(this, iter);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
typename HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::Iterator
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::put(const KEY & key)
{
    CALL_STACK_MSG();
    int ret;
    khint_t iter = kh_put(key, &ret);
    assert(ret >= 0);
    return Iterator(this, iter);
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
std::vector<KEY>
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::get_keys(Int off)
{
    CALL_STACK_MSG();
    std::vector<KEY> keys;
    keys.resize((std::size_t) get_size());
    Int pos = off;
    for (khint_t it = kh_begin(); it != kh_end(); ++it) {
        if (kh_exist(it))
            keys[pos++] = kh_key(it);
    }
    return keys;
}

template <class KEY, class VAL, class HASH_FN, class HASH_EQUAL>
std::vector<VAL>
HashMap<KEY, VAL, HASH_FN, HASH_EQUAL>::get_vals(Int off)
{
    CALL_STACK_MSG();
    std::vector<VAL> vals;
    vals.resize((std::size_t) get_size());
    Int pos = off;
    for (khint_t it = kh_begin(); it != kh_end(); ++it) {
        if (kh_exist(it))
            vals[pos++] = kh_val(it);
    }
    return vals;
}

} // namespace godzilla
