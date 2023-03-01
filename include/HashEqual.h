#pragma once

#include "Types.h"

namespace godzilla {

/// Template for determining the equality of 2 hashes
template <class T>
class HashEqual {
};

template <>
class HashEqual<Int> {
public:
    HashEqual(Int a, Int b) { this->equal = (a == b); }

    operator bool() const { return this->equal; }

private:
    bool equal;
};

} // namespace godzilla
