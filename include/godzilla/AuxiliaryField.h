// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Label.h"
#include "godzilla/Types.h"
#include "godzilla/DenseVector.h"

namespace godzilla {

class UnstructuredMesh;
class Problem;
class DiscreteProblemInterface;

/// Base class for auxiliary fields
///
class AuxiliaryField : public Object, public PrintInterface {
public:
    explicit AuxiliaryField(const Parameters & params);

    void create() override;

    [[nodiscard]] virtual Int get_block_id() const;

    [[nodiscard]] virtual const Label & get_label() const;

    [[nodiscard]] virtual const std::string & get_region() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    [[nodiscard]] virtual Int get_field_id() const;

    /// Get the number of constrained components
    ///
    /// @return The number of constrained components
    [[nodiscard]] virtual Int get_num_components() const = 0;

    [[nodiscard]] virtual PetscFunc * get_func() const = 0;

    virtual const void * get_context() const;

    virtual void evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) = 0;

    template <Int DIM>
    Real get_value(Real time, const DenseVector<Real, DIM> & x);

    template <Int N, Int DIM>
    DenseVector<Real, N> get_vector_value(Real time, const DenseVector<Real, DIM> & x);

protected:
    /// Get mesh this auxiliary field is defined on
    ///
    /// @return Mesh this auxiliary field is defined on
    UnstructuredMesh * get_mesh() const;

    /// Get problem this auxiliary field is part of
    ///
    /// @return Problem this auxiliary field of part of
    Problem * get_problem() const;

private:
    /// Discrete problem this object is part of
    DiscreteProblemInterface * dpi;

    /// Unstructured mesh this field is defined on
    UnstructuredMesh * mesh;

    /// Field name
    const std::string & field;

    /// Region name this auxiliary field is defined on
    const std::string & region;

    /// Field ID
    Int fid;

    /// Block here the auxiliary field lives
    Label label;

    /// Block ID associated with the label where this field is defined
    Int block_id;

public:
    static Parameters parameters();
};

template <Int DIM>
inline Real
AuxiliaryField::get_value(Real time, const DenseVector<Real, DIM> & x)
{
    Real val;
    evaluate(DIM, time, x.data(), 1, &val);
    return val;
}

template <Int N, Int DIM>
inline DenseVector<Real, N>
AuxiliaryField::get_vector_value(Real time, const DenseVector<Real, DIM> & x)
{
    DenseVector<Real, N> val;
    evaluate(DIM, time, x.data(), N, val.data());
    return val;
}

} // namespace godzilla
