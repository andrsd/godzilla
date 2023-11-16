#pragma once

#include "mpi.h"

namespace mpicpp_lite {

/// Wrapper around MPI_Status
class Status {
public:
    /// Construct empty `Status` object
    Status();

    /// Construct `Status` object from `MPI_Status` structure
    ///
    /// @param s MPI_Status object used to initialize this object
    Status(const MPI_Status & s);

    /// Get the source of the message
    ///
    /// @return Source of the message (i.e. rank ID)
    int source() const;

    /// Get the message tag
    ///
    /// @return Message tag
    int tag() const;

    /// Get the error code
    ///
    /// @return Error code
    int error() const;

    /// Type cast operators so we can pass this class directly into the MPI API
    operator MPI_Status *() { return &this->status; }

private:
    MPI_Status status;
};

inline Status::Status() : status({ 0 }) {}

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

} // namespace mpicpp_lite
