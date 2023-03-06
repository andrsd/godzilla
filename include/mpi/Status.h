#pragma once

#include "mpi.h"

namespace godzilla {

namespace mpi {

/// Wrapper around MPI_Status
class Status {
public:
    Status();
    Status(const MPI_Status & s);

    /// Get the source of the message
    int source() const;

    /// Get the message tag
    int tag() const;

    /// Get the error code
    int error() const;

    /// Type cast operators so we can pass this class directly into the MPI API
    operator MPI_Status &() { return this->status; }
    operator const MPI_Status &() const { return this->status; }

private:
    MPI_Status status;
};

inline Status::Status() {}

inline Status::Status(const MPI_Status & s) : status(s) {}

inline int
Status::source() const
{
    return this->status.MPI_SOURCE;
}

inline int
Status::tag() const
{
    return this->status.MPI_TAG;
}

inline int
Status::error() const
{
    return this->status.MPI_ERROR;
}

} // namespace mpi

} // namespace godzilla
