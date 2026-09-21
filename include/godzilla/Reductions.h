// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/IndexSet.h"
#include "godzilla/Range.h"

namespace godzilla {

/// Reducer that combines "computed" parts
///
/// This is what user see and use in their lambdas to compute the reduction
template <typename T, typename BinaryOperation>
class Reducer {
public:
    Reducer(T & local_val, BinaryOperation oper) : local_val_(local_val), oper_(oper) {}

    /// Combine using the user-provided binary op
    Reducer &
    combine(const T & partial)
    {
        this->local_val_ = this->oper_(this->local_val_, partial);
        return *this;
    }

    // Convenience operator overloads delegating to combine
    Reducer &
    operator+=(const T & partial)
    {
        combine(partial);
        return *this;
    }

    Reducer &
    operator*=(const T & partial)
    {
        combine(partial);
        return *this;
    }

private:
    T & local_val_;
    BinaryOperation oper_;
};

/// Reduction that is passed into the for_each loop
///
/// Stores "initial" value (or identity), reference to the final result,
/// and the binary operation for the reduction
template <typename T, typename BinaryOperation = std::plus<T>>
struct Reduction {
    T & result_ref;
    T identity;
    BinaryOperation combiner {};

    template <typename U>
        requires std::is_convertible_v<U, T>
    explicit Reduction(T & target,
                       U identity_val = U {},
                       BinaryOperation combiner = BinaryOperation {}) :
        result_ref(target),
        identity(identity_val),
        combiner(combiner)
    {
    }
};

/// Create `Reduction` class
///
/// @param var Reference to the variable that will hold the result
/// @param identity Identity (or initial value)
/// @param op Reduction operation
template <typename T, typename U, typename BinaryOperation>
    requires std::is_convertible_v<U, T>
auto
reduction(T & var, U identity, BinaryOperation op)
{
    return Reduction<T, BinaryOperation>(var, identity, op);
}

/// Run loop over range and compute reduction using the reduction operation
/// and a lambda that computes the local value
///
/// @param comm MPI communicator
/// @param range Range
/// @param red `Reduction` describing the reduction operation
/// @param fn Lambda computing local contributions
template <typename ReductionOp, typename Func>
void
for_each(mpi::Communicator comm, godzilla::Range range, ReductionOp red, Func && fn)
{
    using ValueType = decltype(red.identity);

    ValueType local_acc = red.identity;
    Reducer<ValueType, decltype(red.combiner)> reducer_handle(local_acc, red.combiner);
    for (auto idx : range) {
        fn(idx, reducer_handle);
    }
    comm.all_reduce(local_acc, red.combiner);
    red.result_ref = local_acc;
}

/// Run loop over index set and compute reduction using the reduction operation
/// and a lambda that computes the local value
///
/// @param comm MPI communicator
/// @param is IndexSet over which we iterate
/// @param red `Reduction` describing the reduction operation
/// @param fn Lambda computing local contributions
template <typename ReductionOp, typename Func>
void
for_each(mpi::Communicator comm, IndexSet is, ReductionOp red, Func && fn)
{
    using ValueType = decltype(red.identity);

    auto idxs = is.borrow_indices();
    ValueType local_acc = red.identity;
    Reducer<ValueType, decltype(red.combiner)> reducer_handle(local_acc, red.combiner);
    for (auto i : godzilla::make_range(idxs.size())) {
        fn(i, idxs[i], reducer_handle);
    }
    comm.all_reduce(local_acc, red.combiner);
    red.result_ref = local_acc;
}

} // namespace godzilla
