// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Preconditioner.h"
#include "godzilla/Types.h"

namespace godzilla {

class PCComposite : public Preconditioner {
public:
    enum Type {
        ADDITIVE = PC_COMPOSITE_ADDITIVE,
        MULTIPLICATIVE = PC_COMPOSITE_MULTIPLICATIVE,
        SYMMETRIC_MULTIPLICATIVE = PC_COMPOSITE_SYMMETRIC_MULTIPLICATIVE,
        SPECIAL = PC_COMPOSITE_SPECIAL,
        SCHUR = PC_COMPOSITE_SCHUR,
        GKB = PC_COMPOSITE_GKB
    };

    PCComposite();
    PCComposite(PC pc);

    /// Creates a preconditioner
    ///
    /// @param comm MPI communicator
    void create(MPI_Comm comm);

    /// Sets the type of composite preconditioner
    ///
    void set_type(Type type);

    /// Gets the type of composite preconditioner
    ///
    Type get_type() const;

    /// Adds another PC to the composite PC.
    ///
    /// @param subpc The new preconditioner
    void add_pc(const Preconditioner & subpc);

    /// Gets the number of PC objects in the composite PC
    ///
    /// @return The number of sub PCs
    Int get_number_pc() const;

    /// Gets one of the PC objects in the composite PC
    ///
    /// @param n The number of the PC requested
    /// @return The PC requested
    Preconditioner get_pc(Int n) const;

    /// Sets alpha for the special composite preconditioner
    void special_set_alpha(Scalar alpha);
};

} // namespace godzilla
