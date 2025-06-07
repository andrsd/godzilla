// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Vector.h"

namespace godzilla {

/// Vector type consisting of nested subvectors, each stored separately.
class NestVector : public Vector {
public:
    NestVector();

    explicit NestVector(Vec vec);

    /// Returns the size of the nest vector.
    Int get_nest_size() const;

    /// Returns a single, sub-vector from a nest vector.
    ///
    /// @param idx Index of the vector within the nest
    Vector get_sub_vector(Int idx) const;

    /// Returns the entire array of vectors defining a nest vector.
    ///
    /// @return Array of vectors
    std::vector<Vector> get_sub_vectors() const;

    /// Set a single component vector in a nest vector at specified index.
    ///
    /// @param idx Index of the vector within the nest vector
    /// @param sx Vector at index `idx` within the nest vector
    void set_sub_vector(Int idx, Vector sx);

    /// Sets the component vectors at the specified indices in a nest vector.
    ///
    /// @param idx Indices of component vectors that are to be replaced
    /// @param sx Array of vectors
    void set_sub_vectors(const std::vector<Int> & idx, const std::vector<Vector> & sx);

    /// Duplicate the vector
    NestVector duplicate() const;
};

} // namespace godzilla
