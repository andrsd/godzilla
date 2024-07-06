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

    /// Get the spatial dimension of the problem this field is part of
    ///
    /// @return Spatial dimension of the problem
    [[nodiscard]] Int get_dimension() const;

    /// Get block ID this field is defined on
    ///
    /// @return Block ID
    [[nodiscard]] Int get_block_id() const;

    /// Get the `Label` this field is defined on
    ///
    /// @return Reference to a `Label`
    [[nodiscard]] const Label & get_label() const;

    /// Get the region name this field is defined on
    ///
    /// @return The region name
    [[nodiscard]] const std::string & get_region() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    [[nodiscard]] Int get_field_id() const;

    /// Get field name
    ///
    /// @return The field name
    [[nodiscard]] const std::string & get_field() const;

    /// Get the number of constrained components
    ///
    /// @return The number of constrained components
    [[nodiscard]] virtual Int get_num_components() const = 0;

    virtual void evaluate(Real time, const Real x[], Scalar u[]) = 0;

    template <Int DIM>
    Real get_value(Real time, const DenseVector<Real, DIM> & x);

    template <Int N, Int DIM>
    DenseVector<Real, N> get_vector_value(Real time, const DenseVector<Real, DIM> & x);

protected:
    /// Get mesh this auxiliary field is defined on
    ///
    /// @return Mesh this auxiliary field is defined on
    [[nodiscard]] UnstructuredMesh * get_mesh() const;

    /// Get problem this auxiliary field is part of
    ///
    /// @return Problem this auxiliary field of part of
    [[nodiscard]] Problem * get_problem() const;

private:
    /// Discrete problem this object is part of
    DiscreteProblemInterface * dpi;

    /// Unstructured mesh this field is defined on
    UnstructuredMesh * mesh;

    /// Field name
    std::string field;

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
    evaluate(time, x.data(), &val);
    return val;
}

template <Int N, Int DIM>
inline DenseVector<Real, N>
AuxiliaryField::get_vector_value(Real time, const DenseVector<Real, DIM> & x)
{
    DenseVector<Real, N> val;
    evaluate(time, x.data(), val.data());
    return val;
}

} // namespace godzilla
