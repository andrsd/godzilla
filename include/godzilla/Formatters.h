// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Exception.h"
#include <petscsystypes.h>
#include <fmt/format.h>

template <>
struct fmt::formatter<InsertMode> {
    constexpr auto
    parse(fmt::format_parse_context & ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(InsertMode e, FormatContext & ctx) const
    {
        switch (e) {
        case NOT_SET_VALUES:
            return fmt::format_to(ctx.out(), "NOT_SET_VALUES");
        case INSERT_VALUES:
            return fmt::format_to(ctx.out(), "INSERT_VALUES");
        case ADD_VALUES:
            return fmt::format_to(ctx.out(), "ADD_VALUES");
        case MAX_VALUES:
            return fmt::format_to(ctx.out(), "MAX_VALUES");
        case MIN_VALUES:
            return fmt::format_to(ctx.out(), "MIN_VALUES");
        case INSERT_ALL_VALUES:
            return fmt::format_to(ctx.out(), "INSERT_ALL_VALUES");
        case ADD_ALL_VALUES:
            return fmt::format_to(ctx.out(), "ADD_ALL_VALUES");
        case INSERT_BC_VALUES:
            return fmt::format_to(ctx.out(), "INSERT_BC_VALUES");
        case ADD_BC_VALUES:
            return fmt::format_to(ctx.out(), "ADD_BC_VALUES");
        default:
            throw godzilla::Exception("Unknown InsertMode");
        }
    }
};

template <>
struct fmt::formatter<godzilla::FieldID> {
    constexpr auto
    parse(fmt::format_parse_context & ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const godzilla::FieldID & e, FormatContext & ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", e.value());
    }
};
