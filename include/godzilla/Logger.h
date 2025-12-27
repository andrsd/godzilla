// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Terminal.h"
#include "godzilla/String.h"
#include "fmt/core.h"
#include <array>
#include <stdexcept>

namespace godzilla {

/// Class for logging errors and warnings
class Logger {
    /// Type of the message
    enum class Type {
        /// Error
        ERROR,
        /// Warning
        WARNING,
        ///
        Count
    };

    constexpr static std::array<std::string_view, static_cast<size_t>(Type::Count)> type_name = {
        "ERROR",
        "WARNING"
    };

    constexpr std::string_view
    to_string(Type c)
    {
        return Logger::type_name[static_cast<size_t>(c)];
    }

public:
    Logger();

    /// Log an error
    template <typename... T>
    void
    error(fmt::format_string<T...> format, T... args)
    {
        error(String(""), format, std::forward<T>(args)...);
    }

    template <typename... T>
    void
    error(const String prefix, fmt::format_string<T...> format, T... args)
    {
        auto str = format_msg(Type::ERROR, prefix, format, std::forward<T>(args)...);
        fmt::println(stderr, "{}{}{}", Terminal::red, str, Terminal::normal);
        ++this->num_errors;
    }

    /// Log a warning
    template <typename... T>
    void
    warning(fmt::format_string<T...> format, T... args)
    {
        warning(String(""), format, std::forward<T>(args)...);
    }

    template <typename... T>
    void
    warning(const String prefix, fmt::format_string<T...> format, T... args)
    {
        auto str = format_msg(Type::WARNING, prefix, format, std::forward<T>(args)...);
        fmt::println(stderr, "{}{}{}", Terminal::yellow, str, Terminal::normal);
        ++this->num_warnings;
    }

    /// Get the number of logged errors/warnings
    ///
    /// @return Number of logger errors/warnings
    std::size_t get_num_entries() const;

    /// Get the number of errors
    ///
    /// @return Number of errors
    std::size_t get_num_errors() const;

    /// Get the number of warnings
    ///
    /// @return Number of warnings
    std::size_t get_num_warnings() const;

    /// Print logged errors and warnings
    void print() const;

protected:
    template <typename... T>
    String
    format_msg(Type type, const String prefix, fmt::format_string<T...> format, T... args)
    {
        String str;
        str.append(fmt::format("[{}] ", to_string(type)));
        if (prefix.length() > 0)
            str.append(fmt::format("{}: ", prefix));
        str.append(fmt::format(format, std::forward<T>(args)...));
        return str;
    }

private:
    /// Number of errors
    std::size_t num_errors;
    /// Number of warnings
    std::size_t num_warnings;
};

} // namespace godzilla
