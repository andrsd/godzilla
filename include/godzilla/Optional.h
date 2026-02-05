// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <optional>

namespace godzilla {

template <typename T>
using Optional = std::optional<T>;

template <typename T>
concept IsOptional = requires {
    typename T::value_type;
    requires std::same_as<T, Optional<typename T::value_type>>;
};

} // namespace godzilla
