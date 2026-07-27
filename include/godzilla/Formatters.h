// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Utils.h"
#include <petscsystypes.h>
#include <fmt/format.h>
#include <filesystem>

namespace fs = std::filesystem;

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
        }
        godzilla::utils::unreachable();
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

template <>
struct fmt::formatter<godzilla::Dimension> {
    constexpr auto
    parse(fmt::format_parse_context & ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const godzilla::Dimension & dim, FormatContext & ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", static_cast<godzilla::Int>(dim));
    }
};

template <>
struct fmt::formatter<fs::path> {
    constexpr auto
    parse(fmt::format_parse_context & ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const fs::path & path, FormatContext & ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", path.string());
    }
};

template <>
struct fmt::formatter<godzilla::PolytopeType> {
    constexpr auto
    parse(fmt::format_parse_context & ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(godzilla::PolytopeType e, FormatContext & ctx) const
    {
        switch (e) {
        case godzilla::PolytopeType::POINT:
            return fmt::format_to(ctx.out(), "POINT");
        case godzilla::PolytopeType::SEGMENT:
            return fmt::format_to(ctx.out(), "SEGMENT");
        case godzilla::PolytopeType::POINT_PRISM_TENSOR:
            return fmt::format_to(ctx.out(), "POINT_PRISM_TENSOR");
        case godzilla::PolytopeType::TRIANGLE:
            return fmt::format_to(ctx.out(), "TRIANGLE");
        case godzilla::PolytopeType::QUADRILATERAL:
            return fmt::format_to(ctx.out(), "QUADRILATERAL");
        case godzilla::PolytopeType::SEG_PRISM_TENSOR:
            return fmt::format_to(ctx.out(), "SEG_PRISM_TENSOR");
        case godzilla::PolytopeType::TETRAHEDRON:
            return fmt::format_to(ctx.out(), "TETRAHEDRON");
        case godzilla::PolytopeType::HEXAHEDRON:
            return fmt::format_to(ctx.out(), "HEXAHEDRON");
        case godzilla::PolytopeType::PYRAMID:
            return fmt::format_to(ctx.out(), "PYRAMID");
        case godzilla::PolytopeType::TRI_PRISM:
            return fmt::format_to(ctx.out(), "TRI_PRISM");
        case godzilla::PolytopeType::TRI_PRISM_TENSOR:
            return fmt::format_to(ctx.out(), "TRI_PRISM_TENSOR");
        case godzilla::PolytopeType::QUAD_PRISM_TENSOR:
            return fmt::format_to(ctx.out(), "QUAD_PRISM_TENSOR");
        case godzilla::PolytopeType::FV_GHOST:
            return fmt::format_to(ctx.out(), "FV_GHOST");
        case godzilla::PolytopeType::INTERIOR_GHOST:
            return fmt::format_to(ctx.out(), "INTERIOR_GHOST");
        default:
            return fmt::format_to(ctx.out(), "Unknow");
        }
        godzilla::utils::unreachable();
    }
};
