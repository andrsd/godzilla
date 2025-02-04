// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <string>
#include "godzilla/Error.h"

namespace godzilla {

/// Class for logging errors and warnings
class Logger {
    /// Type of the message
    enum Type { ERROR, WARNING };

    /// Logger entry
    struct Entry {
        /// Type of the entry
        Type type;
        /// Text of the message
        std::string text;
    };

public:
    Logger();

    /// Log an error
    template <typename... T>
    void
    error(fmt::format_string<T...> format, T... args)
    {
        error(std::string(""), format, std::forward<T>(args)...);
    }

    template <typename... T>
    void
    error(const std::string & prefix, fmt::format_string<T...> format, T... args)
    {
        std::string str = format_msg("[ERROR]", prefix, format, std::forward<T>(args)...);
        this->entries.push_back({ ERROR, str });
        ++this->num_errors;
    }

    /// Log a warning
    template <typename... T>
    void
    warning(fmt::format_string<T...> format, T... args)
    {
        warning(std::string(""), format, std::forward<T>(args)...);
    }

    template <typename... T>
    void
    warning(const std::string & prefix, fmt::format_string<T...> format, T... args)
    {
        std::string str = format_msg("[WARNING]", prefix, format, std::forward<T>(args)...);
        this->entries.push_back({ WARNING, str });
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
    std::string
    format_msg(const std::string & title,
               const std::string & prefix,
               fmt::format_string<T...> format,
               T... args)
    {
        std::string str;
        str += fmt::format("{} ", title);
        if (prefix.length() > 0)
            str += fmt::format("{}: ", prefix);
        str += fmt::format(format, std::forward<T>(args)...);
        return str;
    }

private:
    /// List of logged errors/warnings
    std::vector<Entry> entries;
    /// Number of errors
    std::size_t num_errors;
    /// Number of warnings
    std::size_t num_warnings;
};

} // namespace godzilla
