// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <algorithm>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <fmt/core.h>

namespace godzilla {

/// This is a string (cpt. Obvious) that behaves like `std::string` with some sugar
class String : public std::string {
public:
    String() : std::string() {}

    String(const char * s) : std::string(s) {}

    String(const std::string & str) : std::string(str) {}

    /**
     * Convert supplied string to upper case.
     * @param name The string to convert upper case.
     */
    String
    to_upper() const
    {
        String upper(*this);
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        return upper;
    }

    /**
     * Convert supplied string to lower case.
     * @param name The string to convert upper case.
     */
    String
    to_lower() const
    {
        String lower(*this);
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower;
    }

    /**
     * Check if string ends with specified text
     *
     * @param suffix The suffix to test
     */
    bool
    ends_with(const String & suffix) const
    {
        return size() >= suffix.size() &&
               compare(size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    /**
     * Check if string starts with specified text
     *
     * @param prefix The prefix to test
     */
    bool
    starts_with(const String & prefix) const
    {
        return size() >= prefix.size() && compare(0, prefix.size(), prefix) == 0;
    }
};

} // namespace godzilla

template <>
struct fmt::formatter<godzilla::String> {
    constexpr auto
    parse(fmt::format_parse_context & ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const godzilla::String & str, FormatContext & ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", str.c_str());
    }
};

template <>
struct YAML::convert<godzilla::String> {
    static Node
    encode(const godzilla::String & rhs)
    {
        // do nothing, becuase we only read YML
        Node node;
        return node;
    }

    static bool
    decode(const Node & node, godzilla::String & rhs)
    {
        rhs = node.as<std::string>();
        return true;
    }
};
