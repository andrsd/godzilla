#pragma once

#include <vector>
#include <string>
#include "Terminal.h"
#include "Error.h"
#include "fmt/color.h"

namespace godzilla {

/// Class for logging errors and warnings
class Logger {
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
        std::string str =
            format_msg(Terminal::Color::red, "[ERROR]", prefix, format, std::forward<T>(args)...);
        this->entries.push_back(str);
        this->num_errors++;
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
        std::string str = format_msg(Terminal::Color::yellow,
                                     "[WARNING]",
                                     prefix,
                                     format,
                                     std::forward<T>(args)...);
        this->entries.push_back(str);
        this->num_warnings++;
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
    format_msg(const Terminal::Color & color,
               const char * title,
               const std::string & prefix,
               fmt::format_string<T...> format,
               T... args)
    {
        std::string str;
        str += fmt::format("{}{} ", color, title);
        if (prefix.length() > 0)
            str += fmt::format("{}: ", prefix);
        str += fmt::format(format, std::forward<T>(args)...);
        str += fmt::format("{}", Terminal::Color::normal);
        return str;
    }

    /// List of logged errors/warnings
    std::vector<std::string> entries;
    /// Number of errors
    std::size_t num_errors;
    /// Number of warnings
    std::size_t num_warnings;
};

} // namespace godzilla
