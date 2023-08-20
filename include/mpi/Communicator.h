#pragma once

#include <mpi.h>
#include <vector>
#include "GodzillaConfig.h"
#include "Datatype.h"
#include "Status.h"
#include "Request.h"
#include "Operation.h"
#include "../Error.h"

namespace godzilla {

namespace mpi {

class Communicator {
public:
    /// Create MPI_COMM_WORLD communicator
    Communicator();
    /// Create communicator from an `MPI_Comm` one
    explicit Communicator(const MPI_Comm & comm);
    Communicator(const Communicator & comm);

    /// Determine the rank of the executing process in a communicator
    int rank() const;

    /// Determine the number of processes in a communicator
    int size() const;

    /// Send data to another process
    template <typename T>
    void send(int dest, int tag, const T & value) const;
    template <typename T>
    void send(int dest, int tag, const T * values, int n) const;
    template <typename T, typename A>
    void send(int dest, int tag, const std::vector<T, A> & value) const;

    /// Send a message to another process without any data
    void send(int dest, int tag) const;

    /// Receive data from a remote process
    template <typename T>
    Status recv(int source, int tag, T & value) const;
    template <typename T>
    Status recv(int source, int tag, T * values, int n) const;
    template <typename T, typename A>
    Status recv(int source, int tag, std::vector<T, A> & value) const;

    /// Receive a message from a remote process without any data
    Status recv(int source, int tag) const;

    /// Send a message to a remote process without blocking
    template <typename T>
    Request isend(int dest, int tag, const T & value) const;
    template <typename T>
    Request isend(int dest, int tag, const T * values, int n) const;

    template <typename T>
    Request irecv(int source, int tag, T & value) const;
    template <typename T>
    Request irecv(int source, int tag, T * values, int n) const;

    /// Wait for all processes within a communicator to reach the barrier.
    void barrier() const;

    /// Broadcast a value from a root process to all other processes
    template <typename T>
    void broadcast(T & value, int root);
    template <typename T>
    void broadcast(T * values, int n, int root);

    /// Gather
    template <typename T>
    void gather(const T & in_value, T * out_values, int root);
    template <typename T>
    void gather(const T & in_value, std::vector<T> & out_values, int root);
    template <typename T>
    void gather(const T * in_values, int n, T * out_values, int root);
    template <typename T>
    void gather(const T * in_values, int n, std::vector<T> & out_values, int root);

    /// Scatter
    template <typename T>
    void scatter(const T * in_values, T & out_value, int root);
    template <typename T>
    void scatter(const std::vector<T> & in_values, T & out_value, int root);
    template <typename T>
    void scatter(const T * in_values, T * out_values, int n, int root);
    template <typename T>
    void scatter(const std::vector<T> & in_values, T * out_values, int n, int root);

    /// Reduce
    template <typename T, typename Op>
    void reduce(const T * in_values, int n, T * out_values, Op op, int root);
    template <typename T, typename Op>
    void reduce(const T * in_values, int n, Op op, int root);
    template <typename T, typename Op>
    void reduce(std::vector<T> const & in_values, Op op, int root);
    template <typename T, typename Op>
    void reduce(std::vector<T> const & in_values, std::vector<T> & out_values, Op op, int root);
    template <typename T, typename Op>
    void reduce(const T & in_value, T & out_value, Op op, int root);

    /// All reduce
    template <typename T, typename Op>
    void all_reduce(const T * in_values, int n, T * out_values, Op op);
    template <typename T, typename Op>
    void all_reduce(const T & in_value, T & out_value, Op op);
    template <typename T, typename Op>
    void all_reduce(T & value, Op op);

    /// Abort all tasks in the group of this communicator.
    void abort(int errcode) const;

    operator MPI_Comm() const;

private:
    MPI_Comm comm;
};

//

inline Communicator::Communicator() : comm(MPI_COMM_WORLD) {}

inline Communicator::Communicator(const MPI_Comm & comm) : comm(comm) {}

inline Communicator::Communicator(const Communicator & comm) : comm(comm.comm) {}

inline int
Communicator::rank() const
{
    int r;
    MPI_Comm_rank(this->comm, &r);
    return r;
}

inline int
Communicator::size() const
{
    int sz;
    MPI_Comm_size(this->comm, &sz);
    return sz;
}

// Send

template <typename T>
void
Communicator::send(int dest, int tag, const T & value) const
{
    send(dest, tag, &value, 1);
}

template <typename T>
void
Communicator::send(int dest, int tag, const T * values, int n) const
{
    assert(values != nullptr);
    MPI_CHECK(MPI_Send(const_cast<T *>(values), n, get_mpi_datatype<T>(), dest, tag, this->comm));
}

template <typename T, typename A>
void
Communicator::send(int dest, int tag, const std::vector<T, A> & value) const
{
    typename std::vector<T, A>::size_type size = value.size();
    send(dest, tag, size);
    send(dest, tag, value.data(), size);
}

inline void
Communicator::send(int dest, int tag) const
{
    MPI_CHECK(MPI_Send(MPI_BOTTOM, 0, MPI_PACKED, dest, tag, this->comm));
}

// Recv

template <typename T>
Status
Communicator::recv(int source, int tag, T & value) const
{
    return recv(source, tag, &value, 1);
}

template <typename T>
Status
Communicator::recv(int source, int tag, T * values, int n) const
{
    assert(values != nullptr);
    MPI_Status stat;
    MPI_CHECK(MPI_Recv(const_cast<T *>(values),
                       n,
                       get_mpi_datatype<T>(),
                       source,
                       tag,
                       this->comm,
                       &stat));
    return { stat };
}

template <typename T, typename A>
Status
Communicator::recv(int source, int tag, std::vector<T, A> & values) const
{
    typename std::vector<T, A>::size_type size = 0;
    recv(source, tag, size);
    values.resize(size);
    return recv(source, tag, values.data(), size);
}

inline Status
Communicator::recv(int source, int tag) const
{
    MPI_Status stat;
    MPI_CHECK(MPI_Recv(MPI_BOTTOM, 0, MPI_PACKED, source, tag, this->comm, &stat));
    return { stat };
}

// Isend

template <typename T>
Request
Communicator::isend(int dest, int tag, const T & value) const
{
    return isend(dest, tag, &value, 1);
}

template <typename T>
Request
Communicator::isend(int dest, int tag, const T * values, int n) const
{
    GODZILLA_UNUSED(n);

    assert(values != nullptr);
    MPI_Request request;
    MPI_CHECK(MPI_Isend(const_cast<T *>(values),
                        1,
                        get_mpi_datatype<T>(),
                        dest,
                        tag,
                        this->comm,
                        &request));
    return { request };
}

// Irecv

template <typename T>
Request
Communicator::irecv(int source, int tag, T & value) const
{
    return irecv(source, tag, &value, 1);
}

template <typename T>
Request
Communicator::irecv(int source, int tag, T * values, int n) const
{
    assert(values != nullptr);
    MPI_Request request;
    MPI_CHECK(MPI_Irecv(const_cast<T *>(values),
                        n,
                        get_mpi_datatype<T>(),
                        source,
                        tag,
                        this->comm,
                        &request));
    return { request };
}

// Barrier

inline void
Communicator::barrier() const
{
    MPI_CHECK(MPI_Barrier(this->comm));
}

// Broadcast

template <typename T>
void
Communicator::broadcast(T & value, int root)
{
    broadcast(&value, 1, root);
}

template <typename T>
void
Communicator::broadcast(T * values, int n, int root)
{
    MPI_CHECK(MPI_Bcast(values, n, get_mpi_datatype<T>(), root, this->comm));
}

// Gather

template <typename T>
void
Communicator::gather(const T & in_value, T * out_values, int root)
{
    assert(out_values || (rank() != root));
    gather(&in_value, 1, out_values, root);
}

template <typename T>
void
Communicator::gather(const T & in_value, std::vector<T> & out_values, int root)
{
    if (rank() == root)
        out_values.resize(size());
    gather(in_value, out_values.data(), root);
}

template <typename T>
void
Communicator::gather(const T * in_values, int n, T * out_values, int root)
{
    auto type = get_mpi_datatype<T>();
    MPI_CHECK(
        MPI_Gather(const_cast<T *>(in_values), n, type, out_values, n, type, root, this->comm));
}

template <typename T>
void
Communicator::gather(const T * in_values, int n, std::vector<T> & out_values, int root)
{
    if (rank() == root)
        out_values.resize(size() * (std::size_t) n);
    gather(in_values, n, out_values.data(), root);
}

// Scatter

template <typename T>
void
Communicator::scatter(const T * in_values, T & out_value, int root)
{
    scatter(in_values, &out_value, 1, root);
}

template <typename T>
void
Communicator::scatter(const std::vector<T> & in_values, T & out_value, int root)
{
    scatter(in_values.data(), &out_value, 1, root);
}

template <typename T>
void
Communicator::scatter(const T * in_values, T * out_values, int n, int root)
{
    auto type = get_mpi_datatype<T>();
    MPI_CHECK(
        MPI_Scatter(const_cast<T *>(in_values), n, type, out_values, n, type, root, this->comm));
}

template <typename T>
void
Communicator::scatter(const std::vector<T> & in_values, T * out_values, int n, int root)
{
    scatter(in_values.data(), out_values, n, root);
}

// Reduce

template <typename T, typename Op>
void
Communicator::reduce(const T * in_values, int n, T * out_values, Op, int root)
{
    MPI_Op op = mpi::op::Operation<Op, T>::op();
    MPI_CHECK(MPI_Reduce(const_cast<T *>(in_values),
                         out_values,
                         n,
                         mpi::get_mpi_datatype<T>(),
                         op,
                         root,
                         this->comm));
}

template <typename T, typename Op>
void
Communicator::reduce(const T * in_values, int n, Op op, int root)
{
    reduce(in_values, n, nullptr, op, root);
}

template <typename T, typename Op>
void
Communicator::reduce(std::vector<T> const & in_values, Op op, int root)
{
    reduce(in_values.data(), in_values.size(), op, root);
}

template <typename T, typename Op>
void
Communicator::reduce(std::vector<T> const & in_values, std::vector<T> & out_values, Op op, int root)
{
    if (root == rank())
        out_values.resize(in_values.size());
    reduce(in_values.data(), in_values.size(), out_values.data(), op, root);
}

template <typename T, typename Op>
void
Communicator::reduce(const T & in_value, T & out_value, Op op, int root)
{
    reduce(&in_value, 1, &out_value, op, root);
}

// All reduce

template <typename T, typename Op>
void
Communicator::all_reduce(const T * in_values, int n, T * out_values, Op)
{
    MPI_Op op = mpi::op::Operation<Op, T>::op();
    MPI_CHECK(MPI_Allreduce(const_cast<T *>(in_values),
                            out_values,
                            n,
                            mpi::get_mpi_datatype<T>(),
                            op,
                            this->comm));
}

template <typename T, typename Op>
void
Communicator::all_reduce(const T & in_value, T & out_value, Op op)
{
    all_reduce(&in_value, 1, &out_value, op);
}

template <typename T, typename Op>
void
Communicator::all_reduce(T & in_value, Op op)
{
    T out_value;
    all_reduce(&in_value, 1, &out_value, op);
    in_value = out_value;
}

//

inline void
Communicator::abort(int errcode) const
{
    MPI_Abort(this->comm, errcode);
}

inline Communicator::operator MPI_Comm() const
{
    return this->comm;
}

} // namespace mpi

} // namespace godzilla
